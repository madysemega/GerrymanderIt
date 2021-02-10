#include "AmeliorationLocale.h"

/**
 * Méthode permettant d'attendre la fin du premier thread qui trouvera le partitionnement initial
 * @param f1 : reference au premier thread
 * @param f2 : reference au second thread
 * @return true la valeur de retour du premier thread qui a trouvé la partition
 */
future<int> AmeliorationLocale::attendre_premier_resultat(future<int> f1, future<int> f2)
{
  auto donnee = make_shared<AttendrePremierResultat<int>>(); // Variable pour stocker les informations sur le premier future terminé

  promise<int> attendrePremierePromesse; // promise retournée par le premier future
  future<int> attendrePremierFuture = attendrePremierePromesse.get_future(); 

  thread attendre_premier_thread{
      [](promise<int> p, shared_ptr<AttendrePremierResultat<int>> donnee) {
        unique_lock<mutex> lk(donnee->res_mutex); // Mutex pour empêcher les deux threads d'accéder en même temps à la donnée 
        donnee->res_cv.wait(lk, [&]() { return donnee->resultat_pret; }); // lorsque l'un des deux threads retourne une valeur, mettre cette valeur dans la variable résultat

        p.set_value(donnee->res);
      },
      move(attendrePremierePromesse), donnee}; 

  auto thread_lambda = [](future<int> f,
                          shared_ptr<AttendrePremierResultat<int>> donnee) {
    int r = f.get();
    lock_guard<mutex> lk(donnee->res_mutex); // mutex pour assurer l'exclusion mutuel de la valeur de retour qui sera recupérée du premier thread qui finira
    if (!donnee->resultat_pret) {
      donnee->resultat_pret = true; // indique que le resultat est prêt
      donnee->res = r; // sauvegarde de la valeur de retour du thread 
      donnee->res_cv.notify_one(); // debloque le thread qui attend que le mutex soit liberé pour continuer son execution
    }
  }; // dès que l'un des thread est terminé, récupérer sa valeur de retour
    thread t1{thread_lambda, move(f1), (donnee)},
      t2{thread_lambda, move(f2), (donnee)}; 

  attendre_premier_thread.detach(); // arrêter le thread surveillant
  t1.detach(); // arrêter le thread 1
  t2.detach(); // arrêter le thread 2

  return attendrePremierFuture;
}

/**
 * Methode permettant de determiner si la permutation des circonscriptions de deux municipalites respecte les criteres
 * @param municipalite1 : la premiere municipalite
 * @param municipalite2 : la seconde municipalite
 * @return true si les criteres sont respectes, false sinon
 */
bool AmeliorationLocale::permutation_possible(Municipalite municipalite1, Municipalite municipalite2) {
    int id_circonscription1 = matrice_circonscriptions[municipalite1.i][municipalite1.j];
    int id_circonscription2 = matrice_circonscriptions[municipalite2.i][municipalite2.j];
    return (id_circonscription1 != id_circonscription2
            && circonscriptions[id_circonscription1]->compatible(municipalite2.i, municipalite2.j)
            && circonscriptions[id_circonscription2]->compatible(municipalite1.i, municipalite1.j));
}

/**
 * Methode permettant de trouver tous les voisins d'une municipalite distants d'au plus la distance de manhattan
 * @param i : indice i de la municipalite
 * @param j : indice j de la municipalite
 * @param voisins : vecteur qui contiendra les voisins trouves
 */
void AmeliorationLocale::trouver_voisins(int i, int j, vector<Municipalite> &voisins) const {
    voisins.clear();
    for (int x = max(0, i - distance_manhattan - 1); x < min(nb_lignes, i + distance_manhattan + 1); x++) {
        for (int y = max(0, j - distance_manhattan - 1); y < min(nb_colonnes, j + distance_manhattan + 1); y++) {
            if ((x != i || y != j) && (abs(x - i) + abs(y - j)) <= distance_manhattan) {
                voisins.emplace_back(x,y);
            }
        }
    }
}

/**
 * Methode permettant de trouver le meilleur voisin pour "crack" les votes -> se rapprocher le plus d'un ratio de
 * 50% (sans l'atteindre) pour la municipalite dont fait partie la circonscription
 * @param municipalite : la municipalite a considerer
 * @param voisins : le voisinage a considerer
 * @return le meilleur voisin trouve
 */
Municipalite AmeliorationLocale::meilleur_voisin_concentrer(Municipalite municipalite, const vector<Municipalite> &voisins) {
    int id_municipalite = matrice_circonscriptions[municipalite.i][municipalite.j];
    int votes_municipalite = matrice_votes[municipalite.i][municipalite.j];

    Municipalite meilleur_voisin;
    double diff_min = DBL_MAX;
    bool vert_majoritaire = circonscriptions[id_municipalite]->vert_gagnant();

    for(auto const& voisin: voisins) {
        int id_municipalite_voisin = matrice_circonscriptions[voisin.i][voisin.j];
        if (permutation_possible(municipalite, voisin)){
            int votes_voisin = matrice_votes[voisin.i][voisin.j];
            double ratio = circonscriptions[id_municipalite]->ratio_vert_modifier(votes_municipalite, votes_voisin);
            if (!vert_majoritaire && ratio < 0.5)
                ratio = circonscriptions[id_municipalite_voisin]->ratio_vert_modifier(votes_voisin, votes_municipalite);
            if (ratio > 0.5 && (ratio - 0.5) < diff_min) {
                if(circonscriptions[id_municipalite]->vert_gagnant()
                    && circonscriptions[id_municipalite_voisin]->ratio_vert() <= SEUIL_DILUTION)
                        continue;
                meilleur_voisin = voisin;
                diff_min = ratio - 0.5;
                vert_majoritaire = true;
            }
            // Si aucun voisin ne permet de rendre le parti vert gagant pour la circonscription de la municipalite,
            // on essayera de rendre une circonscription voisine majoritairement vertes

        }
    }
    return meilleur_voisin;
}

/**
 * Methode permettant de trouver le meilleur voisin pour "pack" les votes -> se rapprocher le plus d'un ratio de
 * 0% pour la municipalite dont fait partie la circonscription
 * @param municipalite : la municipalite a considerer
 * @param voisins : le voisinage a considerer
 * @return le meilleur voisin trouve
 */
Municipalite AmeliorationLocale::meilleur_voisin_diluer(Municipalite municipalite,
                                                        const vector<Municipalite> &voisins) {
    Municipalite meilleur_voisin;
    double min_nb_votes_voisin = DBL_MAX;
    for(auto const& voisin: voisins) {
        if (permutation_possible(municipalite, voisin)){
            int votes_voisin = matrice_votes[voisin.i][voisin.j];
            if (votes_voisin < min_nb_votes_voisin) {
                meilleur_voisin = voisin;
                min_nb_votes_voisin = votes_voisin;
            }
        }
    }
    return meilleur_voisin;
}

/**
 * Methode permettant de trouver le meilleur voisin de la municipalite consideree
 * @param i : indice i de la municipalite
 * @param j : indice j de la municipalite
 * @return : le meilleur voisin en cas de succes, Municipalite(-1,-1) en cas d'echec
 */
Municipalite AmeliorationLocale::trouver_meilleur_voisin(int i, int j) {
    int id_circonscription = matrice_circonscriptions[i][j];
    vector<Municipalite> voisins;
    trouver_voisins(i, j, voisins);
    if (circonscriptions[id_circonscription]->ratio_vert() < SEUIL_DILUTION)
        return meilleur_voisin_diluer(Municipalite(i,j), voisins);
    else
        return meilleur_voisin_concentrer(Municipalite(i,j), voisins);
}

/**
 * Methode s'occupant de l'affichage sur le terminal tel que demande
 * @param vecteur : un vecteur contenant les informations de toutes les circonscriptions
 */
void AmeliorationLocale::afficher() {
    for(int i = 0; i < circonscriptions.size(); i++){
        for(auto const& municipalite: circonscriptions.at(i)->municipalites)
            cout << municipalite.i << " " << municipalite.j << " ";
        cout << endl;
    }
    cout << endl;
}

/**
 * Methode permettant de generer une matrice des circonscriptions a partir d'une solution initiale, ce qui utile pour
 * faciler les permutations de munucipalites entre circonscriptions
 */
void AmeliorationLocale::generer_matrice_circonscriptions() {
    int id = 0;
    for (auto const& val : circonscriptions){
        for (auto const& municipalite : val->municipalites) {
            matrice_circonscriptions[municipalite.i][municipalite.j]=id;
        }
        id++;
    }
}

/**
 * Methode permettant de determiner le nombre de circonscriptions majoritairement vertes
 * @return le nb de circonscriptions majoritairement vertes
 */
int AmeliorationLocale::nb_circ_vertes() {
    int nb_circ_vertes = 0;
    for (auto const& val : circonscriptions){
        nb_circ_vertes += (val->vert_gagnant() ? 1 : 0);
    }
    return nb_circ_vertes;
}

/**
 * Methode permettant d'evaluer la nouvelle solution si une permutation entre deux municipalites avait lieu
 * @param solution_passee : la solution a l'iteration precedente
 * @param municipalite1 : premiere municipalite a permuter
 * @param municipalite2 : seconde municipalite a permuter
 * @return la valeur de la solution si la permutation avait lieu
 */
int AmeliorationLocale::evaluer_solution(int solution_passee, Municipalite municipalite1, Municipalite municipalite2) {
    int id_circ1 = matrice_circonscriptions[municipalite1.i][municipalite1.j];
    int id_circ2 = matrice_circonscriptions[municipalite2.i][municipalite2.j];

    int votes_circ1 = matrice_votes[municipalite1.i][municipalite1.j];
    int votes_circ2 = matrice_votes[municipalite2.i][municipalite2.j];

    int variation_votes = (circonscriptions[id_circ1]->vert_gagnant(votes_circ2, votes_circ1) ? 1 : 0)
                        + (circonscriptions[id_circ2]->vert_gagnant(votes_circ1, votes_circ2) ? 1 : 0)
                        - (circonscriptions[id_circ1]->vert_gagnant() ? 1 : 0)
                        - (circonscriptions[id_circ2]->vert_gagnant() ? 1 : 0);

    return solution_passee + variation_votes;
}

/**
* Methode principale d'améliorations locales
* @param nb_circonscriptions : le nombre de circonscriptions
* @param _matrice_votes : le résultat des votes pour chaque municipalite
* @param afficher_matrice : specifie si l'argument -p est present ou non
*/
void AmeliorationLocale::truquer_election(const int &nb_circonscriptions, const vector<vector<int>> &_matrice_votes, bool afficher_matrice) {
    matrice_votes = _matrice_votes;

    // 1. GENERATION DE LA SOLUTION INITIALE
    bool solution_trouvee = false;
    vector<unique_ptr<Circonscription>> circonscriptions_BFS;
    vector<unique_ptr<Circonscription>> circonscriptions_diagonales;
    // Wrapper associant les attributs necessaires à l'appel de la méthode SolutionInitiale::generer_par_bfs
    auto generer_par_bfs = bind(&SolutionInitiale::generer_par_bfs, ref(nb_circonscriptions), ref(circonscriptions_BFS), 
    ref(matrice_votes), ref(solution_trouvee));
    // Wrapper associant les attributs necessaires à l'appel de la méthode SolutionInitiale::generer_par_diagonales
    auto generer_par_diagonales = bind(&SolutionInitiale::generer_par_diagonales, ref(nb_circonscriptions), ref(circonscriptions_diagonales), 
    ref(matrice_votes), ref(solution_trouvee));
    // Lance un thread pour trouver la solution initiale à partir de la méthode SolutionInitiale::generer_par_bfs
    auto resultat_BFS = async(launch::async, ref(generer_par_bfs));
    // Lance un thread pour trouver la solution initiale à partir de la méthode SolutionInitiale::generer_par_diagonales
    auto resultat_diagonales = async(launch::async, ref(generer_par_diagonales));
    // La solution est obtenue dès que resultat_BFS ou resultat_diagonales retourne une valeur
    auto solution = attendre_premier_resultat(move(resultat_BFS), move(resultat_diagonales));
    if(solution.get() == 1 && solution_trouvee)
    {
        for(auto& circonscription: circonscriptions_BFS)
        {
            circonscriptions.push_back(move(circonscription));
        }
    }else
    {
        for(auto& circonscription: circonscriptions_diagonales)
        {
            circonscriptions.push_back(move(circonscription));
        }
    }
    
    
    int meilleure_solution = nb_circ_vertes(); // la meilleure solution est initialiser a la valeur de la solution init
    if(afficher_matrice)
        afficher();
    else
        cout << meilleure_solution << endl;
    // 2. INITIALISATION DES DIFFERENTS ATTRIBUTS ET VARIABLES LOCALES
    distance_manhattan = circonscriptions.at(0)->distance_manhattan;

    int solution_courante;

    nb_lignes = matrice_votes.size();
    nb_colonnes = matrice_votes[0].size();

    matrice_circonscriptions.resize(nb_lignes, vector<int>(nb_colonnes));
    generer_matrice_circonscriptions();

    // 3. GENERATION D'UN SEED PERMETTANT LE CHOIX ALEATOIRE D'UNE MUNICIPALITE
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist_i(0, matrice_votes.size()-1);
    uniform_int_distribution<int> dist_j(0, matrice_votes[0].size()-1);

    while (true){
        // 1. Trouver une municipalité
        int rand_i = dist_i(mt);
        int rand_j = dist_j(mt);

        // 2. Trouver un voisin pouvant etre permutter avec cette municipalite de telle sorte a "pack" et "crack"
        Municipalite meilleur_voisin = AmeliorationLocale::trouver_meilleur_voisin(rand_i, rand_j);
        if (meilleur_voisin == Municipalite()) // dans le cas ou aucun voisin n'a ete trouve
            continue;

        // 3. Evaluer la solution si la permutation avait lieu
        solution_courante = evaluer_solution(meilleure_solution,Municipalite(rand_i, rand_j), meilleur_voisin);

        bool permuter = false;

        // 4. Si la solution est meilleure que la solution optimale, l'afficher
        if (solution_courante > meilleure_solution){
            meilleure_solution = solution_courante;
            permuter = true;
            if(afficher_matrice)
                afficher();
            else
                cout << meilleure_solution << endl;
        } else {
            if(solution_courante == meilleure_solution)
                permuter = true;
        }

        // 5. Si la solution courante ne deteriore pas la solution precedente, effectuer la permutation
        if (permuter){
            int id_circ1 = matrice_circonscriptions[rand_i][rand_j];
            int id_circ2 = matrice_circonscriptions[meilleur_voisin.i][meilleur_voisin.j];
            circonscriptions[id_circ1]->retirer_municipalite(rand_i, rand_j);
            circonscriptions[id_circ1]->municipalites.emplace(meilleur_voisin.i, meilleur_voisin.j);
            circonscriptions[id_circ1]->nb_votes_vert += matrice_votes[meilleur_voisin.i][meilleur_voisin.j] - matrice_votes[rand_i][rand_j];
            circonscriptions[id_circ2]->retirer_municipalite(meilleur_voisin.i, meilleur_voisin.j);
            circonscriptions[id_circ2]->municipalites.emplace(rand_i, rand_j);
            circonscriptions[id_circ2]->nb_votes_vert += matrice_votes[rand_i][rand_j] - matrice_votes[meilleur_voisin.i][meilleur_voisin.j];

            swap(matrice_circonscriptions[rand_i][rand_j], matrice_circonscriptions[meilleur_voisin.i][meilleur_voisin.j]);
        }
    }
}

