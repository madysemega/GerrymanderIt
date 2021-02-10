//
// Created by Chaimaa Khal on 2020-12-09.
//
#ifndef TP3_INF8775_V2_CIRCONSCRIPTION_H
#define TP3_INF8775_V2_CIRCONSCRIPTION_H

#include <cmath>
#include <unordered_set>
#include "Partition.h"

using namespace std;
/**
 * Struct representant la municipalite avec ses indices
 */
struct Municipalite {
    int i = -1;
    int j = -1;

    Municipalite() = default;

    Municipalite(int i_, int j_): i(i_), j(j_){}

    bool operator==(const Municipalite& autre) const{
        return autre.i == i && autre.j == j;
    }
};

/**
 * Creation d'un hash pour Municipalite afin de permettre des retraits et recherches en O(1)
 */
namespace std
{
    template<> struct hash<Municipalite>
    {
        std::size_t operator()(Municipalite const& municipalite) const noexcept
        {
            return ((hash<int>{}(municipalite.i)) ^ (hash<int>{}(municipalite.j) << 1));
        }
    };
}

/**
 * Struct representant une circonscription
 */
class Circonscription{
public:
    unordered_set<Municipalite> municipalites;
    uint64_t nb_votes_vert;
    int nb_municipalites;
    int distance_manhattan;
    /**
     * Constructeur par parametres
     * @param _nb_municipalites : le nb de municipalites maximal pouvant etre compris dans la circonscription
     * @param _distance_manhattan : la distance de manhattan maximale a respecter
     */
    Circonscription(int _nb_municipalites, int _distance_manhattan): nb_votes_vert(0),
                                                                     nb_municipalites(_nb_municipalites),
                                                                     distance_manhattan(_distance_manhattan){};
    /**
     * Constructeur par copie
     * @param c autre circonscription
     */
    Circonscription(const Circonscription& c) = default;

    /**
     * Methode permettant d'ajouter une municipalité a la circonscription
     * @param i : indice i de la municipalite a ajouter
     * @param j : indice j de la municipalite a ajouter
     * @param votes_vert : nb de votes pour le parti vert dans cette municipalite
     * @return true si l'ajout a reussi, false sinon
     */
    bool ajouter_municipalite(int i, int j, int votes_vert){
        // Verification du respect des contraintes
        if (municipalites.size() < nb_municipalites && compatible(i, j)){
            municipalites.emplace(i, j);
            nb_votes_vert += votes_vert;
            return true;
        }
        return false;
    }

    /**
     * Methode permettant de calculer le ratio de vote pour le parti vert
     * @return le ratio
     */
    double ratio_vert() const{
        return ((double)nb_votes_vert)/nb_municipalites/100;
    }

    /**
     * Methode permettant de determiner si le partie vert est vainqueur dans cette circonscription
     * @return true si le parti vert est gagnant, false sinon
     */
    bool vert_gagnant() const{
        return ratio_vert() > 0.5;
    }

    /**
     * Methode permettant de calculer le ratio de vote pour le parti vert avant un echange de municipalite
     * @param votes_out : votes pour les verts de la municipalite faisant partie de la circonscription courrante
     * @param votes_in : votes pour les verts de la municipalite que nous voudrions ajouter
     * @return le nouveau ratio si l'echange s'effectuait
     */
    double ratio_vert_modifier(int votes_out, int votes_in) const{
        return ((double)(nb_votes_vert - votes_out + votes_in))/nb_municipalites/100;
    }

    /**
     * Methode permettant de determiner si le partie vert est vainqueur dans cette circonscription
     * @param votes_in : votes pour les verts de la municipalite que nous voudrions ajouter
     * @param votes_out : votes pour les verts de la municipalte a retirer
     * @return true si les verts sont majoritaire, false sinon
     */
    bool vert_gagnant(int votes_in, int votes_out) const{
        return ratio_vert_modifier(votes_out, votes_in) > 0.5;
    }

    /**
     * Verifie si une municipalite est compatible avec toutes les autres formant la circonscription
     * @param i : indice i de la municipalite
     * @param j : indice j de la municipalite
     * @return true si elle est compatible, false sinon
     */
    bool compatible(int i, int j){
        int distance;
        for (const auto& point : municipalites){
            distance = abs(i - point.i) + abs(j - point.j);
            if (distance > distance_manhattan)
                return false;
        }
        return true;
    }

    /**
     * Methode permettant de retirer une municipalite de la circonscription
     * @param i
     * @param j
     */
    void retirer_municipalite(int i, int j){
        if(municipalites.count(Municipalite(i, j)))
            municipalites.erase(Municipalite(i, j));
    }

   bool ajouter_partition(std::shared_ptr<Partition>& partition, const vector<vector<int>>& matrice_votes)
   {
       for (auto& municipalite : partition->getLesMunicipalites())
       {
           municipalites.insert(Municipalite(municipalite->getX(), municipalite->getY()));
           nb_votes_vert += matrice_votes[municipalite->getX()][municipalite->getY()];
       }
       return true;
   }
};


#endif //TP3_INF8775_V2_CIRCONSCRIPTION_H
