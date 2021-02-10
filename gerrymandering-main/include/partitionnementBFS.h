#pragma once
#include "Contraintes.h"
#include <map>
#include <vector>
#include <iostream>
class partitionnementBFS
{
private:
    Partition::Municipalite indexDeDepart;
    bool toutesCombinaisonsEssayee;
    bool indexValide(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, int debut, int fin,
        std::vector<std::vector<bool>>& visites);
    std::shared_ptr<Partition> trouverCirconscriptionAvecPlafond(std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, Contraintes& contraintes);
    void BFS(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, int index_X, int index_Y,
        std::vector<std::vector<bool>>& visites, Contraintes& contraintes, std::queue<std::shared_ptr<Partition::Municipalite>>& parcours,
        std::shared_ptr<Partition>& circonscription, bool backTracking, Partition::queueElementsUniques& circonscriptionsIncompletes,
        std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou);
    void BackTracking(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, std::vector<std::vector<bool>>& visites,
        Contraintes& contraintes, std::queue<std::shared_ptr<Partition::Municipalite>>& parcours, Partition::queueElementsUniques& circonscriptionsIncompletes,
        std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou);
    void trouverProchaineMunicipalite(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, 
        std::queue<std::shared_ptr<Partition::Municipalite>>& parcours, bool& toutesMunicipalitesAssignee);
    void trouverCirconscriptionsIncompletes(std::vector<std::shared_ptr<Partition>>& lesCirconscriptions, Partition::queueElementsUniques& circonscriptionsIncompletes, Contraintes& contraintes);
    std::vector<std::shared_ptr<Partition>> formerCirconscriptions(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> municipalites, Contraintes& contraintes);
public:
    partitionnementBFS();
    int compterCirconscriptionsVertes(std::vector<std::vector<int>>& municipalities, std::vector<std::shared_ptr<Partition>> circonscriptions);
    std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> determinerListeDesPoints(const std::vector<std::vector<int>>& matriceMunicipalites);
    std::vector<std::shared_ptr<Partition>>  genererCirconscription(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> points, Contraintes& contraintes, bool& solutionInitialeTrouvee);
};

