#include "Partition.h"


Partition::Partition()
    : numeroDeCirconscription_(-1), nombreDeMunicipalites_(0),
    lesMunicipalites_(std::list<std::shared_ptr<Municipalite>>())
{

}


Partition::Partition(int numeroDeCirconscription)
    :numeroDeCirconscription_(numeroDeCirconscription), lesMunicipalites_(std::list<std::shared_ptr<Municipalite>>()), nombreDeMunicipalites_(0)
{
}


int Partition::getNumeroDeCirconscription()
{
    return numeroDeCirconscription_;
}



int Partition::getNombreDeMunicipalites()
{
    return nombreDeMunicipalites_;
}



std::list<std::shared_ptr<Partition::Municipalite>>& Partition::getLesMunicipalites()
{
    return lesMunicipalites_;
}



void Partition::setNumeroDeCirconscription(int numeroDeCirconscription)
{
    numeroDeCirconscription_ = numeroDeCirconscription;
}




void Partition::ajouterMunicipalite(std::shared_ptr<Municipalite> laMunicipalite)
{
    lesMunicipalites_.push_back(laMunicipalite);
    laMunicipalite->setLaCirconscription(shared_from_this());
    nombreDeMunicipalites_++;
}

std::list<std::shared_ptr<Partition::Municipalite>>::iterator Partition::retirerMunicipalite(std::shared_ptr<Municipalite> laMunicipalite)
{
    auto it = std::find_if(lesMunicipalites_.begin(), lesMunicipalites_.end(), [&](std::shared_ptr<Municipalite>& candidat)
        {
            return candidat->getX() == laMunicipalite->getX() && candidat->getY() == laMunicipalite->getY();
        });
    if (it != lesMunicipalites_.end())
    {
        it = lesMunicipalites_.erase(it);
        nombreDeMunicipalites_--;
        return it;
    }
    return lesMunicipalites_.end();
}

Partition::Municipalite::Municipalite()
    :x_(-1), y_(-1), laCirconsription_(nullptr)
{

}


Partition::Municipalite::Municipalite(int x, int y, std::shared_ptr<Partition> laCirconscription)
    :x_(x), y_(y), laCirconsription_(laCirconscription)
{

}


int& Partition::Municipalite::getX()
{
    return x_;
}


int& Partition::Municipalite::getY()
{
    return y_;
}


std::shared_ptr<Partition>& Partition::Municipalite::getLaCirconscription()
{
    return laCirconsription_;
}
