#include "partitionnementBFS.h"

/*
* Constructeur par défaut de la classe partitionnnementBFS 
*/
partitionnementBFS::partitionnementBFS() :indexDeDepart({ 0, 0, nullptr }), toutesCombinaisonsEssayee(false) {}

/**
 * Methode servant à generer une solution en parcourant le graphe en bfs pour former des circonscriptions
 * @param municipalities : matrice des municipalites
 * @param debut : index premiere dimension du tableau
 * @param fin : index 2e dimension du tableau
 * @param visites : tableau indiquant les noeuds visités 
 * @return: booléen indiquant si l'index est valide
 */
bool partitionnementBFS::indexValide(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, int debut, int fin,
    std::vector<std::vector<bool>>& visites)
{
    return debut >= 0 && fin >= 0 && debut < municipalites.size() && fin < municipalites[debut].size() && !visites[debut][fin]; // s'assure que l'index est valide pour le tableau � visiter
}

/**
 * Methode servant à trouver à une circonscription contenant plafond(n/m) municipalités
 * @param registreDesMunicipalites : Dictionnaire des circonscriptions
 * @param contraintes : Les contraintes du problème
 * @return: un pointeur vers une circonscription de plafond(n/m) municipalites
 */
std::shared_ptr<Partition> partitionnementBFS::trouverCirconscriptionAvecPlafond(std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, Contraintes& contraintes)
{
    for (auto& it : registreDesMunicipalites)
    {
        if (it.second == contraintes.plafond_k)
            return it.first;
    }
    return nullptr;
}



/**
 * Methode servant à generer une solution en parcourant le graphe en bfs pour former des circonscriptions
 * @param municipalities : matrice des municipalites
 * @param index_X : index premiere dimension du tableau
 * @param index_Y : index 2e dimension du tableau
 * @param visites : tableau indiquant les noeuds visités 
 * @param contraintes : les contraintes du probleme
 * @param parcours : file d'attente des noeuds devant être visités 
 * @param circonscription : La circonscription devant être completé
 * @param backTracking : booleen indiquant si l'algorithme fait du backtracking
 * @param circonscriptionsIncompletes : file d'attente des circonscriptions incompletes
 * @param registreDesMunicipalites : Dictionnaire des circonscriptions 
 * @param tabou : matrice des noeuds tabou (ne devant pas être ajoutés à une circonscription)
 */
void partitionnementBFS::BFS(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, int index_X, int index_Y,
    std::vector<std::vector<bool>>& visites, Contraintes& contraintes, std::queue<std::shared_ptr<Partition::Municipalite>>& parcours,
    std::shared_ptr<Partition>& circonscription, bool backTracking, Partition::queueElementsUniques& circonscriptionsIncompletes,
    std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou)
{
    if (backTracking && circonscription->getNombreDeMunicipalites() == contraintes.plancher_k)
        return;
    if (indexValide(municipalites, index_X, index_Y, visites))
    {
        parcours.push(municipalites[index_X][index_Y]);
        visites[index_X][index_Y] = true;
        contraintes.ajouterDansCirconscription(municipalites[index_X][index_Y], circonscription, backTracking, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
    }
}
/**
 * Methode servant à redistribuer les municipalités pour que toutes les circonscriptions soient complètes
 * @param municipalities : matrice des municipalites
 * @param visites : tableau indiquant les noeuds visités 
 * @param contraintes : les contraintes du probleme
 * @param parcours : file d'attente des noeuds devant être visités 
 * @param circonscriptionsIncompletes : file d'attente des circonscriptions incompletes
 * @param registreDesMunicipalites : Dictionnaire des circonscriptions 
 * @param tabou : matrice des noeuds tabou (ne devant pas être ajoutés à une circonscription)
 */
void partitionnementBFS::BackTracking(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, std::vector<std::vector<bool>>& visites,
    Contraintes& contraintes, std::queue<std::shared_ptr<Partition::Municipalite>>& parcours, Partition::queueElementsUniques& circonscriptionsIncompletes,
    std::map<std::shared_ptr<Partition>, int>& registreDesMunicipalites, std::vector<std::vector<bool>>& tabou)
{  
    while (!circonscriptionsIncompletes.empty())
    {
        visites = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        parcours = std::queue<std::shared_ptr<Partition::Municipalite>>();
        std::shared_ptr<Partition> circonscriptionCourante = circonscriptionsIncompletes.pop();
        if (circonscriptionCourante->getNombreDeMunicipalites() == 0)
        {
            std::shared_ptr<Partition> fournisseur = trouverCirconscriptionAvecPlafond(registreDesMunicipalites, contraintes);
            registreDesMunicipalites[fournisseur]--;
            std::shared_ptr<Partition::Municipalite> municipaliteFournie = fournisseur->getLesMunicipalites().front();
            fournisseur->retirerMunicipalite(municipaliteFournie);
            circonscriptionCourante->ajouterMunicipalite(municipaliteFournie);
            municipaliteFournie->getLaCirconscription() = circonscriptionCourante;
            if (fournisseur->getNombreDeMunicipalites() < contraintes.plancher_k)
                circonscriptionsIncompletes.push(fournisseur);
            tabou[municipaliteFournie->getX()][municipaliteFournie->getY()] = true;
        }
        for (auto& municipalite : circonscriptionCourante->getLesMunicipalites())
        {
            tabou[municipalite->getX()][municipalite->getY()] = true;
        }
        std::shared_ptr<Partition::Municipalite> municipaliteCourante = circonscriptionCourante->getLesMunicipalites().front();
        int x_courant = municipaliteCourante->getX();
        int y_courant = municipaliteCourante->getY();
        visites[x_courant][y_courant] = true;
        parcours.push(municipaliteCourante);
        while (!parcours.empty() && circonscriptionCourante->getNombreDeMunicipalites() < contraintes.plancher_k)
        {
            x_courant = parcours.front()->getX();
            y_courant = parcours.front()->getY();
            parcours.pop();
            BFS(municipalites, x_courant - 1, y_courant, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            BFS(municipalites, x_courant, y_courant + 1, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            BFS(municipalites, x_courant + 1, y_courant, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            BFS(municipalites, x_courant, y_courant - 1, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            BFS(municipalites, x_courant - 1, y_courant - 1, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            BFS(municipalites, x_courant + 1, y_courant + 1, visites, contraintes, parcours, circonscriptionCourante, true, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
        }
    }
}

/**
 * Methode servant à trouver la prochaine municipalité non assignée
 * @param municipalities : matrice des municipalites
 * @param parcours : file d'attente des noeuds devant être visités 
 * @param toutesMunicipalitesAssignee : booleen indiquant si toutes les municipalités ont été assignées
 */
void partitionnementBFS::trouverProchaineMunicipalite(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>>& municipalites, std::queue<std::shared_ptr<Partition::Municipalite>>& parcours, bool& toutesMunicipalitesAssignee)
{
    for (int i = 0; i < municipalites.size(); i++)
    {
        for (int j = 0; j < municipalites[i].size(); j++)
        {
            if (municipalites[i][j]->getLaCirconscription() == nullptr)
            {
                parcours.push(municipalites[i][j]);
                return;
            }
        }
    }
    toutesMunicipalitesAssignee = true;
}

/**
 * Methode servant à generer une solution en parcourant le graphe en bfs pour former des circonscriptions
 * @param lesCirconscriptions : la liste des circonscriptions
 * @param visites : tableau indiquant les noeuds visités 
 * @param contraintes : les contraintes du probleme
 * @param parcours : file d'attente des noeuds devant être visités 
 * @param circonscriptionsIncompletes : file d'attente des circonscriptions incompletes
 * @param registreDesMunicipalites : Dictionnaire des circonscriptions 
 * @param tabou : matrice des noeuds tabou (ne devant pas être ajoutés à une circonscription)
 */
void partitionnementBFS::trouverCirconscriptionsIncompletes(std::vector<std::shared_ptr<Partition>>& lesCirconscriptions, Partition::queueElementsUniques& circonscriptionsIncompletes, Contraintes& contraintes)
{
    for (auto& circonscription : lesCirconscriptions)
    {
        if (circonscription->getNombreDeMunicipalites() < contraintes.plancher_k)
            circonscriptionsIncompletes.push(circonscription);
    }
}



/**
 * Methode servant à former les circonscriptions selon les contraintes du problème
 * @param municipalities : matrice des municipalites
 * @param contraintes : les contraintes du probleme
 * @return : la liste des circonscriptions
 */
std::vector<std::shared_ptr<Partition>> partitionnementBFS::formerCirconscriptions(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> municipalites, Contraintes& contraintes)
{
    std::queue<std::shared_ptr<Partition::Municipalite>> parcours;
    parcours.push(municipalites[indexDeDepart.getX()][indexDeDepart.getY()]);
    std::vector<std::shared_ptr<Partition>> lesCirconscriptions;
    Partition::queueElementsUniques circonscriptionsIncompletes;
    std::map<std::shared_ptr<Partition>, int> registreDesMunicipalites;
    std::vector<std::vector<bool>> visites(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
    std::vector<std::vector<bool>> tabou(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
    bool toutesMunicipalitesAssignee = false;
    // Pour chaque circonscriptions
    for (int k = 0; k < contraintes.nombreDeCirconscription_; k++)
    {
        std::shared_ptr<Partition> circonscription = std::make_shared<Partition>(k);
        tabou = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        visites = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        if (toutesMunicipalitesAssignee)
        {
            std::shared_ptr<Partition> fournisseur = trouverCirconscriptionAvecPlafond(registreDesMunicipalites, contraintes);
            registreDesMunicipalites[fournisseur]--;
            std::shared_ptr<Partition::Municipalite> municipaliteFournie = fournisseur->getLesMunicipalites().front();
            fournisseur->retirerMunicipalite(municipaliteFournie);
            circonscription->ajouterMunicipalite(municipaliteFournie);
            municipaliteFournie->getLaCirconscription() = circonscription;
            if (fournisseur->getNombreDeMunicipalites() < contraintes.plancher_k)
                circonscriptionsIncompletes.push(fournisseur);
            if (circonscription->getNombreDeMunicipalites() < contraintes.plancher_k)
                circonscriptionsIncompletes.push(circonscription);
            tabou[municipaliteFournie->getX()][municipaliteFournie->getY()] = true;
            BackTracking(municipalites, visites, contraintes, parcours, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            tabou = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        }
        else
        {
            circonscription->ajouterMunicipalite(parcours.front());
            parcours.front()->getLaCirconscription() = circonscription;
            visites[parcours.front()->getX()][parcours.front()->getY()] = true;
            // Maximiser le nombre de municipalites dans les circonscriptions en respectant les constraintes
            while (!parcours.empty() && circonscription->getNombreDeMunicipalites() < contraintes.plafond_k)
            {
                int x_courant = parcours.front()->getX();
                int y_courant = parcours.front()->getY();
                parcours.pop();
                BFS(municipalites, x_courant - 1, y_courant, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
                BFS(municipalites, x_courant, y_courant + 1, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
                BFS(municipalites, x_courant + 1, y_courant, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
                BFS(municipalites, x_courant, y_courant - 1, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
                BFS(municipalites, x_courant - 1, y_courant - 1, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
                BFS(municipalites, x_courant + 1, y_courant + 1, visites, contraintes, parcours, circonscription, false, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            }

        }
        parcours = std::queue<std::shared_ptr<Partition::Municipalite>>();
        if (circonscription->getNombreDeMunicipalites() < contraintes.plancher_k)
        {
            visites = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
            circonscriptionsIncompletes.push(circonscription);
            if (circonscription->getNombreDeMunicipalites() < 1)
                k--;
            else
            {
                for (auto& municipalite : circonscription->getLesMunicipalites())
                {
                    tabou[municipalite->getX()][municipalite->getY()] = true;
                }
            }
            BackTracking(municipalites, visites, contraintes, parcours, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
            tabou = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        }
        trouverCirconscriptionsIncompletes(lesCirconscriptions, circonscriptionsIncompletes, contraintes);
        if (circonscription->getNombreDeMunicipalites() <= contraintes.plafond_k && circonscription->getNombreDeMunicipalites() >= contraintes.plancher_k)
        {
            registreDesMunicipalites[circonscription] = circonscription->getNombreDeMunicipalites();
            lesCirconscriptions.push_back(circonscription);
        }
        else
        {
            circonscriptionsIncompletes.push(circonscription);
        }
        parcours = std::queue<std::shared_ptr<Partition::Municipalite>>();
        trouverProchaineMunicipalite(municipalites, parcours, toutesMunicipalitesAssignee);
    }
    int compteurCompletions = 0;
    while (!circonscriptionsIncompletes.empty() && compteurCompletions < 10)
    {
        tabou = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        visites = std::vector<std::vector<bool>>(municipalites.size(), std::vector<bool>(municipalites[0].size(), false));
        BackTracking(municipalites, visites, contraintes, parcours, circonscriptionsIncompletes, registreDesMunicipalites, tabou);
        trouverCirconscriptionsIncompletes(lesCirconscriptions, circonscriptionsIncompletes, contraintes);
        compteurCompletions++;
    }
    if (indexDeDepart.getY() < municipalites[indexDeDepart.getX()].size() - 1)
        indexDeDepart.getY()++;
    else
    {
        if (indexDeDepart.getX() < municipalites.size() - 1) {
            indexDeDepart.getX()++;
            indexDeDepart.getY() = 0;
        }
        else
        {
            toutesCombinaisonsEssayee = true;
            indexDeDepart.getX() = 0;
            indexDeDepart.getY() = 0;
            indexDeDepart.getLaCirconscription() = nullptr;
        }
    }
    return lesCirconscriptions;
}

/**
 * Methode servant à compter le nombre de circonscriptions vertes
 * @param municipalities : matrice des municipalites
 * @param circonscriptions : la liste des circonscriptions
 * @return : entier indiquant le nombre de circonscriptions vertes
 */
int partitionnementBFS::compterCirconscriptionsVertes(std::vector<std::vector<int>>& municipalities, std::vector<std::shared_ptr<Partition>> circonscriptions)
{
    int circonscriptionsVertes = 0;
    for (int i = 0; i < circonscriptions.size(); i++)
    {
        int somme = 0;
        for (auto& municipalite : circonscriptions[i]->getLesMunicipalites())
        {
            somme += municipalities[municipalite->getX()][municipalite->getY()];
        }
        double pourcentageVert = (double)somme / (100 * circonscriptions[i]->getNombreDeMunicipalites());
        if (pourcentageVert > 0.5)
            circonscriptionsVertes++;
    }
    return circonscriptionsVertes;
}

/**
 * Methode servant à generer une solution en parcourant le graphe en bfs pour former des circonscriptions à partir d'index aléatoires
 * @param points : la liste des points 2d
 * @param contraintes : les contraintes du probleme
 * @param solutionInitialeTrouvee : file d'attente des circonscriptions incompletes
 * @return : la liste des circonscriptions
 */
std::vector<std::shared_ptr<Partition>> partitionnementBFS::genererCirconscription(std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> points,
 Contraintes& contraintes, bool& solutionInitialeTrouvee)
{
    int compteurEssai = 0;
    std::vector<std::shared_ptr<Partition>> resultat;
    while (!toutesCombinaisonsEssayee && !solutionInitialeTrouvee)
    {
        std::vector<std::shared_ptr<Partition>> lesCirconscriptions = formerCirconscriptions(points, contraintes);
        int totalSize = 0;
        bool partitionTailleCorrecte = true;
        for (int i = 0; i < lesCirconscriptions.size(); i++)
        {
            if (lesCirconscriptions[i]->getNombreDeMunicipalites() < contraintes.plancher_k || lesCirconscriptions[i]->getNombreDeMunicipalites() > contraintes.plafond_k)
            {
                partitionTailleCorrecte = false;
                break;
            }
            totalSize += lesCirconscriptions[i]->getNombreDeMunicipalites();
        }
        if (totalSize == contraintes.nombreDeMunicipalites_ && partitionTailleCorrecte)
            solutionInitialeTrouvee = true;
        compteurEssai++;
        for (int i = 0; i < points.size(); i++)
        {
            for (int j = 0; j < points[i].size(); j++)
            {
                points[i][j]->getLaCirconscription() = nullptr;
            }
        }
        if (solutionInitialeTrouvee)
            resultat = lesCirconscriptions;
    }
    indexDeDepart.getX() = 0;
    indexDeDepart.getY() = 0;
    indexDeDepart.getLaCirconscription() = nullptr;
    toutesCombinaisonsEssayee = false;
    return resultat;
}

/**
 * Methode servant à generer une solution en parcourant le graphe en bfs pour former des circonscriptions
 * @param matriceMunicipalites : matrice des municipalites
 * @return : la liste des points 2d
 */
std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> partitionnementBFS::determinerListeDesPoints(const std::vector<std::vector<int>>& matriceMunicipalites)
{
    std::vector<std::vector<std::shared_ptr<Partition::Municipalite>>> points(matriceMunicipalites.size(), std::vector<std::shared_ptr<Partition::Municipalite>>());

    for (int i = 0; i < matriceMunicipalites.size(); i++)
    {
        for (int j = 0; j < matriceMunicipalites[i].size(); j++)
        {
            points[i].push_back(std::make_shared<Partition::Municipalite>(i, j, nullptr));
        }
    }
    return points;
}
