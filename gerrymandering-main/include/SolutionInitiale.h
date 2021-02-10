//
// Created by Chaimaa Khal on 2020-12-09.
//
#ifndef TP3_INF8775_V2_SOLUTIONINITIALE_H
#define TP3_INF8775_V2_SOLUTIONINITIALE_H
#include <vector>
#include <memory>
#include <exception>
#include <iterator>
#include <random>
#include "Circonscription.h"
#include "partitionnementBFS.h"

using namespace std;

class SolutionInitiale
{
public: 
    static void initialiser_circonscriptions(const int& nb_municipalites, const int& nb_circonscriptions,
                                  vector<unique_ptr<Circonscription>>& circonscriptions);

    static void parcours_diagonales(const int& nb_circonscriptions,
                            vector<unique_ptr<Circonscription>>& circonscriptions,
                            vector<unique_ptr<Circonscription>>& circonscriptions_dispo,
                            const vector<vector<int>>& matrice_votes);

    static int generer_par_diagonales(const int& nb_circonscriptions,
                                vector<unique_ptr<Circonscription>>& circonscriptions,
                                const vector<vector<int>>& matrice_votes, bool& solutionInitialeTrouvee);
    
    static void transfert_solution(vector<shared_ptr<Partition>>& resultatBFS, vector<unique_ptr<Circonscription>>& circonscriptions,
                        const vector<vector<int>>& matrice_votes, Contraintes& contraintes);

    static int generer_par_bfs(const int& nb_circonscriptions,
                    vector<unique_ptr<Circonscription>>& circonscriptions,
                    const vector<vector<int>>& matrice_votes, bool& solutionInitialeTrouvee);
};


#endif //TP3_INF8775_V2_SOLUTIONINITIALE_H
