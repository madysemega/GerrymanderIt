//
// Created by Chaimaa Khal on 2020-12-09.
//

#include "SolutionInitiale.h"

/**
 * Fonction permettant d'initialiser les circonscriptions avec leur nb de municipalites de tel facon a ce que
 * chaque municipalite soit attribuee a une seule circonscription
 * @param nb_municipalites : le nombre de de municipalites au total (n)
 * @param nb_circonscriptions : le nombre de circonscriptions (m)
 * @param circonscriptions : un vecteur qui contiendra des pointeurs vers les circonscriptions initialisees
 */
void SolutionInitiale::initialiser_circonscriptions(const int& nb_municipalites, const int& nb_circonscriptions,
                                  vector<unique_ptr<Circonscription>>& circonscriptions){
    circonscriptions.clear();
    int distance_manh = ceil(((double)nb_municipalites)/nb_circonscriptions/2);
    int k = floor(((double)nb_municipalites) / nb_circonscriptions);
    int nb_k = (k+1) * nb_circonscriptions - nb_municipalites;
    for (int i = 0; i < nb_circonscriptions; i++){
        if (i < nb_k)
            circonscriptions.emplace_back(make_unique<Circonscription>(k, distance_manh));
        else
            circonscriptions.emplace_back(make_unique<Circonscription>(k+1, distance_manh));
    }
}



/**
 * Fonction realisant un parcours diagonal de la matrice_votes des votes pour tenter un partionnement
 * @param nb_circonscriptions : le nb de circonscriptions (m)
 * @param circonscriptions : un vecteur contenant le resultat du partionnement
 * @param circonscriptions_dispo : vecteur contenant un pointeur vers les circonscriptions initialisees
 * @param matrice_votes : matrice contenant les votes de chaque municipalite pour le parti vert
 */
void SolutionInitiale::parcours_diagonales(const int& nb_circonscriptions,
                         vector<unique_ptr<Circonscription>>& circonscriptions,
                         vector<unique_ptr<Circonscription>>& circonscriptions_dispo,
                         const vector<vector<int>>& matrice_votes){
    int nb_lignes = matrice_votes.size();
    int nb_colonnes = matrice_votes[0].size();
    bool insertion_reussie;
    int i;

    // Parcours diagonal de la matrice_votes
    for(int k = 0; k <= nb_lignes+nb_colonnes-2; k++){
        for(int j = 0; j <= k; j++){
            i = k - j;
            if(i >= nb_lignes || j >= nb_colonnes)
                continue;
            // Considerer les circonscriptions disponibles une a une
            auto it = circonscriptions_dispo.begin();
            do {
                insertion_reussie = (*it)->ajouter_municipalite(i, j, matrice_votes[i][j]);
                it++;
            }while (!insertion_reussie && it != circonscriptions_dispo.end());

            /* Si toutes les circonscriptions ont ete considere et que l'insertion de la municipalite n'a pas ete
               reussie, lancer une exception */
            if (!insertion_reussie)
                throw runtime_error("Aucune solution trouvee...");

            // Sinon, verifier que la circonscription d'insertion n'a pas atteint sa pleine capacite
            it--;
            if ((*it)->nb_municipalites == (*it)->municipalites.size()) {
                circonscriptions.push_back(move(*it));
                circonscriptions_dispo.erase(it);
            }
        }
    }
}

/**
 * Fonction faisant appel au parcours en diagonal pour generer la solution initiale jusqu'a ce que la generation
 * soit reussie
 * @param nb_circonscriptions : le nb de circonscriptions (m)
 * @param circonscriptions : un vecteur contenant le resultat du partionnement
 * @param matrice_votes : matrice contenant les votes de chaque municipalite pour le parti vert
 * @param solutionInitialeTrouvee : booleen indiquant si la solution a été trouvée
 * @return: entier indiquant le numero du thread ayant trouvé la solution
 */
int SolutionInitiale::generer_par_diagonales(const int& nb_circonscriptions,
                            vector<unique_ptr<Circonscription>>& circonscriptions,
                            const vector<vector<int>>& matrice_votes, bool& solutionInitialeTrouvee){
    int nb_lignes = matrice_votes.size();
    int nb_colonnes = matrice_votes[0].size();
    vector<unique_ptr<Circonscription>> circonscriptions_dispo;
    random_device rd;
    mt19937 mt(rd());

    while(true){
        try {
            // (Re)initialisation des circonscriptions
            initialiser_circonscriptions(nb_lignes*nb_colonnes, nb_circonscriptions, circonscriptions_dispo);

            // Considerer les circonscriptions dans un ordre aleatoire (utile lorsque plafond(n/m) != plancher(n/m)
            shuffle(begin(circonscriptions_dispo), end(circonscriptions_dispo), mt);
            parcours_diagonales(nb_circonscriptions, circonscriptions, circonscriptions_dispo, matrice_votes);
        }
        catch (...) {
            circonscriptions.clear();
            continue;
        }
        solutionInitialeTrouvee = true;
        return 2;
    }
}

/**
 * Methode servant à transferer la solution generé par generer_par_bfs dans la solution finale
 * @param resultatBFS : le résultat retourné par generer_par_bfs
 * @param circonscriptions : le vecteur devant contenir la solution finale
 * @param matrice_votes : matrice contenant les votes de chaque municipalite pour le parti vert
 * @param contraintes : Les contraintes devant etre respectées pour le problème
 */
void SolutionInitiale::transfert_solution(vector<shared_ptr<Partition>>& resultatBFS, vector<unique_ptr<Circonscription>>& circonscriptions,
                       const vector<vector<int>>& matrice_votes, Contraintes& contraintes)
{
   int i = 0;
   for (auto& partition : resultatBFS)
   {
       circonscriptions.push_back(make_unique<Circonscription>(partition->getNombreDeMunicipalites(), contraintes.distanceMaxEntreDeuxPoints));
       circonscriptions[i++]->ajouter_partition(partition, matrice_votes);
   }
}

/**
 * Methode servant à generer une solution en pacourant le graphe en bfs pour former des circonscriptions
 * @param nb_circonscriptions : le nombre de circonscriptions
 * @param circonscriptions : le vecteur devant contenir la solution finale
 * @param matrice_votes : matrice contenant les votes de chaque municipalite pour le parti vert
 * @param solutionInitialeTrouvee : booleen indiquant si la solution a été trouvée
 * @return: entier indiquant le numero du thread ayant trouvé la solution
 */
int SolutionInitiale::generer_par_bfs(const int& nb_circonscriptions,
                 vector<unique_ptr<Circonscription>>& circonscriptions,
                 const vector<vector<int>>& matrice_votes, bool& solutionInitialeTrouvee){
    while(!solutionInitialeTrouvee)
    {
        unsigned long nombreDeMunicipalite = matrice_votes[0].size() * matrice_votes.size();
        Contraintes contraintes = Contraintes(nombreDeMunicipalite, nb_circonscriptions);
        partitionnementBFS partitionnement = partitionnementBFS();
        vector<vector<shared_ptr<Partition::Municipalite>>> liste = partitionnement.determinerListeDesPoints(matrice_votes);
        vector<shared_ptr<Partition>> resultatBFS = partitionnement.genererCirconscription(liste, contraintes, solutionInitialeTrouvee);
        transfert_solution(resultatBFS, circonscriptions, matrice_votes, contraintes);
        if(!circonscriptions.empty())
            {
                solutionInitialeTrouvee = true;
            }
    }
    return 1;
}