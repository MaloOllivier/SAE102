#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define TAILLE 12
#define NB_DEPLACEMENTS 500

// definition des touches
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'

typedef char t_Plateau[TAILLE][TAILLE];
typedef char typeDeplacements[NB_DEPLACEMENTS];

// definition des char a enregistrer / afficher
const char SOKOBAN[1] = "@";
const char CAISSES[1] = "$";
const char CIBLES[1] = ".";
const char MURS[1] = "#";
const char ESPACE[1] = " ";
const char CAISSES_SUR_CIBLES[1] = "*";
const char SOKOBAN_SUR_CIBLE[1] = "+";
const char SOK_GAUCHE = 'g';
const char CAISSE_GAUCHE = 'G';
const char SOK_DROITE = 'd';
const char CAISSE_DROITE = 'D';
const char SOK_HAUT = 'h';
const char CAISSE_HAUT = 'H';
const char SOK_BAS = 'b';
const char CAISSE_BAS = 'B';

// prototypes de toutes les fonctions / procedures
void lecture_niveau(char niveau[]);
int kbhit();
void charger_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, t_Plateau niveau);
void affiche_entete(char niveau[], int compteur);
void lecture_touches(char *Adr_touche);
void deplacer(typeDeplacements deplacement, t_Plateau plateau, int x, int y, int nbDep, int *compteur);
void detection_sokoban(t_Plateau plateau, int *AdrX, int *AdrY);
bool gagne(t_Plateau plateau, t_Plateau niveau);
void chargerDeplacements(typeDeplacements t, char fichier[], int * nb);


int main(){
    //declaration des variables
    bool victoire = false;
    t_Plateau plateau, niveau;
    char nomNiveau[30], nomDeplacement[30];
    int compteur, nbDep;
    int sokobanX, sokobanY;
    typeDeplacements deplacements;
    lecture_niveau(nomNiveau);
    charger_partie(niveau, nomNiveau);

    while (victoire == false){
        // remise a 0
        compteur = 0;        

        // initialisation
        charger_partie(plateau, nomNiveau);
        printf("Fichier deplacements (.dep) : ");
        scanf("%s",nomDeplacement);
        chargerDeplacements(deplacements, nomDeplacement, &nbDep);
        printf("nbDep : %d\n",nbDep);
        system("clear");
        affiche_entete(nomNiveau, compteur);
        afficher_plateau(plateau, niveau);

        while (victoire == false){ 
            usleep(120000); // delay pour ne pas prendre trop de ressources
            victoire = gagne(plateau, niveau);
            detection_sokoban(plateau, &sokobanX, &sokobanY);
            deplacer(deplacements, plateau, sokobanX, sokobanY, nbDep, &compteur);
            system("clear");
            affiche_entete(nomNiveau, compteur);
            afficher_plateau(plateau, niveau);
        }
    }
    if (victoire == true){ // victoire
        printf("Bravo !!! passez au niveau suivant !\n");
    }
    return EXIT_SUCCESS;
}

void lecture_niveau(char niveau[]){
    printf("nom du fichier .sok : ");
    scanf("%s", niveau);
}

void charger_partie(t_Plateau plateau, char fichier[]){
    FILE *f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f == NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    }
    else{
        for (int ligne = 0; ligne < TAILLE; ligne++){
            for (int colonne = 0; colonne < TAILLE; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

void afficher_plateau(t_Plateau plateau, t_Plateau niveau){
    char caseAffiche;
    caseAffiche = ESPACE[0];
    for (int x = 0; x < TAILLE; x++){
        for (int y = 0; y < TAILLE; y++){
            char casePlateau[1], caseNiveau[1];
            casePlateau[0] = plateau[x][y];
            caseNiveau[0] = niveau[x][y];
            if (casePlateau[0] == MURS[0]){
                caseAffiche = MURS[0];
            }
            else if (casePlateau[0] == CAISSES[0] || casePlateau[0] == CAISSES_SUR_CIBLES[0]){
                caseAffiche = CAISSES[0];
                plateau[x][y] = CAISSES[0];
            }
            else if (casePlateau[0] == SOKOBAN[0] || casePlateau[0] == SOKOBAN_SUR_CIBLE[0]){
                caseAffiche = SOKOBAN[0];
            }
            else if (caseNiveau[0] == CIBLES[0] || caseNiveau[0] == CAISSES_SUR_CIBLES[0] || caseNiveau[0] == SOKOBAN_SUR_CIBLE[0]){
                if (casePlateau[0] != SOKOBAN[0] && casePlateau[0] != CAISSES[0]){
                    caseAffiche = CIBLES[0];
                    plateau[x][y] = CIBLES[0];
                }
            }
            else{
                caseAffiche = ESPACE[0];
            }
            printf("%c", caseAffiche);
        }
        printf("\n");
    }
}

void affiche_entete(char niveau[], int compteur){
    printf("SOKOBAN niveau : %s\n\ntouches de depalcements :\n%c (haut)", niveau, HAUT);
    printf(" %c (gauche)\n%c (bas)  %c (droite)\n\n", GAUCHE, BAS, DROITE);
    printf("Nombre de deplacements : %d \n\n\n", compteur);
}

void deplacer(typeDeplacements deplacement, t_Plateau plateau, int x, int y, int nbDep, int *compteur){
    int i = *compteur;
    if(deplacement[i] == SOK_BAS){
        plateau[x + 1][y] = SOKOBAN[0]; 
        plateau[x][y] = ESPACE[0];
        (*compteur)++;
    }
    else if(deplacement[i] == SOK_HAUT){ 
        plateau[x - 1][y] = SOKOBAN[0];
        plateau[x][y] = ESPACE[0];
        (*compteur)++;
    }
    else if(deplacement[i] == SOK_DROITE){
        plateau[x][y + 1] = SOKOBAN[0];
        plateau[x][y] = ESPACE[0];
        (*compteur)++;
    }
    else if(deplacement[i] == SOK_GAUCHE){
        plateau[x][y - 1] = SOKOBAN[0];
        plateau[x][y] = ESPACE[0];
        (*compteur)++;
    }
    else if(deplacement[i] == CAISSE_BAS && plateau[x + 1][y] == CAISSES[0]){ 
        plateau[x + 1][y] = SOKOBAN[0];
        plateau[x - 1][y] = ESPACE[0];
        plateau[x][y] = CAISSES[0];
        (*compteur)++;
    }
    else if(deplacement[i] == CAISSE_HAUT){
        plateau[x - 1][y] = SOKOBAN[0];
        plateau[x + 1][y] = ESPACE[0];
        plateau[x][y] = CAISSES[0];
        (*compteur)++;
    }
    else if(deplacement[i] == CAISSE_DROITE){
        plateau[x][y + 1] = SOKOBAN[0];
        plateau[x][y - 1] = ESPACE[0];
        plateau[x][y] = CAISSES[0];
        (*compteur)++;
    }
    else if(deplacement[i] == CAISSE_GAUCHE){
        plateau[x][y - 1] = SOKOBAN[0];
        plateau[x][y + 1] = ESPACE[0];
        plateau[x][y] = CAISSES[0];
        (*compteur)++;
    }
}


void detection_sokoban(t_Plateau plateau, int *AdrX, int *AdrY){
    int x, y;
    bool trouve = false;
    for (x = 0; x < TAILLE; x++){
        for (y = 0; y < TAILLE; y++){
            if (plateau[x][y] == SOKOBAN[0] || plateau[x][y] == SOKOBAN_SUR_CIBLE[0]){
                trouve = true;
                break;
            }
        }
        if (trouve){
            break;
        }
    }
    if (trouve){
        *AdrX = x;
        *AdrY = y;
    }
    else{
        printf("sokoban introuvable\n");
    }
}

bool gagne(t_Plateau plateau, t_Plateau niveau){
    bool victoire = true;
    for (int x = 0; x < TAILLE; x++){
        for (int y = 0; y < TAILLE; y++){
            if ((niveau[x][y] == CIBLES[0] || niveau[x][y] == SOKOBAN_SUR_CIBLE[0]) || niveau[x][y] == CAISSES_SUR_CIBLES[0]){
                if (plateau[x][y] != CAISSES[0]){
                    victoire = false;
                }
            }
        }
    }
    return victoire;
}

void chargerDeplacements(typeDeplacements t, char fichier[], int * nb){
    FILE * f;
    char dep;
    *nb = 0;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("FICHIER NON TROUVE\n");
    } else {
        fread(&dep, sizeof(char), 1, f);
        if (feof(f)){
            printf("FICHIER VIDE\n");
        } else {
            while (!feof(f)){
                t[*nb] = dep;
                (*nb)++;
                fread(&dep, sizeof(char), 1, f);
            }
        }
    }
    fclose(f);
}
