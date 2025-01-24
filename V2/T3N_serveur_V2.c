#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 5060
#define LG_MESSAGE 256

/**
 * Fonction pour afficher la grille de jeu
 * @param grille La grille de jeu
 */
void afficher_grille(char grille[3][3]) {
    printf(" %c | %c | %c\n", grille[0][0], grille[0][1], grille[0][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n", grille[1][0], grille[1][1], grille[1][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n", grille[2][0], grille[2][1], grille[2][2]);
    printf("\n");
}

/**
 * Fonction pour vérifier si un joueur a gagné
 * @param grille La grille de jeu
 * @param joueur Le joueur à vérifier
 */
int verifier_victoire(char grille[3][3], char joueur) {
    for (int i = 0; i < 3; i++) {
        if (grille[i][0] == joueur && grille[i][1] == joueur && grille[i][2] == joueur) return 1;
        if (grille[0][i] == joueur && grille[1][i] == joueur && grille[2][i] == joueur) return 1;
    }
    if (grille[0][0] == joueur && grille[1][1] == joueur && grille[2][2] == joueur) return 1;
    if (grille[0][2] == joueur && grille[1][1] == joueur && grille[2][0] == joueur) return 1;
    return 0;
}

/**
 * Fonction pour vérifier si la grille est pleine
 * @param grille La grille de jeu
 */
int grille_pleine(char grille[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grille[i][j] == ' ') return 0;
        }
    }
    return 1;
}

/**
 * Fonction main
 */
int main(int argc, char *argv[]) {
    int socketEcoute;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;
    int socketDialogue1, socketDialogue2;
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

        // Acceptation de la première connexion entrante (joueur X)
        socketDialogue1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketDialogue1 < 0) {
            perror("accept");
            close(socketDialogue1);
            close(socketEcoute);
            exit(-4);
        }
        printf("Joueur X connecté !\n");

        // Envoyer le message "start" au joueur X
        strcpy(messageRecu, "start");
        send(socketDialogue1, messageRecu, strlen(messageRecu) + 1, 0);

        // Acceptation de la deuxième connexion entrante (joueur O)
        socketDialogue2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketDialogue2 < 0) {
            perror("accept");
            close(socketDialogue2);
            close(socketEcoute);
            exit(-5);
        }
        printf("Joueur O connecté !\n");

        // Envoyer le message "start" au joueur O
        send(socketDialogue2, messageRecu, strlen(messageRecu) + 1, 0);

        // Initialisation de la grille de jeu
        char grille[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
        afficher_grille(grille);

        int joueurActuel = 1; // 1 pour X, 2 pour O

        while (1) {
            int x, y;
            int socketActuel = (joueurActuel == 1) ? socketDialogue1 : socketDialogue2;
            char joueurSymbole = (joueurActuel == 1) ? 'X' : 'O';

            // Informer le joueur actuel que c'est son tour
            strcpy(messageRecu, "yourturn");
            send(socketActuel, messageRecu, strlen(messageRecu) + 1, 0);

            // Réception du coup du joueur actuel
            lus = recv(socketActuel, messageRecu, LG_MESSAGE, 0);
            if (lus == -1) {
                perror("read");
                close(socketDialogue1);
                close(socketDialogue2);
                exit(-6);
            } else if (lus == 0) {
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue1);
                close(socketDialogue2);
                break;
            } else {
                sscanf(messageRecu, "%d %d", &x, &y);
                grille[x][y] = joueurSymbole;
                afficher_grille(grille);

                // Vérifier si le joueur actuel a gagné
                if (verifier_victoire(grille, joueurSymbole)) {
                    sprintf(messageRecu, "%cwins %d %d", joueurSymbole, x, y);
                    send(socketDialogue1, messageRecu, strlen(messageRecu) + 1, 0);
                    send(socketDialogue2, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Vérifier si la grille est pleine
                if (grille_pleine(grille)) {
                    sprintf(messageRecu, "%cend %d %d", joueurSymbole, x, y);
                    send(socketDialogue1, messageRecu, strlen(messageRecu) + 1, 0);
                    send(socketDialogue2, messageRecu, strlen(messageRecu) + 1, 0);
                    break;
                }

                // Envoyer le coup au joueur adverse
                sprintf(messageRecu, "continue %d %d", x, y);
                int socketAdverse = (joueurActuel == 1) ? socketDialogue2 : socketDialogue1;
                send(socketAdverse, messageRecu, strlen(messageRecu) + 1, 0);

                // Changer de joueur
                joueurActuel = (joueurActuel == 1) ? 2 : 1;
            }
        }

        close(socketDialogue1);
        close(socketDialogue2);
    }
    close(socketEcoute);
    return 0;
}