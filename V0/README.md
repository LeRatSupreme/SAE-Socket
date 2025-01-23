## Description du projet
T3N (Tic-Tac-Toe Net) est une application en C permettant à deux joueurs de s'affronter en ligne au jeu du Morpion via une communication client-serveur utilisant des sockets en TCP/IP. Ce projet est réalisé dans le cadre de la SAÉ 3.01 du BUT Informatique et vise à approfondir la maîtrise des communications réseau en C.

## Fonctionnalités de la Version 0
La version 0 de T3N introduit les bases de la communication client-serveur pour le jeu du Morpion. Voici les principales fonctionnalités :
- Le client représente le joueur **X** et le serveur le joueur **O**.
- Le client se connecte au serveur et attend le message de début de partie.
- Le client joue son coup, envoie la case choisie au serveur et attend une réponse.
- Le serveur choisit une case libre de manière aléatoire et renvoie la mise à jour au client.
- Affichage dynamique de la grille de jeu après chaque coup.

## Installation et Compilation
### Prérequis
- Un système Linux avec GCC installé.
- Une connexion réseau fonctionnelle.

### Compilation
Utilisez la commande suivante pour compiler le client et le serveur :
```bash
gcc -o T3N_client_V0 T3N_client_V0.c
gcc -o T3N_serveur_V0 T3N_serveur_V0.c
```

### Exécution
Lancez d'abord le serveur :
```bash
./T3N_serveur_V0
```

Puis le client sur un autre terminal :
```bash
./T3N_client_V0 <IP du serveur> <5050>
```

## Architecture et Organigramme
Le fonctionnement du programme repose sur un échange de messages entre le client et le serveur via des sockets TCP/IP. L'organigramme de communication détaillant ces interactions est fourni dans le fichier `T3N_V0_organigramme.pdf`.

## Auteurs
- FARDEL Mathéïs, CAPON Ethan, TELLIEZ Luc, ZEMRANI Sofiane
- IUT Calais - BUT Informatique