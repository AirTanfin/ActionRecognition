# Acquisition Kinect

L'interface est créée avec Qt dans l'IDE QtCreator.  
Pour télécharger et installer Qt: https://www.qt.io/download

Pour que le flux soit affiché correctement, vérifier que la Kinect (v2) est connectée et que le SDK est bien installé.

Il faut que OpenCV soit intsallé pour faire fonctionner l'appli. J'ai testé avec la version 3.4.4 et la version 2.4.13. Pour installer OpenCV pour que ça fonctionne avec Qt (sous Windows): https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows

Une fois OpenCV installé, modifier le fichier **face-k.pro**, pour lier les bonnes librairies.

Dans le dossier **inc** il y a toutes les libs pour utiliser la Kinect, il ne devrait pas y avoir de problème de ce coté là.

Pour ouvrir le projet dans QtCreator:
- Lancer QtCreator
- Cliquer sur "Open Project"
- Sélectionner le fichier **face-k.pro**

Si vous avez des questions sur l'installation ou le code n'hésitez pas à me soliciter :)
