#pragma once
#include <queue>
#include <map>
#include "Partition.h"

struct Contraintes
{
    int nombreDeMunicipalites_;
    int nombreDeCirconscription_;
    int plafond_k;
    int plancher_k;
    int distanceMaxEntreDeuxPoints;
    Contraintes(int nombreDeMunicipalites, int nombreDeCirconscriptions);
    int distanceManhattan(Partition::Municipalite m1, Partition::Municipalite m2);
    int plafondDivision(int x, int y);
    bool ajouterDansCirconscription(std::shared_ptr<Partition::Municipalite>& laMunicipalite, std::shared_ptr<Partition>& laCirconscription,
        bool backtracking, Partition::queueElementsUniques& circonscriptionsIncompletes,
        std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou);
};
