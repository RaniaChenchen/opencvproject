#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H
// Inclusion des bibliothèques nécessaires
#include <QObject> // Permet de créer des objets Qt avec fonctionnalités (signaux/slots, propriétés).
#include <opencv2/opencv.hpp> // Inclut les fonctions principales d'OpenCV.
#include <QString> // Gestion des chaînes de caractères dans Qt.
#include "opencv2/highgui/highgui.hpp" // Fonctions pour l'interface graphique OpenCV.
#include "opencv2/core/core.hpp" // Fonctions principales de base OpenCV.
#include "opencv2/imgproc/imgproc.hpp" // Fonctions pour le traitement d'image.
#include "opencv2/videoio/videoio.hpp" // Gestion de la capture vidéo.
#include "opencv2/imgcodecs/imgcodecs.hpp" // Gestion des codecs d'image.
#include <chrono> // Fournit des outils pour la gestion du temps.
#include <QTimer> // Gestion des minuteries dans Qt.
#include <QElapsedTimer> // Chronomètre pour mesurer des intervalles.
class VideoCapture : public QObject { // Déclaration de la classe héritant de QObject (nécessaire pour QML et signaux/slots).
    Q_OBJECT // Macro Qt pour activer les fonctionnalités spécifiques de QObject.
    // Propriétés accessibles depuis QML avec des getters et signaux de changement
    Q_PROPERTY(bool isCapturing READ isCapturing NOTIFY isCapturingChanged) // Indique si la capture est active.
    Q_PROPERTY(QString frame READ frame NOTIFY frameChanged) // Contient l'image capturée sous forme de chaîne.
    Q_PROPERTY(double realFrameRate READ realFrameRate NOTIFY realFrameRateChanged) // Fréquence d'images actuelle.
    Q_PROPERTY(bool isRecording READ isRecording WRITE setRecording NOTIFY recordingChanged) // Indique si l'enregistrement est actif.
    // Membres privés de la classe
    QElapsedTimer elapsedTimer; // Chronomètre pour mesurer des durées.
    cv::VideoCapture cap; // Objet OpenCV pour capturer des flux vidéo.
public:
    explicit VideoCapture(QObject *parent = nullptr); // Constructeur avec paramètre parent (nullptr par défaut).
    ~VideoCapture(); // Destructeur.
    // Méthodes accessibles depuis QML
    Q_INVOKABLE void startCapture(); // Démarrer la capture vidéo.
    Q_INVOKABLE void captureFrame(); // Capturer une image.
    Q_INVOKABLE void stopCapture(); // Arrêter la capture vidéo.
    Q_INVOKABLE void setOutputFile(const QString &filePath); // Définir le fichier de sortie pour l'enregistrement.
    Q_INVOKABLE bool isCapturing() const; // Vérifier si la capture est active.
    Q_INVOKABLE void setResolution(int width, int height); // Définir la résolution de la capture.
    Q_INVOKABLE void setFPS(int fps); // Définir les images par seconde.
    Q_INVOKABLE void setFilterMode(int mode); // Définir un mode de filtre (ex. : gris, inversion).
    Q_INVOKABLE void detectImage(); // Détecter une image dans le flux vidéo.
    QString frame() const; // Récupérer l'image capturée en tant que chaîne.
    double realFrameRate() const; // Récupérer la fréquence d'images actuelle.
    bool isRecording() const; // Vérifier si l'enregistrement est actif.
    Q_INVOKABLE void setRecording(bool recording); // Activer ou désactiver l'enregistrement.
    Q_INVOKABLE void applyFilters(cv::Mat &frame); // Appliquer des filtres sur une image donnée.
signals: // Déclaration des signaux pour notifier des changements
    void isCapturingChanged(); // Signal émis lorsque l'état de capture change.
    void frameChanged(); // Signal émis lorsque l'image capturée change.
    void frameChanged(const QString &frameData); // Version surchargée avec données d'image.
    void realFrameRateChanged(); // Signal émis lorsque la fréquence d'images change.
    void recordingChanged(); // Signal émis lorsque l'état d'enregistrement change.
private:// Membres privés pour la gestion de la capture et du traitement
    cv::VideoWriter writer; // Objet pour écrire des vidéos.
    int frameWidth = 640; // Largeur par défaut des images capturées.
    int frameHeight = 480; // Hauteur par défaut des images capturées.
    int fps = 30; // Fréquence d'images par défaut.
    QString m_frame; // Image capturée, encodée en Base64.
    int m_filterMode = 0; // Mode de filtre (0 = Aucun, 1 = Gris, 2 = Inversion).
    QString matToBase64(const cv::Mat &frame); // Convertir une image Mat en Base64.
    cv::CascadeClassifier faceCascade; // Classificateur pour la détection de visages.
    void detectFaces(cv::Mat &frame); // Méthode pour détecter les visages dans une image.
    double m_realFrameRate; // Stocker la fréquence d'images actuelle.
    QTimer *frameTimer; // Minuterie pour capturer des images périodiquement.
    bool m_isRecording = false; // Indique si l'enregistrement est actif.
};
#endif // VIDEOCAPTURE_H
