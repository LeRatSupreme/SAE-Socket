#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 5050
#define LG_MESSAGE 256

// Fonction pour afficher la grille de jeu
void afficher_grille(char grille[3][3]) {
    printf(" %c | %c | %c\n", grille[0][0], grille[0][1], grille[0][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n", grille[1][0], grille[1][1], grille[1][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n", grille[2][0], grille[2][1], grille[2][2]);
    printf("\n");
}

// Fonction pour vérifier si un joueur a gagné
int verifier_victoire(char grille[3][3], char joueur) {
    for (int i = 0; i < 3; i++) {
        if (grille[i][0] == joueur && grille[i][1] == joueur && grille[i][2] == joueur) return 1;
        if (grille[0][i] == joueur && grille[1][i] == joueur && grille[2][i] == joueur) return 1;
    }
    if (grille[0][0] == joueur && grille[1][1] == joueur && grille[2][2] == joueur) return 1;
    if (grille[0][2] == joueur && grille[1][1] == joueur && grille[2][0] == joueur) return 1;
    return 0;
}

// Fonction pour vérifier si la grille est pleine
int grille_pleine(char grille[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grille[i][j] == ' ') return 0;
        }
    }
    return 1;
}

// Fonction pour choisir une case aléatoire vide dans la grille
void choisir_case_aleatoire(char grille[3][3], int *x, int *y) {
    do {
        *x = rand() % 3;
        *y = rand() % 3;
    } while (grille[*x][*y] != ' ');
}

int main(int argc, char *argv[]) {
    int socketEcoute;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;
    int socketDialogue;
    struct sockaddr_in pointDeRencontreDistant;
    char messageRecu[LG_MESSAGE];
    int lus;

    // Création de la socket d'écoute
    socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
    if (socketEcoute < 0) {
        perror("socket");
        exit(-1);
    }
    printf("Socket créée avec succès ! (%d)\n", socketEcoute);

    // Initialisation de l'adresse locale
    longueurAdresse = sizeof(pointDeRencontreLocal);
    memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
    pointDeRencontreLocal.sin_family = PF_INET;
    pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    pointDeRencontreLocal.sin_port = htons(PORT);

    // Attachement de la socket à l'adresse locale
    if (bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse) < 0) {
        perror("bind");
        exit(-2);
    }
    printf("Socket attachée avec succès !\n");

    // Mise en écoute de la socket
    if (listen(socketEcoute, 5) < 0) {
        perror("listen");
        exit(-3);
    }
    printf("Socket placée en écoute passive ...\n");

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    while (1) {
        memset(messageRecu, 0x00, LG_MESSAGE);
        printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

        // Acceptation d'une connexion entrante
        socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketDialogue < 0) {
            perror("accept");
            close(socketDialogue);
            close(socketEcoute);
            exit(-4);
        }

        // Envoyer le message "start" au client
        strcpy(messageRecu, "start");
        send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);

        // Initialisation de la grille de jeu
        char grille[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
        afficher_grille(grille);

        while (1) {
            // Réception du coup du client
            lus = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (lus == -1) {
                perror("read");
                close(socketDialogue);
                exit(-5);
            } else if (lus == 0) {
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue);
                break;
            } else {
                int x, y;
                sscanf(messageRecu, "%d %d", &x, &y);
                grille[x][y] = 'X';
                afficher_grille(grille);

                // Vérifier si le joueur X a gagné
                if (verifier_victoire(grille, 'X')) {
                    strcpy(messageRecu, "Xwins");
                    send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Vérifier si la grille est pleine
                if (grille_pleine(grille)) {
                    strcpy(messageRecu, "Xend");
                    send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Choisir une case aléatoire pour le serveur
                choisir_case_aleatoire(grille, &x, &y);
                grille[x][y] = 'O';
                afficher_grille(grille);

                // Vérifier si le joueur O a gagné
                if (verifier_victoire(grille, 'O')) {
                    sprintf(messageRecu, "Owins %d %d", x, y);
                    send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Vérifier si la grille est pleine après le coup du serveur
                if (grille_pleine(grille)) {
                    sprintf(messageRecu, "Oend %d %d", x, y);
                    send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Envoyer le coup du serveur au client
                sprintf(messageRecu, "continue %d %d", x, y);
                send(socketDialogue, messageRecu, strlen(messageRecu) + 1, 0);
            }
        }

        close(socketDialogue);
    }
    close(socketEcoute);
    return 0;
}