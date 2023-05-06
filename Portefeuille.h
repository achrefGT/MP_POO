#ifndef PORTEFEUILLE_H_INCLUDED
#define PORTEFEUILLE_H_INCLUDED
#include "Titre.h"

using namespace std;

class Portefeuille {
    private :
        long solde;
        vector<Titre> titres;
    public :
        Portefeuille(long solde = 0.0):solde(solde), titres() {};
		long getSolde() const;
        vector<Titre> getTitres() const;
//		bool chercherTitre (const Titre titre) const;
		bool ajouterTitre(const Titre& titre);
		bool retirerTitre(const Titre& titre);
        bool deposerMontant (long );
        bool retirerMontant (long );
};
/*
bool chercherTitre (const Titre titre) const{
    bool test = true;
    for(Titre t : Titres){
        if (t.getNomAction()==titre.getNomAction()) test=false;
    }
    return test;
};*/

bool Portefeuille::ajouterTitre (const Titre& titre){
    if (titre.getQuantite()==0) return false;
    for (vector<Titre>::size_type i = 0; i<titres.size(); i++){
		if (titres[i] == titre){                                                    // si l'action existe d�j�
            titres.push_back(Titre(titre.getNomAction(),titre.getQuantite() + titres[i].getQuantite()));          // on ajoute une avec la nouvelle quantit�
			titres.erase(titres.begin() + i);                                                                     // puis on supprime la premi�re
			return true;
		}
    }
	titres.push_back(titre);
    return true;
}

bool Portefeuille::retirerTitre(const Titre& titre) {
    long difference;
    for (vector<Titre>::size_type i = 0; i < titres.size(); i++) {
        if (titres[i] == titre && titres[i].getQuantite() >= titre.getQuantite()) {
            difference = titres[i].getQuantite() - titre.getQuantite();
            if (difference > 0) {                                                                                   // si l'action existe d�j� et la quantite � retirer est < � la quantite existant
                titres.push_back(Titre(titre.getNomAction(), difference));                                          // on diminue la quantit� et on ajoute une avec la nouvelle quantit�
            }                                                                                                       // si l'action existe d�j� et la quantite � retirer est = � la quantite existant on n'ajoute rien
            titres.erase(titres.begin() + i);                                                                       // puis on supprime la premi�re
            return true;
        }
    }
    return false;
}


bool Portefeuille::deposerMontant (long montant){
    if(montant<0) return false;
    solde+=montant;
    return true;
}
bool Portefeuille::retirerMontant (long montant){
    if(montant<0) return false;
    if (solde >= montant){
		solde -= montant;
		return true;
	}
	return false;
}

long Portefeuille::getSolde() const{
    return solde;
}


vector<Titre> Portefeuille::getTitres() const {
    return titres;
}




#endif // PORTEFEUILLE_H_INCLUDED
