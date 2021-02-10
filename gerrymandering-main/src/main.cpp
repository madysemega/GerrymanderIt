#include <iostream>
#include <vector>
#include <cstdlib>
#include <iterator>
#include <string>
#include <cstring>
#include "AmeliorationLocale.h"
using namespace std;

/**
 * Fonction utilitaire permettant de lire le fichier contenant les donnees de votes
 * @param nom_fichier : le nom du fichier
 * @param resultats_vote : une matrice qui contiendra le resultat de la lecture
 */
void lire_fichier_votes(string nom_fichier, vector<vector<int>>& resultats_vote){
    resultats_vote.clear();
    ifstream infile(nom_fichier);
    int nb_colonnes, nb_lignes;
    vector<int> ligne;
    int resultat;
    if (infile.is_open()) {
        infile >> nb_colonnes >> nb_lignes;
        for (int i = 0; i < nb_lignes; i++){
            ligne.clear();
            for(int j = 0; j < nb_colonnes; j++){
                infile >> resultat;
                ligne.push_back(resultat);
            }
            resultats_vote.push_back(ligne);
        }
        infile.close();
    }
    else {
        cout << "Impossible d'ouvrir le fichier " << nom_fichier << endl;
        exit(1);
    }
}

/**
 * Fonction servant a verifier que le chemin entre vers le fichier contenant
 * les donnes necessaires au calcul de la tour maximale est un
 * chemin absolu
 * @param path le chemin
 * @return true si le chemin est absolu
 */
bool est_chemin_absolu(string path){
    if (path[0] == '/' || (path[0] == 'C' && path[1] == ':')) return true;
    return false;
}


/**
 * Fonction principale
 */
int main(int argc, char* argv[]) {
    if (argc < 5){
        cout << "Argument ou parametre manquant lors de l'appel.\n";
        exit(1);
    }
    // Verification de l'argument -e
    if (strncmp(argv[1], "-e", 2) != 0){
        cout << "Argument "<< argv[1] << " inconnu.\n";
        exit(1);
    }
    string nom_fichier = argv[2];
    if (!est_chemin_absolu(nom_fichier)){
        cout << nom_fichier << " n'est pas un chemin absolu.\n";
        exit(1);
    }
    // Verification de l'argument -c
    if (strncmp(argv[3], "-c", 2) != 0){
        cout << argv[3] << endl;
        cout << "Argument "<< argv[3] << " inconnu.\n";
        exit(1);
    }
    vector<vector<int>> resultats_vote;
    string nombre_circonscriptions = argv[4];
    lire_fichier_votes(nom_fichier, resultats_vote);
    bool afficher_matrice = false;
    int nb_circonscriptions = stoi(nombre_circonscriptions);
    if (argc == 6){
        if(strncmp(argv[5], "-p", 2) != 0){
            cout << "Argument " << argv[5] << "inconnu.\n";
            exit(0);
        }
        afficher_matrice = true;
    }
    AmeliorationLocale ameliorationLocale;
    ameliorationLocale.truquer_election(nb_circonscriptions, resultats_vote, afficher_matrice);
}
