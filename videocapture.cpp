#include "videocapture.h" // Déclaration de la classe VideoCapture.
#include <QDebug> // Utilisé pour la sortie des messages de debug.
#include <QDir> // Gestion des chemins et répertoires.
#include <opencv2/opencv.hpp> // Bibliothèque OpenCV principale.
#include <opencv2/highgui.hpp> // Fonctions pour la manipulation des fenêtres et des images.
#include <opencv2/imgproc.hpp> // Fonctions de traitement d'image OpenCV.
#include <opencv2/objdetect.hpp> // Pour la détection d'objets, comme les visages.
#include <QImage> // Représente des images en Qt.
#include <QBuffer> // Utilisé pour manipuler des données binaires en mémoire.
#include <QCoreApplication> // Gestion des propriétés globales de l'application Qt.
#include <QString> // Gestion de chaînes de caractères.
#include <QPixmap> // Manipulation d'images pour l'affichage dans l'interface Qt.
#include <chrono> // Mesure du temps (calcul du FPS).
#include <sstream> // Manipulation avancée des flux.
#include <vector> // Gestion des conteneurs de type vecteurs.
#include <iomanip> // Formattage précis des flux de sortie.
#include <thread>
// Constructeur de la classe VideoCapture
VideoCapture::VideoCapture(QObject *parent) : QObject(parent) ,m_filterMode(0) ,m_realFrameRate(0.0)  {
    cap.open(0);// Ouvre la caméra par défaut (index 0).
    frameTimer = new QTimer(this);// Création d'un timer pour capturer les frames périodiquement.
    connect(frameTimer, &QTimer::timeout, this, &VideoCapture::captureFrame);// Lier le timer à la méthode captureFrame.
    //Initialisez le détecteur de visages
    QString haarcascadePath = QCoreApplication::applicationDirPath() + "/haarcascade_frontalface_default.xml";
    if (!faceCascade.load(haarcascadePath.toStdString())) {
        qWarning() << "Erreur : Impossible de charger le modèle Haarcascade depuis :" << haarcascadePath;
    }
    // Initialisation
    frameWidth = 640;// Largeur par défaut des frames.
    frameHeight = 480;// Hauteur par défaut des frames.
    fps = 30;// Images par seconde.
    startCapture();// Démarre la capture vidéo à l'initialisation.
}
// Destructeur
VideoCapture::~VideoCapture() {
    if (cap.isOpened()) { // Vérifie si la caméra est ouverte
        cap.release(); // Libère les ressources liées à la caméra
    }
}
// Gestion des FPS
double VideoCapture::realFrameRate() const {
    return m_realFrameRate; // Retourne le FPS réel calculé
}
// Méthode pour vérifier si la capture est en cours
bool VideoCapture::isCapturing() const {
    return cap.isOpened(); // Renvoie vrai si la caméra est ouverte
}

// Démarrer la capture
void VideoCapture::startCapture() {
    // Démarre le timer pour mesurer les FPS réels
    elapsedTimer.start();

    if (!cap.open(0)) { // 0 : identifiant de la caméra par défaut
        qWarning("Erreur : Impossible d'accéder à la caméra !");
        return; // Arrête l'exécution si la caméra n'est pas accessible
    }

    // Configuration de la caméra
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1); // Définit la taille du buffer
    cap.set(cv::CAP_PROP_FRAME_WIDTH, frameWidth); // Largeur des images
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, frameHeight); // Hauteur des images
    cap.set(cv::CAP_PROP_FPS, fps); // Définit le nombre d'images par seconde

    // Affiche les propriétés actuelles de la caméra
    qDebug() << "Propriétés caméra : "
             << cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x"
             << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << "@"
             << cap.get(cv::CAP_PROP_FPS) << "FPS";

    if (!writer.isOpened()) { // Vérifie si le fichier vidéo est configuré
        qWarning("Attention : Aucun fichier vidéo n'a été configuré pour l'écriture !");
    }

    // Démarre un timer pour capturer les frames selon le FPS défini
    frameTimer->start(1000 / fps);
    m_isRecording = true; // Marque l'état comme en enregistrement
}

// Arrêter la capture
void VideoCapture::stopCapture() {
    if (writer.isOpened()) writer.release(); // Ferme le fichier vidéo si ouvert
    if (cap.isOpened()) cap.release(); // Libère la caméra si ouverte
    frameTimer->stop(); // Arrête le timer des frames
    m_realFrameRate = 0.0; // Réinitialise le FPS réel
    emit realFrameRateChanged(); // Signale le changement de FPS
    m_isRecording = false; // Met à jour l'état pour indiquer la fin de l'enregistrement
}

// Définir la résolution de la caméra
void VideoCapture::setResolution(int width, int height) {
    frameWidth = width; // Enregistre la largeur de l'image
    frameHeight = height; // Enregistre la hauteur de l'image
    qDebug() << "Résolution définie sur" << width << "x" << height;
}

// Définir le nombre d'images par seconde
void VideoCapture::setFPS(int fpsValue) {
    fps = fpsValue; // Enregistre le nouveau FPS
    qDebug() << "FPS défini sur" << fps;
}



void VideoCapture::setOutputFile(const QString &filePath) {
    QString absolutePath = QDir::currentPath() + "/" + filePath; // Crée le chemin absolu pour le fichier vidéo
    double fps = 7.5;  // FPS souhaité pour la sortie vidéo
    int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'); // Codec vidéo utilisé pour la compression

    // Ouvre le fichier de sortie pour enregistrer la vidéo
    writer.open(absolutePath.toStdString(), fourcc, fps, cv::Size(frameWidth, frameHeight));
    if (!writer.isOpened()) { // Vérifie si le fichier a été ouvert correctement
        qWarning() << "Erreur : Impossible d'ouvrir le fichier vidéo pour l'écriture !" << absolutePath;
        return;
    }
    qDebug() << "Fichier vidéo configuré pour la sortie :" << absolutePath;

    cv::VideoCapture cap(0); // Ouvre la caméra par défaut
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);  // Définit la largeur des frames capturées
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240); // Définit la hauteur des frames capturées
    cap.set(cv::CAP_PROP_FPS, 30);  // Définit les FPS de capture

    if (!cap.isOpened()) { // Vérifie si la caméra a été ouverte correctement
        qWarning() << "Erreur lors de l'ouverture de la caméra !";
        return;
    }

    cv::CascadeClassifier face_cascade; // Initialise un classificateur pour la détection de visages
    if (!face_cascade.load("C:/chemin/vers/haarcascade_frontalface_default.xml")) { // Charge le fichier Haar pour la détection des visages
        qWarning() << "Erreur lors du chargement du classificateur de visages !";
        cap.release(); // Libère la caméra en cas d'échec
        return;
    }

    cv::Mat frame; // Matrice pour stocker chaque frame capturée
    auto lastTime = std::chrono::high_resolution_clock::now(); // Enregistre le temps initial pour gérer les FPS
    const double frameDuration = 1.0 / fps; // Calcul de la durée entre chaque frame (en secondes)

    try {
        while (cap.read(frame)) { // Lit les frames de la caméra en boucle
            if (frame.empty()) { // Vérifie si la frame est vide
                qWarning() << "Frame vide capturée !";
                break;
            }

            qDebug() << "Frame capturée : " << frame.rows << "x" << frame.cols; // Affiche les dimensions de la frame

            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // Convertit la frame en niveaux de gris

            std::vector<cv::Rect> faces; // Liste des visages détectés
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30)); // Détecte les visages dans l'image

            for (const auto &face : faces) { // Trace un rectangle autour de chaque visage détecté
                cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            }

            // Applique des filtres supplémentaires à la frame si nécessaire
            applyFilters(frame);

            // Calcul du temps écoulé depuis la dernière frame
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedTime = currentTime - lastTime;

            // Si le temps écoulé est inférieur à la durée d'une frame, attend
            if (elapsedTime.count() < frameDuration) {
                std::this_thread::sleep_for(std::chrono::duration<double>(frameDuration - elapsedTime.count()));
            }

            // Enregistre la frame dans le fichier vidéo
            writer.write(frame);

            lastTime = currentTime; // Met à jour le dernier temps enregistré
        }
    } catch (const std::exception &e) { // Gère les exceptions éventuelles
        qWarning() << "Exception capturée : " << e.what();
    }

    cap.release(); // Libère la caméra
    writer.release(); // Ferme le fichier vidéo
    qDebug() << "Capture vidéo terminée.";
}

// Vérifie si l'enregistrement vidéo est en cours
bool VideoCapture::isRecording() const {
    return m_isRecording; // Retourne l'état d'enregistrement
}

void VideoCapture::setRecording(bool recording) {
    if (m_isRecording != recording) { // Vérifie si l'état d'enregistrement a changé.
        m_isRecording = recording; // Met à jour l'état d'enregistrement.
        emit recordingChanged(); // Émet un signal pour notifier les observateurs du changement.
    }
}
void VideoCapture::captureFrame() {
    if (!cap.isOpened()) { // Vérifie si la caméra est ouverte.
        qWarning("Message : La caméra n'est pas ouverte !");
        return;
    }

    cv::Mat frame;
    cap >> frame; // Capture une image depuis la caméra

    if (frame.empty()) { // Vérifie si la frame capturée est vide.
        qWarning("Erreur : La caméra ne renvoie pas de frame !");
        return;

    }

    // Ajouter un log pour vérifier les dimensions de la frame
    qDebug() << "Dimensions de la frame : " << frame.rows << "x" << frame.cols;

    // Calcul du FPS avec std::chrono
    static auto lastTime = std::chrono::high_resolution_clock::now();// Initialise le temps précédent.
    auto currentTime = std::chrono::high_resolution_clock::now();// Temps actuel.
    double elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count(); // Temps écoulé en ms.
    lastTime = currentTime;

    if (elapsedMs > 0) { // Calcul du FPS.
        m_realFrameRate = 1000.0 / elapsedMs;
        emit realFrameRateChanged(); // Notifie les changements de FPS.
        qDebug() << "Real FPS:" << m_realFrameRate;
    }
    // Appliquer les filtres à la frame
    applyFilters(frame);

    // Convertir la frame de OpenCV en RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    // Convertir la frame de OpenCV en QImage
    if (frame.empty()) {
        qWarning("Erreur : La frame est vide après les filtres !");
        return;
    }
    QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    if (qimage.isNull()) { // Vérifie si la conversion a échoué.
        qWarning("Erreur : Conversion de cv::Mat en QImage a échoué !");
        return;
    }

    // Encoder en base64
    QByteArray buffer;// Prépare un buffer pour encoder l'image.
    QBuffer buf(&buffer);
    buf.open(QIODevice::WriteOnly);
    qimage.save(&buf, "JPEG"); // Sauvegarde l'image en format JPEG dans le buffer.
    m_frame = QString::fromLatin1(buffer.toBase64());// Encode en base64.
    emit frameChanged(); // Notifier le changement de frame


    // Record video with synchronized FPS
    if (m_isRecording  && writer.isOpened()) {// Vérifie si l'enregistrement est actif.
        writer.write(frame);// Enregistre la frame.
    }else {
        qWarning("Erreur : Le VideoWriter n'est pas ouvert !");
    }

    // Désactivez temporairement les filtres si besoin
    applyFilters(frame);

    // Redémarrer le timer pour la prochaine frame
    elapsedTimer.restart();
}
void VideoCapture::applyFilters(cv::Mat &frame) {
    // Appliquer les filtres
    if (m_filterMode == 0) {
        // Aucun filtre
        // Aucun changement
    } else if (m_filterMode == 1) {
        // Gris
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
    } else if (m_filterMode == 2) {
        // Inversion
        cv::bitwise_not(frame, frame);
    } else if (m_filterMode == 3) {
        // Gaussian
        cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
    } else if (m_filterMode == 4) {
        // Median
        cv::medianBlur(frame, frame, 5);
    } else if (m_filterMode == 5) {
        // CLAHE
        cv::Mat lab;
        cv::cvtColor(frame, lab, cv::COLOR_BGR2Lab);
        std::vector<cv::Mat> lab_planes;
        cv::split(lab, lab_planes);
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        clahe->apply(lab_planes[0], lab_planes[0]);
        cv::merge(lab_planes, lab);
        cv::cvtColor(lab, frame, cv::COLOR_Lab2BGR);
    } else if (m_filterMode == 6) {
        // Sobel
        cv::Mat sobelX, sobelY;
        cv::Sobel(frame, sobelX, CV_64F, 1, 0, 3);
        cv::Sobel(frame, sobelY, CV_64F, 0, 1, 3);
        cv::Mat sobel;
        cv::magnitude(sobelX, sobelY, sobel);
        sobel.convertTo(frame, CV_8U);
    } else if (m_filterMode == 7) {
        // Sel & Poivre
        for (int i = 0; i < 500; i++) {
            int x = rand() % frame.cols;
            int y = rand() % frame.rows;
            frame.at<cv::Vec3b>(y, x) = (i % 2 == 0) ? cv::Vec3b(255, 255, 255) : cv::Vec3b(0, 0, 0);
        }
    } else if (m_filterMode == 8) {
        // Histogram
        cv::normalize(frame, frame, 0, 255, cv::NORM_MINMAX);
    } else if (m_filterMode == 9) {
        // Canny Edge Detection
        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY); // Conversion en niveaux de gris
        cv::Canny(grayFrame, frame, 100, 200);
    } else if (m_filterMode == 10) {
        // Bilateral Filter
        cv::Mat filteredFrame;
        // Vérifiez si l'image est en niveaux de gris ou en couleur
        if (frame.channels() == 1) {
            // Image en niveaux de gris, OK
            cv::bilateralFilter(frame, filteredFrame, 9, 75, 75);
        } else if (frame.channels() == 3) {
            // Image couleur
            cv::bilateralFilter(frame, filteredFrame, 9, 75, 75);
        } else {
            // Format d'image non pris en charge
            qDebug() << "Format d'image non pris en charge pour Bilateral Filter.";
            return;
        }
        // Copie du résultat dans l'image originale
        frame = filteredFrame.clone();
    } else if (m_filterMode == 11) {
        // Laplacian Filter
        cv::Mat laplacianFrame;
        // Applique le filtre Laplacian (CV_16S pour capturer les valeurs négatives/positives des gradients)
        cv::Laplacian(frame, laplacianFrame, CV_16S, 3);
        // Convertit en un format affichable (CV_8U)
        cv::convertScaleAbs(laplacianFrame, frame);
    } else if (m_filterMode == 12) {
        // Sharpening
        cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        cv::filter2D(frame, frame, -1, kernel);
    } else if (m_filterMode == 13) {
        // Cartoon Effect
        cv::Mat gray, edges;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::medianBlur(gray, gray, 7);
        cv::adaptiveThreshold(gray, edges, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 2);
        cv::bitwise_and(frame, frame, frame, edges);
    } else if (m_filterMode == 14) {
        // Motion Blur
        cv::Mat kernel = cv::Mat::zeros(15, 15, CV_32F);
        kernel.at<float>(7, 7) = 1.0 / 15.0;
        cv::filter2D(frame, frame, -1, kernel);
    } else if (m_filterMode == 15) {
        // Emboss Effect
        cv::Mat kernel = (cv::Mat_<float>(3, 3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
        cv::filter2D(frame, frame, CV_8U, kernel);
    } else if (m_filterMode == 16) {
        // Sepia Effect
        cv::Mat sepiaKernel = (cv::Mat_<float>(3, 3) <<
                                   0.272, 0.534, 0.131,
                               0.349, 0.686, 0.168,
                               0.393, 0.769, 0.189);
        cv::transform(frame, frame, sepiaKernel);
    }else if (m_filterMode == 17) {
        detectFaces(frame); // Appliquer la détection des visages
    }
}
// Renvoie l'image actuelle encodée en base64
QString VideoCapture::frame() const {
    return m_frame;
}
// Définit le mode de filtre et affiche un message de débogage
void VideoCapture::setFilterMode(int mode) {
    m_filterMode = mode; // Enregistre le mode de filtre sélectionné
    qDebug() << "Mode de filtre défini sur :" << mode; // Log pour le suivi
}
// Fonction pour détecter une image via la caméra
void VideoCapture::detectImage() {
    if (!cap.isOpened()) { // Vérifie si la caméra est active
        qDebug() << "Erreur : La caméra n'est pas active.";
        return; // Arrête l'exécution si la caméra n'est pas active
    }
    cv::Mat frame; // Initialise une matrice pour stocker l'image capturée
    cap >> frame; // Capture une seule image de la caméra
    if (frame.empty()) { // Vérifie si la capture a réussi
        qDebug() << "Erreur : Frame vide.";
        return; // Arrête l'exécution si l'image est vide
    }
    // Applique des filtres à l'image capturée
    applyFilters(frame);
    // Enregistre l'image modifiée dans un fichier
    bool savedModified = cv::imwrite("C:/Users/RANIA/Documents/projetsQT/mauellopencv/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/debug/outimg_modified.jpg", frame);
    if (savedModified) { // Vérifie si l'image a été enregistrée avec succès
        qDebug() << "Image modifiée enregistrée avec succès sous 'outimg_modified.jpg'.";
    } else { // Avertit en cas d'échec de l'enregistrement
        qWarning() << "Erreur lors de l'enregistrement de l'image modifiée.";
    }
    // Convertit la frame de OpenCV (BGR) en format RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    // Crée un objet QImage à partir des données OpenCV
    QImage qimage((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    if (qimage.isNull()) { // Vérifie si la conversion en QImage a échoué
        qWarning("Erreur : Conversion de cv::Mat en QImage a échoué !");
        return; // Ignore l'image si la conversion échoue
    }
    // Prépare un buffer pour encoder l'image en base64
    QByteArray buffer; // Déclare un tableau de bytes
    QBuffer buf(&buffer); // Associe le buffer au tableau
    buf.open(QIODevice::WriteOnly); // Ouvre le buffer en mode écriture
    // Sauvegarde l'image dans le buffer au format JPEG
    qimage.save(&buf, "JPEG");
    // Encode les données du buffer en base64 et les stocke dans m_frame
    m_frame = QString::fromLatin1(buffer.toBase64());
    // Émet un signal pour notifier que l'image a changé
    emit frameChanged();
}
void VideoCapture::detectFaces(cv::Mat &frame) {
    // Vérifier si le cadre d'entrée est vide
    if (frame.empty()) {
        qWarning() << "Erreur : L'image est vide!";
        return; // Quitte la fonction si l'image est invalide
    }
    // Vérifier si le détecteur Haarcascade est chargé
    if (faceCascade.empty()) {
        qWarning("Erreur : Le détecteur Haarcascade n'est pas chargé !");
        return; // Quitte la fonction si le détecteur est introuvable
    }
    std::vector<cv::Rect> faces; // Stocke les rectangles des visages détectés
    cv::Mat gray; // Matrice pour stocker l'image convertie en niveaux de gris
    // Convertir l'image en niveaux de gris
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    // Améliorer le contraste de l'image pour de meilleures détections
    cv::equalizeHist(gray, gray);
    // Détecter les visages dans l'image avec le détecteur Haarcascade
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));
    // Parcourir chaque visage détecté et effectuer des actions
    for (size_t i = 0; i < faces.size(); ++i) {
        cv::Rect face = faces[i]; // Rectangle délimitant un visage
        cv::Scalar color = cv::Scalar(0, 255, 0); // Couleur verte pour dessiner le rectangle
        int thickness = 2; // Définir l'épaisseur des lignes du rectangle
        // Dessiner un rectangle autour du visage
        cv::rectangle(frame, face, color, thickness);
        // Ajouter un texte à côté du visage (index ou annotation)
        std::string text = "Visage " + std::to_string(i + 1); // Génère le texte
        int fontFace = cv::FONT_HERSHEY_SIMPLEX; // Police du texte
        double fontScale = 0.5; // Taille du texte
        int fontThickness = 1; // Épaisseur du texte
        cv::Point textOrg(face.x, face.y - 10); // Position du texte au-dessus du visage
        // Dessiner le texte (principal)
        cv::putText(frame, text, textOrg, fontFace, fontScale, color, fontThickness);
        // Ajouter une ombre au texte (optionnel)
        cv::Point shadowOffset(2, 2); // Décalage de l'ombre
        cv::putText(frame, text, textOrg + shadowOffset, fontFace, fontScale, cv::Scalar(0, 0, 0), fontThickness);
    }

    // Afficher le nombre de visages détectés dans la console
    qDebug() << "Nombre de visages détectés :" << faces.size();

    // Si l'enregistrement est activé et que le writer est ouvert, enregistrer la frame
    if (m_isRecording && writer.isOpened()) {
        writer.write(frame); // Écrit l'image actuelle dans la vidéo
    } else {
        qWarning("Erreur : Le VideoWriter n'est pas ouvert !");
    }
}
