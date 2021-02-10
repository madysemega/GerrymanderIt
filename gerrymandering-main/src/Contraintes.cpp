#include "Contraintes.h"

Contraintes::Contraintes(int nombreDeMunicipalites, int nombreDeCirconscriptions)
    :nombreDeMunicipalites_(nombreDeMunicipalites), nombreDeCirconscription_(nombreDeCirconscriptions),
    plafond_k(plafondDivision(nombreDeMunicipalites, nombreDeCirconscriptions)), plancher_k(nombreDeMunicipalites / nombreDeCirconscriptions),
    distanceMaxEntreDeuxPoints(plafondDivision(nombreDeMunicipalites, (2 * nombreDeCirconscriptions)))
{
}
int Contraintes::distanceManhattan(Partition::Municipalite m1, Partition::Municipalite m2)
{
    return std::abs(m1.getX() - m2.getX()) + std::abs(m1.getY() - m2.getY());
}
int Contraintes::plafondDivision(int x, int y)
{
    return (x + y - 1) / y;
}
bool Contraintes::ajouterDansCirconscription(std::shared_ptr<Partition::Municipalite>& laMunicipalite, std::shared_ptr<Partition>& laCirconscription,
    bool backtracking, Partition::queueElementsUniques& circonscriptionsIncompletes,
    std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou)
{
    bool peutEtreAjoute = true;
    int limiteMunicipalites = 0;
    if (backtracking)
        limiteMunicipalites = plancher_k;
    else
        limiteMunicipalites = plafond_k;
    if (((laMunicipalite->getLaCirconscription() == nullptr) || (backtracking)) && laCirconscription->getNombreDeMunicipalites() < limiteMunicipalites
        && !tabou[laMunicipalite->getX()][laMunicipalite->getY()])
    { // Si la municipalite n'était pas déjà dans une circonscription et que la taille de la circonscription n'est est inférieure au plafond de k
        for (auto& municipalityPoint : laCirconscription->getLesMunicipalites())
        {
            if (distanceManhattan(*laMunicipalite, *municipalityPoint) > distanceMaxEntreDeuxPoints)
            {
                peutEtreAjoute = false; // Si la distance de manhattan est plus grande que la distance maximale l'élément ne peut pas être ajouté à la circonscription
                break;
            }
        }
        if (peutEtreAjoute)
        {
            if (laMunicipalite->getLaCirconscription() != nullptr)
            {
                std::shared_ptr<Partition> circonscription = laMunicipalite->getLaCirconscription();
                circonscription->retirerMunicipalite(laMunicipalite);
                registreDesMunicipalites[circonscription]--;
                if (circonscription != laCirconscription && circonscription->getNombreDeMunicipalites() < plancher_k)
                    circonscriptionsIncompletes.push(circonscription);
            }
            laCirconscription->ajouterMunicipalite(laMunicipalite);
            tabou[laMunicipalite->getX()][laMunicipalite->getY()] = true;
        }
    }
    return peutEtreAjoute;
}