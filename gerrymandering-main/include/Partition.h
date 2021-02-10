#pragma once
#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <random>
#include <queue>
#include <memory>
class Partition: public std::enable_shared_from_this<Partition>
{
public:
    class Municipalite
    {
        int x_;
        int y_;
        std::shared_ptr<Partition> laCirconsription_;

    public:
        Municipalite();
        Municipalite(int x, int y, std::shared_ptr<Partition> laCirconscription);
        int& getX();
        int& getY();
        void setLaCirconscription(std::shared_ptr<Partition> laCirconscription)
        {
            laCirconsription_ = laCirconscription;
        }
        std::shared_ptr<Partition>& getLaCirconscription();
    };
    struct queueElementsUniques
    {
        std::set<std::shared_ptr<Partition>> s;
        std::queue<std::shared_ptr<Partition>> q;

        void push(std::shared_ptr<Partition>& x)
        {
            auto p = s.insert(x);       // std::pair<std::set<Foo>::iterator, bool>
            if (p.second)
            {
                q.push(*p.first);
            }
        }
        std::shared_ptr<Partition> pop()
        {
            std::shared_ptr<Partition> temp = q.front();
            s.erase(temp);
            q.pop();
            return temp;
        }
        bool empty()
        {
            return q.size() == 0;
        }
        void shuffleOrder()
        {
            std::vector<std::shared_ptr<Partition>> temp;
            while (!q.empty())
            {
                temp.push_back(q.front());
                q.pop();
            }
            std::random_device rd;
            std::mt19937 g(rd());

            std::shuffle(temp.begin(), temp.end(), g);
            s.clear();
            for (auto element : temp)
            {
                s.insert(element);
                q.push(element);
            }
        }
    };

    Partition();
    Partition(int numeroDeCirconscription);
    int getNumeroDeCirconscription();
    int getNombreDeMunicipalites();
    std::list<std::shared_ptr<Municipalite>>& getLesMunicipalites();
    void setNumeroDeCirconscription(int numeroDeCirconscription);
    void ajouterMunicipalite(std::shared_ptr<Municipalite> laMunicipalite);
    std::list<std::shared_ptr<Partition::Municipalite>>::iterator retirerMunicipalite(std::shared_ptr<Municipalite> laMunicipalite);
private:

    int numeroDeCirconscription_;
    int nombreDeMunicipalites_;
    std::list<std::shared_ptr<Partition::Municipalite>> lesMunicipalites_;
};

