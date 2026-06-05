# SAE Socket - T3N

T3N, pour Tic-Tac-Toe Net, est un projet de morpion en réseau développé en C dans le cadre d'une SAE autour des sockets TCP/IP.

Le projet montre l'évolution progressive d'une application client-serveur : d'abord une communication simple entre un client et un serveur, puis une vraie partie de morpion avec règles, deux joueurs distants et une tentative d'ajout d'un mode spectateur.

## Objectif du projet

L'objectif principal est de manipuler les sockets réseau en C :

- création de sockets TCP ;
- liaison d'un serveur à un port ;
- attente de connexions avec `listen` et `accept` ;
- connexion d'un client avec `connect` ;
- échange de messages avec `send` et `recv` ;
- gestion d'un protocole simple entre client et serveur ;
- synchronisation des tours de jeu ;
- gestion des états de fin de partie.

## Fonctionnalités

### Jeu

- Morpion sur une grille 3x3.
- Affichage de la grille dans le terminal.
- Saisie des coups au format `ligne colonne`.
- Symboles `X` et `O`.
- Détection des victoires sur lignes, colonnes et diagonales.
- Détection du match nul lorsque la grille est pleine.
- Validation des coups côté client dans les versions avancées.

### Réseau

- Communication TCP/IP.
- Serveur en écoute sur le port `5050`.
- Client lancé avec l'adresse IP et le port du serveur.
- Protocole textuel simple :
  - `start` pour démarrer ;
  - `yourturn` pour indiquer au client que c'est son tour ;
  - `continue x y` pour transmettre le coup adverse ;
  - `Xwins` ou `Owins` pour annoncer une victoire ;
  - `Xend` ou `Oend` pour annoncer un match nul.

## Versions

### V0

Première version de communication client-serveur.

- Le client joue `X`.
- Le serveur joue `O`.
- Le serveur répond avec un coup choisi aléatoirement.
- La grille est affichée après chaque coup.
- Les règles de fin de partie ne sont pas encore gérées.

### V1

Ajout des règles du morpion.

- Détection de victoire pour `X` et `O`.
- Détection de grille pleine.
- Messages de fin de partie envoyés au client.
- Le serveur joue encore automatiquement contre le client.

### V2

Version principale jouable à deux joueurs.

- Deux clients se connectent au serveur.
- Le premier client devient `X`.
- Le second client devient `O`.
- Le serveur synchronise les tours.
- Chaque joueur reçoit les coups de l'adversaire.
- La partie se termine sur victoire, match nul ou déconnexion.

### V3

Tentative d'ajout d'un mode spectateur.

- Le client accepte un argument optionnel `spectate`.
- Le serveur prévoit une connexion spectateur.
- Cette version est conservée comme trace de recherche, mais elle est indiquée comme non fonctionnelle dans son README.

La version conseillée pour tester le projet est donc `V2`.

## Prérequis

- Un système Linux ou macOS avec un compilateur C.
- `gcc`
- Un terminal pour le serveur.
- Un ou deux terminaux supplémentaires pour les clients.

Sur Linux :

```bash
sudo apt install build-essential
```

Sur macOS, installer les Command Line Tools :

```bash
xcode-select --install
```

## Compilation

Chaque version se compile séparément.

Exemple avec la V2 :

```bash
cd V2
gcc -o T3N_serveur_V2 T3N_serveur_V2.c
gcc -o T3N_client_V2 T3N_client_V2.c
```

Même principe pour les autres versions :

```bash
cd V0
gcc -o T3N_serveur_V0 T3N_serveur_V0.c
gcc -o T3N_client_V0 T3N_client_V0.c
```

```bash
cd V1
gcc -o T3N_serveur_V1 T3N_serveur_V1.c
gcc -o T3N_client_V1 T3N_client_V1.c
```

```bash
cd V3
gcc -o T3N_serveur_V3 T3N_serveur_V3.c
gcc -o T3N_client_V3 T3N_client_V3.c
```

## Lancer une partie en V2

Terminal 1, lancer le serveur :

```bash
cd V2
./T3N_serveur_V2
```

Terminal 2, connecter le premier joueur :

```bash
cd V2
./T3N_client_V2 127.0.0.1 5050
```

Terminal 3, connecter le second joueur :

```bash
cd V2
./T3N_client_V2 127.0.0.1 5050
```

Pour jouer sur deux machines différentes, remplacer `127.0.0.1` par l'adresse IP de la machine qui exécute le serveur.

## Format des coups

Les coups se saisissent sous la forme :

```text
ligne colonne
```

Les indices vont de `0` à `2`.

Exemples :

```text
0 0
1 2
2 1
```

La case en haut à gauche correspond à `0 0`, et la case en bas à droite à `2 2`.

## Structure du projet

```text
.
├── V0/
│   ├── T3N_client_V0.c
│   ├── T3N_serveur_V0.c
│   └── README.md
├── V1/
│   ├── T3N_client_V1.c
│   ├── T3N_serveur_V1.c
│   └── README.md
├── V2/
│   ├── T3N_client_V2.c
│   ├── T3N_serveur_V2.c
│   └── README.md
└── V3/
    ├── T3N_client_V3.c
    ├── T3N_serveur_V3.c
    └── README.md
```

## Architecture générale

Le serveur ouvre une socket TCP sur le port `5050`, attend les connexions, puis pilote la partie.

Dans la V2 :

1. Le serveur attend le joueur `X`.
2. Le serveur attend le joueur `O`.
3. Les deux clients reçoivent `start`.
4. Chaque client reçoit son symbole.
5. Le serveur envoie `yourturn` au joueur actif.
6. Le joueur actif envoie son coup au serveur.
7. Le serveur met à jour la grille et transmet le coup à l'adversaire.
8. Le serveur vérifie victoire ou match nul.
9. La partie s'arrête si une condition de fin est atteinte.

## Limites connues

- Les programmes sont pensés pour une exécution en terminal.
- Il n'y a pas de chiffrement ni d'authentification.
- Le protocole est textuel et volontairement simple.
- Le serveur V2 gère une partie à la fois.
- La V3 avec spectateur n'est pas fonctionnelle.
- Les exécutables générés par `gcc` ne sont pas fournis : ils doivent être recompilés localement.

## Auteurs

- Mathéïs FARDEL
- Ethan CAPON
- Luc TELLIEZ
- Sofiane ZEMRANI

Projet réalisé à l'IUT de Calais dans le cadre du BUT Informatique.

## Licence

Tous droits réservés.

Ce projet est privé. Aucune utilisation, copie, modification, distribution, publication ou réutilisation du code n'est autorisée sans accord écrit préalable des auteurs du projet.
