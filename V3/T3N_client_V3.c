#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
 * Fonction main
 */
int main(int argc, char *argv[]) {
    int descripteurSocket;
    struct sockaddr_in sockaddrDistant;
    socklen_t longueurAdresse;
    char buffer[LG_MESSAGE];
    int nb;
    char ip_dest[16];
    int port_dest;
    int mode_spectateur = 0;
    char joueurSymbole, adversaireSymbole;
    // Vérification des arguments
    if (argc > 1) {
        strncpy(ip_dest, argv[1], 16);
        sscanf(argv[2], "%d", &port_dest);
        if (argc > 3 && strcmp(argv[3], "spectate") == 0) {
            mode_spectateur = 1;
        }
    } else {
        printf("USAGE : %s ip port [spectate]\n", argv[0]);
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
    if (mode_spectateur) {
        printf("Mode spectateur activé.\n");
    } else {
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
        // Réception du symbole du joueur (X ou O)
        nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
        if (nb == -1) {
            perror("Erreur en lecture...");
            close(descripteurSocket);
            exit(-5);
        }
        buffer[nb] = '\0';
        joueurSymbole = buffer[0];
        adversaireSymbole = (joueurSymbole == 'X') ? 'O' : 'X';
        printf("Vous jouez avec le symbole : %c\n", joueurSymbole);
    }
     // Initialisation de la grille de jeu
    char grille[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
    afficher_grille(grille);

    while (1) {
        int x, y;
        nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
        if (nb == -1) {
            perror("Erreur en lecture...");
            close(descripteurSocket);
            exit(-6);
        }
        buffer[nb] = '\0';
        // Traitement du message du serveur
        if (strncmp(buffer, "yourturn", 8) == 0) {
            if (mode_spectateur) {
                continue;
            }
            printf("C'est votre tour!\n");
            while (1) {
                printf("Entrez votre coup (ligne colonne) : ");
                scanf("%d %d", &x, &y);
                if (x >= 0 && x <= 2 && y >= 0 && y <= 2 && grille[x][y] == ' ') {
                    break;
                } else {
                    printf("Valeur invalide. Veuillez entrer des valeurs entre 0 et 2 pour une case vide.\n");
                }
            }
            grille[x][y] = joueurSymbole;
            afficher_grille(grille);
            
            // Envoi du coup au serveur
            sprintf(buffer, "%d %d", x, y);
            nb = send(descripteurSocket, buffer, strlen(buffer) + 1, 0);
            if (nb == -1) {
                perror("Erreur en écriture...");
                close(descripteurSocket);
                exit(-7);
            }
        } else if (strncmp(buffer, "continue", 8) == 0) {
            sscanf(buffer + 9, "%d %d", &x, &y);
            grille[x][y] = adversaireSymbole;
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
        } else if (strncmp(buffer, "Xend", 4) == 0) {
            sscanf(buffer + 5, "%d %d", &x, &y);
            grille[x][y] = 'X';
            afficher_grille(grille);
            printf("La grille est pleine, pas de gagnant!\n");
            break;
        } else if (strncmp(buffer, "Xwins", 5) == 0) {
            sscanf(buffer + 6, "%d %d", &x, &y);
            grille[x][y] = 'X';
            afficher_grille(grille);
            printf("Le joueur X gagne!\n");
            break;
        } else if (strncmp(buffer, "Le joueur", 9) == 0) {
            printf("%s\n", buffer);
            break;
        } else if (strncmp(buffer, "grille", 6) == 0) {
            // Afficher la grille même en mode spectateur
            sscanf(buffer + 7, "%c %c %c", &grille[0][0], &grille[0][1], &grille[0][2]);
            afficher_grille(grille);
        } else {
            printf("Erreur : message inattendu du serveur : %s\n", buffer);
            break;
        }
    }
    close(descripteurSocket);
    return 0;
}