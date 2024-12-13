# Inclusion des modules Qt nécessaires pour ce projet
QT += quick core gui
CONFIG += c++17#Utilise la norme C++17 pour la compilation.
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \# Inclusion des fichiers sources
        main.cpp \
        videocapture.cpp
RESOURCES += qml.qrc# Inclusion des fichiers de ressources
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin # Définit le chemin cible pour les fichiers binaires lors du déploiement sur un système QNX.
else: unix:!android: target.path = /opt/$${TARGET}/bin # Définit le chemin cible pour les binaires sur des systèmes Unix non Android.
!isEmpty(target.path): INSTALLS += target# Si le chemin cible est défini, ajoute 'target' à la liste des installations à déployer.
# Ajoute les fichiers d'en-tête au projet.
HEADERS += \
    videocapture.h # Inclut le fichier d'en-tête "videocapture.h" pour être utilisé dans le projet.
# Ajoute les bibliothèques OpenCV nécessaires pour Windows (MinGW).
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_calib3d490.dll# Lie la bibliothèque OpenCV pour la calibration 3D.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_features2d490.dll# Lie la bibliothèque pour les fonctionnalités 2D.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_flann490.dll# Lie la bibliothèque pour la recherche approximative des voisins.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_dnn490.dll# Lie la bibliothèque pour les réseaux de neurones profonds.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_objdetect490.dll # Lie la bibliothèque pour la détection d'objets.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_core490.dll# Lie la bibliothèque pour les fonctionnalités principales d'OpenCV.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_imgcodecs490.dll# Lie la bibliothèque pour les codecs d'images.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_imgproc490.dll # Lie la bibliothèque pour le traitement d'images.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_videoio490.dll# Lie la bibliothèque pour l'entrée/sortie vidéo.
win32: LIBS += -L$$PWD/../install/x64/mingw/lib/ -llibopencv_highgui490.dll# Lie la bibliothèque pour l'interface graphique de visualisation.
# Définit les chemins d'inclusion pour les fichiers d'en-tête.
INCLUDEPATH += $$PWD/../install/include# Ajoute le répertoire contenant les fichiers d'en-tête à la liste des chemins d'inclusion.
DEPENDPATH += $$PWD/../install/include# Définit le répertoire contenant les dépendances pour le suivi des changements.
