# T3N - Tic-Tac-Toe Net (Version 1)

## Description du projet
T3N (Tic-Tac-Toe Net) est une application en C permettant à deux joueurs de s'affronter en ligne au jeu du Morpion via une communication client-serveur utilisant des sockets en TCP/IP. Ce projet est réalisé dans le cadre de la SAÉ 3.01 du BUT Informatique et vise à approfondir la maîtrise des communications réseau en C.

## Fonctionnalités de la Version 1
La version 2 de T3N introduit de nouvelles fonctionnalités et améliorations par rapport à la version 1 :
- Deux joueurs peuvent se connecter au serveur et jouer en tant que **X** et **O**.
- Les joueurs se connectent au serveur et attendent le message de début de partie.
- Les joueurs jouent chacun leur tour, envoient la case choisie au serveur et attendent une réponse.
- Le serveur choisit une case libre et renvoie la mise à jour au client.
- Détection des conditions de fin de partie :
  - **Victoire de X** → le client reçoit "Xwins" et termine.
  - **Victoire de O** → le client reçoit "Owins" et termine.
  - **Match nul** → le client reçoit "Oend" ou "Xend" et termine.
- Affichage dynamique de la grille de jeu après chaque coup.

## Installation et Compilation
### Prérequis
- Un système Linux avec GCC installé.
- Une connexion réseau fonctionnelle.

### Compilation
Utilisez la commande suivante pour compiler le client et le serveur :
```bash
gcc -o T3N_client_V2 T3N_client_V2.c
gcc -o T3N_serveur_V2 T3N_serveur_V2.c
```

### Exécution
Lancez d'abord le serveur :
```bash
./T3N_serveur_V1
```
Puis le client sur un autre terminal :
```bash
./T3N_client_V1 <IP du serveur> <5050>
```

## Architecture et Organigramme
Le fonctionnement du programme repose sur un échange de messages entre le client et le serveur via des sockets TCP/IP. L'organigramme de communication détaillant ces interactions est fourni dans le fichier `T3N_V2_organigramme.pdf`.


## Auteurs
- FARDEL Mathéïs, CAPON Ethan, TELLIEZ Luc, ZEMRANI Sofiane
- IUT Calais - BUT Informatique

