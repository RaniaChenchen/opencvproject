#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "videocapture.h" // Inclut la classe VideoCapture définie par l'utilisateur.
#include <QQmlContext> // Fournit un accès au contexte de QML pour exposer des objets C++.
int main(int argc, char *argv[])// Fonction principale de l'application.
{
// Active la prise en charge des écrans haute résolution si Qt est inférieur à la version 6.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);// Instancie l'application graphique Qt.
    // Enregistrement du module VideoCapture
    qmlRegisterType<VideoCapture>("VideoCapture", 1, 0, "VideoCapture");
    VideoCapture videoCapture; // Crée une instance de la classe VideoCapture.
    QQmlApplicationEngine engine; // Crée le moteur pour charger les fichiers QML.
    // Exposition de l'objet VideoCapture à QML
    engine.rootContext()->setContextProperty("videoCapture", &videoCapture);
    // Définit l'URL du fichier QML principal à charger.
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(// Connecte un signal pour gérer les erreurs de chargement du fichier QML.
        &engine,// Émetteur : le moteur QML.
        &QQmlApplicationEngine::objectCreated,// Signal émis lorsque l'objet racine est créé.
        &app,// Récepteur : l'application principale.
        [url](QObject *obj, const QUrl &objUrl) {// Lambda pour vérifier l'état de chargement.
            if (!obj && url == objUrl)// Si l'objet racine n'est pas créé, on quitte l'application.
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);// Type de connexion pour exécuter la lambda dans une file d'attente.
    engine.load(url);// Charge le fichier QML spécifié.
    return app.exec();// Lance la boucle d'événements de l'application.
}
