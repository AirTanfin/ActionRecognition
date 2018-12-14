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
