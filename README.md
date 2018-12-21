# ActionRecognition

Dans ce projet, nous proposons une application de reconnaissance d'actions. Nous souhaitons qu'en temps réel le programme
prédise l'action effectuée par l'utilisateur. 

Les données seront récupérées à l'aide d'une kinect v2 et présentées à un réseau de neurones qui nous renverra une distribution
de probabilités.
Nous limitons notre étude aux 6 actions ci-dessous :
- s'assoir
- se lever
- lire
- tituber
- tomber
- marcher

Nous utilisons comme modèle une architecture Part-Aware LSTM proposée dans [ce document](https://arxiv.org/abs/1604.02808)
et implémentée par [FesianXu](https://github.com/FesianXu/PLSTM).

Nous avons entraîné ce modèle sur une partie de [la base de données NTURBG-D](http://rose1.ntu.edu.sg/datasets/actionrecognition.asp)
ne contenant que les données correspondant aux 6 actions citées plus haut.

Dans le dossier python, on retrouve toutes les fichiers Python et un dossier model contenant les fichiers de sauvegarde
du modèle entraîné. La classe DataLoader permet de traiter les données récupérées. Les classes PartAwareLSTMCell et PLSTM correspondent
à l'architecture de notre réseau de neurones. Dans le fichier features.py, on retrouve les paramètres de notre modèle
(nombre de couches cachées, taux d'apprentissage, ...). Le fichier main.py correspond au programme principal.

Dans le dossier face-k, on retrouve tous les fichiers C++ correspondant à l'interface utilisateur. Le fichier main.cpp correspond au
programme principal. 

Dans le dossier data, on retrouve les fichiers csv  dans lesquels on écrit les données issues de la kinect et l'on écrit les
résultats.

Pour utiliser notre application, il faut lancer le programme face-k/main.cpp. La kinect va commencer à écrire les informations dans
le fichier data/skeleton.csv. Puis, le script Python main.py va être lancé dans un second thread et commencer à traiter les données
du fichier data/skeleton.csv et faire des prédictions qu'il va écrire dans le fichier preds.csv. Dans le thread principal, les
prédictions vont être lues et affichées à l'écran sous la forme d'un camembert.

Dans le fichier main.py, il est possible de modifier le nombre de frames pour une prédiction en changeant la valeur de n_frame
et le pas de la fenêtre glissante avec la valeur step.