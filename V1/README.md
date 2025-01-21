# T3N - Tic-Tac-Toe Net (Version 1)

## Description du projet
T3N (Tic-Tac-Toe Net) est une application en C permettant à deux joueurs de s'affronter en ligne au jeu du Morpion via une communication client-serveur utilisant des sockets en TCP/IP. Ce projet est réalisé dans le cadre de la SAÉ 3.01 du BUT Informatique et vise à approfondir la maîtrise des communications réseau en C.

## Fonctionnalités de la Version 1
La version 1 de T3N introduit le respect des règles du jeu, en améliorant la gestion des communications entre le client et le serveur. Voici les principales fonctionnalités :
- Le client représente le joueur **X** et le serveur le joueur **O**.
- Le client se connecte au serveur et attend le message de début de partie.
- Le client joue son coup, envoie la case choisie au serveur et attend une réponse.
- Le serveur choisit une case libre et renvoie la mise à jour au client.
- Détection des conditions de fin de partie :
  - **Victoire de X** → le client reçoit "Xwins" et termine.
  - **Victoire de O** → le client reçoit "Owins" et termine.
  - **Match nul** → le client reçoit "Draw" et termine.
- Affichage dynamique de la grille de jeu après chaque coup.

## Installation et Compilation
### Prérequis
- Un système Linux avec GCC installé.
- Une connexion réseau fonctionnelle.

### Compilation
Utilisez la commande suivante pour compiler le client et le serveur :
```bash
gcc -o T3N_client_V1 T3N_client_V1.c
gcc -o T3N_serveur_V1 T3N_serveur_V1.c
```

### Exécution
Lancez d'abord le serveur :
```bash
./T3N_serveur_V1
```
Puis le client sur une autre machine ou un autre terminal :
```bash
./T3N_client_V1 <IP du serveur> <port>
```

## Architecture et Organigramme
Le fonctionnement du programme repose sur un échange de messages entre le client et le serveur via des sockets TCP/IP. L'organigramme de communication détaillant ces interactions est fourni dans le fichier `T3N_V1_organigramme.pdf`.

## Structure du Code
### `T3N_client_V1.c`
- **Connexion au serveur** via une socket TCP.
- **Affichage de la grille de jeu** et récupération des coups de l'utilisateur.
- **Envoi et réception de messages** pour gérer les tours de jeu.
- **Vérification des conditions de victoire ou d'égalité**.

### `T3N_serveur_V1.c`
- **Attente de connexion** d'un client.
- **Envoi du message de début de partie**.
- **Gestion des tours de jeu**, mise à jour de la grille et sélection aléatoire des coups du serveur.
- **Vérification des conditions de victoire** et envoi des messages appropriés au client.

## Auteurs
- [Nom de l'équipe]
- IUT de Lille Côte d’Opale - BUT Informatique

## Licence
Ce projet est réalisé dans le cadre pédagogique du BUT Informatique et n'est pas destiné à un usage commercial.

## Remarques
- Le serveur tourne indéfiniment et peut gérer plusieurs parties successives.
- La gestion des erreurs est minimale et devra être améliorée dans les versions ultérieures.
- Une future version ajoutera la possibilité d'avoir deux clients joueurs sans intervention du serveur.