#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

int main(int argc, char *argv[]) {
    int descripteurSocket;
    struct sockaddr_in sockaddrDistant;
    socklen_t longueurAdresse;

    char buffer[LG_MESSAGE];
    int nb;

    char ip_dest[16];
    int port_dest;

    // Vérification des arguments
    if (argc > 1) {
        strncpy(ip_dest, argv[1], 16);
        sscanf(argv[2], "%d", &port_dest);
    } else {
        printf("USAGE : %s ip port\n", argv[0]);
        exit(-1);
    }

    // Création de la socket
    descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (descripteurSocket < 0) {
        perror("Erreur en création de la socket...");
        exit(-1);
    }
    printf("Socket créée! (%d)\n", descripteurSocket);

    // Initialisation de l'adresse du serveur
    longueurAdresse = sizeof(sockaddrDistant);
    memset(&sockaddrDistant, 0x00, longueurAdresse);
    sockaddrDistant.sin_family = AF_INET;
    sockaddrDistant.sin_port = htons(port_dest);
    inet_aton(ip_dest, &sockaddrDistant.sin_addr);

    // Connexion au serveur
    if (connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant, longueurAdresse) == -1) {
        perror("Erreur de connection avec le serveur distant...");
        close(descripteurSocket);
        exit(-2);
    }
    printf("Connexion au serveur %s:%d réussie!\n", ip_dest, port_dest);

    // Attendre le message "start" du serveur
    nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
    if (nb == -1) {
        perror("Erreur en lecture...");
        close(descripteurSocket);
        exit(-3);
    }
    buffer[nb] = '\0';
    if (strcmp(buffer, "start") != 0) {
        printf("Erreur : message inattendu du serveur : %s\n", buffer);
        close(descripteurSocket);
        exit(-4);
    }
    printf("Début de la partie!\n");

    // Initialisation de la grille de jeu
    char grille[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
    afficher_grille(grille);

    while (1) {
        int x, y;
        // Lecture du coup du joueur
        printf("Entrez votre coup (ligne colonne) : ");
        scanf("%d %d", &x, &y);

        // Envoi du coup au serveur
        sprintf(buffer, "%d %d", x, y);
        nb = send(descripteurSocket, buffer, strlen(buffer) + 1, 0);
        if (nb == -1) {
            perror("Erreur en écriture...");
            close(descripteurSocket);
            exit(-5);
        }

        // Réception du message du serveur
        nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
        if (nb == -1) {
            perror("Erreur en lecture...");
            close(descripteurSocket);
            exit(-6);
        }
        buffer[nb] = '\0';

        if (strcmp(buffer, "invalid") == 0) {
            printf("Case déjà occupée, veuillez rejouer.\n");
            continue;
        }

        if (strncmp(buffer, "continue", 8) == 0) {
            sscanf(buffer + 9, "%d %d", &x, &y);
            grille[x][y] = 'O';
            afficher_grille(grille);
        } else if (strncmp(buffer, "Owins", 5) == 0) {
            sscanf(buffer + 6, "%d %d", &x, &y);
            grille[x][y] = 'O';
            afficher_grille(grille);
            printf("Le joueur O gagne!\n");
            break;
        } else if (strncmp(buffer, "Oend", 4) == 0) {
            sscanf(buffer + 5, "%d %d", &x, &y);
            grille[x][y] = 'O';
            afficher_grille(grille);
            printf("La grille est pleine, pas de gagnant!\n");
            break;
        } else if (strcmp(buffer, "Xend") == 0) {
            printf("La grille est pleine, pas de gagnant!\n");
            break;
        } else if (strcmp(buffer, "Xwins") == 0) {
            printf("Le joueur X gagne!\n");
            break;
        } else {
            printf("Erreur : message inattendu du serveur : %s\n", buffer);
            break;
        }
    }

    close(descripteurSocket);
    return 0;
}