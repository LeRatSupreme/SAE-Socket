#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 6050
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

int main(int argc, char *argv[]) {
    int socketEcoute;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;
    int socketClientX, socketClientO;
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
    if (listen(socketEcoute, 2) < 0) {
        perror("listen");
        exit(-3);
    }
    printf("Socket placée en écoute passive ...\n");

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    // Acceptation des connexions des deux clients
    printf("Attente de la connexion du joueur X...\n");
    socketClientX = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if (socketClientX < 0) {
        perror("accept");
        close(socketEcoute);
        exit(-4);
    }
    printf("Joueur X connecté !\n");

    printf("Attente de la connexion du joueur O...\n");
    socketClientO = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if (socketClientO < 0) {
        perror("accept");
        close(socketClientX);
        close(socketEcoute);
        exit(-5);
    }
    printf("Joueur O connecté !\n");

    // Envoyer le message "start" aux deux clients
    strcpy(messageRecu, "start");
    send(socketClientX, messageRecu, strlen(messageRecu) + 1, 0);
    send(socketClientO, messageRecu, strlen(messageRecu) + 1, 0);

    // Initialisation de la grille de jeu
    char grille[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
    afficher_grille(grille);

    int joueurActuel = 'X';
    int socketActuel = socketClientX;

    while (1) {
        // Réception du coup du client actuel
        lus = recv(socketActuel, messageRecu, LG_MESSAGE, 0);
        if (lus == -1) {
            perror("read");
            close(socketClientX);
            close(socketClientO);
            exit(-6);
        } else if (lus == 0) {
            fprintf(stderr, "La socket a été fermée par le client !\n\n");
            close(socketClientX);
            close(socketClientO);
            break;
        } else {
            int x, y;
            sscanf(messageRecu, "%d %d", &x, &y);

            // Vérifier si la case est déjà occupée
            if (grille[x][y] != ' ') {
                strcpy(messageRecu, "invalid");
                send(socketActuel, messageRecu, strlen(messageRecu) + 1, 0);
                continue;
            }

            grille[x][y] = joueurActuel;
            afficher_grille(grille);

            // Vérifier si le joueur actuel a gagné
            if (verifier_victoire(grille, joueurActuel)) {
                sprintf(messageRecu, "%cwins %d %d", joueurActuel, x, y);
                send(socketClientX, messageRecu, strlen(messageRecu) + 1, 0);
                send(socketClientO, messageRecu, strlen(messageRecu) + 1, 0);
                break;
            }

            // Vérifier si la grille est pleine
            if (grille_pleine(grille)) {
                sprintf(messageRecu, "%cend %d %d", joueurActuel, x, y);
                send(socketClientX, messageRecu, strlen(messageRecu) + 1, 0);
                send(socketClientO, messageRecu, strlen(messageRecu) + 1, 0);
                break;
            }

            // Passer au joueur suivant
            joueurActuel = (joueurActuel == 'X') ? 'O' : 'X';
            socketActuel = (socketActuel == socketClientX) ? socketClientO : socketClientX;

            // Envoyer le coup au joueur suivant
            sprintf(messageRecu, "continue %d %d", x, y);
            send(socketActuel, messageRecu, strlen(messageRecu) + 1, 0);
        }
    }

    close(socketClientX);
    close(socketClientO);
    close(socketEcoute);
    return 0;
}