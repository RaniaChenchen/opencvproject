import QtQuick 2.15 // Importation des modules nécessaires pour l'interface utilisateur QtQuick
import QtQuick.Window 2.15 // Importation du module pour la gestion des fenêtres
import QtQuick.Controls 2.15 // Importation des contrôles d'interface utilisateur (comme les boutons, sliders, etc.)
import VideoCapture 1.0 // Importation du module pour accéder à la caméra et capturer des images
import QtQuick.Controls.Material 2.15 // Importation des éléments visuels pour le thème Material
import QtQuick.Layouts 1.15 // Importation des dispositions de mise en page pour organiser les éléments de l'interface
Window {
    width:  900 // Largeur de la fenêtre définie à 900 pixels
    height: 700 // Hauteur de la fenêtre définie à 700 pixels
    visible: true // La fenêtre est visible dès le démarrage
    title: qsTr("Camera PC avec OpenCV") // Titre de la fenêtre défini à "Hello World"
    Material.theme: Material.Dark // Utilisation du thème sombre pour l'interface
    Material.primary: Material.Blue // Définition de la couleur principale du thème à bleu
//Déclaration de la caméra
    VideoCapture {
        id: camera // Identifiant unique pour accéder à la caméra
        onFrameChanged: imageSource.source = frame // Lors du changement de cadre, mettre à jour l'image affichée avec la nouvelle image de la caméra
        onRecordingChanged: { // Lors de l'activation ou de la désactivation de l'enregistrement
            recordingIndicator.color = camera.isRecording ? "green" : "red" // Change la couleur de l'indicateur d'enregistrement selon l'état
        }
    }
//Zone de contrôle (Disposition avec RowLayout)
    Rectangle {
        anchors.fill: parent // Le rectangle remplit toute la fenêtre
        color: "#2E2E2E" // Couleur de fond du rectangle
        radius: 10 // Coins arrondis pour le rectangle
        border.color: "#555555" // Couleur de la bordure
        border.width: 2; // Largeur de la bordure
        RowLayout {
            anchors.fill: parent // La disposition RowLayout remplit également le parent
            spacing: 20 // Espacement de 20 pixels entre les éléments
           // Zone de contrôle de la caméra (Titre et paramètres)

            Column {
                spacing: 20 // Espacement vertical entre les éléments de la colonne
                anchors.left: parent.left // Ancrage à gauche du parent
                anchors.top: parent.top // Ancrage en haut du parent
                anchors.leftMargin: 20 // Marge gauche
                anchors.topMargin: 20 // Marge en haut
                Text {
                    text: "📹  Gestion de Caméra avec OpenCV et Qt Quick 😊" // Titre principal
                    font.pixelSize: 30 // Taille de la police
                    font.bold: true // Police en gras
                    color: "#FFFFFF" // Couleur du texte
                    anchors.horizontalCenter: parent.horizontalCenter // Centrage horizontal
                }
                Text {
                    text: "⚙ Paramètres" // Titre de la section des paramètres
                    font.pixelSize: 22
                    font.bold: true
                    color: "#FFFFFF"
                    anchors.leftMargin: 10 // Marge à gauche
                }
                // Paramètre de résolution (largeur et hauteur)
                Text {
                    text: "Résolution :" // Label pour la résolution
                    color: "#AAAAAA" // Couleur du texte
                    font.pixelSize: 15 // Taille de la police
                }
                Row {
                    spacing: 5 // Espacement horizontal entre les éléments
                    SpinBox {
                        id: widthBox // Zone de sélection de la largeur de la résolution
                        from: 320 // Valeur minimale
                        to: 1920 // Valeur maximale
                        value: 640 // Valeur initiale
                        stepSize: 1 // Incrément de la valeur
                        wrap: true // Permet de faire défiler la valeur de manière circulaire
                    }
                    Text {
                        text: "x" // Texte affichant "x" entre la largeur et la hauteur
                        color: "#AAAAAA"
                        font.pixelSize: 15
                    }
                    SpinBox {
                        id: heightBox // Zone de sélection de la hauteur de la résolution
                        from: 240
                        to: 1080
                        value: 480
                        stepSize: 1
                        wrap: true
                    }
                }

                // Contrôle du Frame Rate (taux de rafraîchissement)
                Column {
                    spacing: 10
                    Label {
                        text: "Frame Rate :" // Label pour le frame rate
                        font.pixelSize: 16
                        color: "#FFFFFF"
                    }

                    Slider {
                        id: frameRateSlider // Slider pour ajuster le frame rate
                        from: 1 // Valeur minimale
                        to: 60 // Valeur maximale
                        value: 30 // Valeur initiale
                        stepSize: 1
                        width: 200 // Largeur du slider
                        onValueChanged: { // Lors du changement de valeur
                            camera.frameRate = value; // Mettre à jour le frame rate de la caméra
                            captureTimer.interval = 1000 / value; // Ajuste l'intervalle du minuteur
                        }
                    }

                    Row {
                        spacing: 20
                        Label {
                            text: frameRateSlider.value + " FPS" // Affiche la valeur du frame rate
                            font.pixelSize: 16
                            color: "#FFFFFF"
                        }
                        Label {
                            text: "Real FPS: " + camera.realFrameRate.toFixed(2) // Affiche le frame rate réel de la caméra
                            font.pixelSize: 16
                            color: "#FFFFFF"
                        }
                    }
                }
            }
               // Zone d'affichage de l'image de la caméra
            Rectangle {
                anchors.centerIn: parent // Centrage du rectangle dans le parent
                width: imageSource.implicitWidth + 40 // Largeur du rectangle ajustée à l'image
                height: imageSource.implicitHeight + 40 // Hauteur du rectangle ajustée à l'image
                radius: 15 // Coins arrondis pour un design doux
                border.color: "#FFFFFF" // Couleur de la bordure
                border.width: 1
                gradient: Gradient { // Dégradé de couleur pour l'arrière-plan
                    GradientStop { position: 0; color: "#333333" }
                    GradientStop { position: 1; color: "#000000" }
                }
                Image {
                    id: imageSource // Affiche l'image capturée par la caméra
                    anchors.centerIn: parent // Centrage de l'image dans le rectangle
                    width: parent.width - 40 // Largeur de l'image
                    height: parent.height - 40 // Hauteur de l'image
                    source: "data:image/jpeg;base64," + camera.frame // Source de l'image en base64
                    fillMode: Image.PreserveAspectFit // Préserve le rapport d'aspect de l'image
                    smooth: true // Rendre l'image plus fluide
                    clip: true // Applique un clipping pour éviter que l'image ne dépasse
                }
                Rectangle {
                    anchors.centerIn: parent // Zone de filtre pour l'image
                    width: parent.width + 20
                    height: parent.height + 20
                    radius: 15
                    color: "#0a0800"
                    opacity: 0.15 // Opacité de l'effet
                    z: -1 // Position sur l'axe z
                }

                MouseArea {
                    anchors.fill: parent // Capture la zone entière pour l'interaction
                    onEntered: parent.border.color = "#00A8E8" // Change la couleur de la bordure lors du survol
                    onExited: parent.border.color = "#FFFFF" // Rétablit la couleur de la bordure lors du retrait
                }
            }
            //Zone des filtres
            Column {
                anchors.left: parent.left // Ancrage à gauche
                anchors.bottom: parent.bottom // Ancrage en bas
                anchors.leftMargin: 20 // Marge gauche
                anchors.bottomMargin: 120 // Marge inférieure
                spacing: 20 // Espacement vertical

                Text {
                    text: "Filtre :" // Titre de la section des filtres
                    color: "#AAAAAA"
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter // Centrage horizontal
                }

                Row {
                    spacing: 10
                    ComboBox {
                        id: filterBox // Liste déroulante pour choisir le filtre
                        model: ["Aucun", "Gris", "Inversion", "Gaussian", "Median", "CLAHE", "Sobel", "Sel & Poivre", "Histogram", "Canny Edge Detection", "Bilateral", "Laplacian", "Sharpening", "Cartoon Effect", "Motion Blur", "Emboss Effect", "Sepia Effect" ,"detectFaces"]
                        currentIndex: 0 // Valeur par défaut sélectionnée
                        onCurrentIndexChanged: camera.setFilterMode(currentIndex) // Applique le filtre sélectionné à la caméra
                    }
                }
            }
            // Indicateur de l'enregistrement de la vidéo
            Rectangle {
                id: recordingIndicator // L'élément "Rectangle" est utilisé pour afficher l'indicateur d'enregistrement. Il porte l'identifiant "recordingIndicator".
                width: 20 // La largeur du rectangle est de 20 pixels.
                height: 20 // La hauteur du rectangle est également de 20 pixels.
                radius: 10 // Les coins du rectangle sont arrondis avec un rayon de 10 pixels pour lui donner une forme circulaire.
                // La couleur du rectangle change en fonction de l'état de l'enregistrement.
                // Si la caméra enregistre, la couleur devient verte, sinon elle devient rouge.
                color: camera.isRecording ? "green" : "red" // Si camera.isRecording est vrai, la couleur est verte, sinon elle est rouge.
                // Positionnement du rectangle selon les coordonnées de l'image source.
                // Le rectangle se positionne en fonction de la position de son parent et de l'imageSource (par exemple, un affichage de caméra).
                x: imageSource.parent.x + imageSource.x // Position horizontale du rectangle, basée sur la position de l'imageSource.
                y: imageSource.parent.y + imageSource.y // Position verticale du rectangle, également basée sur la position de l'imageSource.
            }
        }
        // Boutons de contrôle pour démarrer/arrêter la caméra et détecter une image
        Row {
            anchors.bottom: parent.bottom // Le Row est ancré en bas du parent (en bas de l'écran).
            anchors.horizontalCenter: parent.horizontalCenter // Le Row est centré horizontalement par rapport à son parent.
            spacing: 20 // Il y a un espacement de 20 pixels entre les éléments du Row.
            // Premier bouton pour démarrer la caméra
               Button {
                   text: " 🎥 Démarrer la caméra" // Texte du bouton, avec un emoji de caméra.
                   onClicked: {
                       // Lorsque le bouton est cliqué, les actions suivantes sont exécutées :
                       camera.setResolution(widthBox.value, heightBox.value) // La résolution de la caméra est définie selon les valeurs des zones de saisie (widthBox et heightBox).
                       camera.setOutputFile("output.avi") // Le fichier de sortie pour la capture vidéo est défini sur "output.avi".
                       camera.startCapture() // La capture vidéo commence.
                       camera.setRecording(true) // L'enregistrement vidéo est activé.
                   }
               }
               // Deuxième bouton pour arrêter la caméra
                   Button {
                       text: " ⏹ Arrêter la caméra" // Texte du bouton, avec un emoji d'arrêt.
                       onClicked: {
                           camera.setRecording(false) // L'enregistrement vidéo est arrêté.
                           camera.stopCapture() // La capture vidéo est arrêtée.
                       }
                   }
                   // Troisième bouton pour détecter une image
                     Button {
                         text: " 📸 Détecter une image" // Texte du bouton, avec un emoji de caméra.
                         onClicked: camera.detectImage() // Lors du clic sur ce bouton, la fonction "detectImage" de la caméra est appelée pour capturer une image.
                     }
                 }
           }


}
