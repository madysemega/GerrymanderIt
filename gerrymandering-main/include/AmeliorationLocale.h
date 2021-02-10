//
// Created by Chaimaa Khal on 2020-12-09.
//

#ifndef TP3_INF8775_V2_AMELIORATIONLOCALE_H
#define TP3_INF8775_V2_AMELIORATIONLOCALE_H
#include <cmath>
#include <vector>
#include <memory>
#include <fstream>
#include <random>
#include <mutex>
#include <functional>
#include <thread>
#include <future>
#include <cfloat>
#include "SolutionInitiale.h"
#define SEUIL_DILUTION 0.2
using namespace std;

class AmeliorationLocale {
private:
template <typename T>
    class AttendrePremierResultat {
    public:
    T res;
    bool resultat_pret;
    std::condition_variable res_cv;
    std::mutex res_mutex;
    AttendrePremierResultat() : resultat_pret{false} {}
    };
    vector<unique_ptr<Circonscription>> circonscriptions;
    vector<vector<int>> matrice_votes;
    vector<vector<int>> matrice_circonscriptions;
    int nb_lignes;
    int nb_colonnes;
    int distance_manhattan;
    std::future<int> attendre_premier_resultat(std::future<int> f1, std::future<int> f2);
    bool permutation_possible(Municipalite municipalite1, Municipalite municipalite2);
    Municipalite meilleur_voisin_diluer(Municipalite municipalite, const vector<Municipalite>& voisins);
    Municipalite meilleur_voisin_concentrer(Municipalite municipalite, const vector<Municipalite>& voisins);
    void trouver_voisins(int i, int j, vector<Municipalite>& voisins) const;
    Municipalite trouver_meilleur_voisin(int i, int j);
    int evaluer_solution(int solution_passee, Municipalite municipalite1, Municipalite municipalite2);
    void afficher();
    int nb_circ_vertes();
    void generer_matrice_circonscriptions();
    void generer_solution_initiale(const int &nb_circonscriptions);
public:
    [[noreturn]] void truquer_election(const int& nb_circonscriptions,
                                       const vector<vector<int>>& _matrice_votes, bool afficher_matrice);
};

#endif //TP3_INF8775_V2_AMELIORATIONLOCALE_H
