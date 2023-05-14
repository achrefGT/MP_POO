#ifndef TRADER_H_INCLUDED
#define TRADER_H_INCLUDED
#include "Transaction.h"
#include "Portefeuille.h"
#include "Date.h"
#include <random>
#include <cmath>

using namespace std;
class Trader {
    public :
        virtual ~Trader(){};
        virtual Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille)=0;

};

///////////////////////////////// Trader Aleatoire /////////////////////////////////////////////

class TraderAleatoire : public Trader {
    public :
        Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille);

} ;

Transaction TraderAleatoire::choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille) {
    Transaction tx;
    vector<PrixJournalier> actionsDisponibles = bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), portefeuille.getSolde()); // avoir une copie des prix journaliers disponibles
    vector<Titre> titresDisponibles = portefeuille.getTitres();
    if (actionsDisponibles.empty() && titresDisponibles.empty()) return tx;
    int choix = rand() % 3;
    if (actionsDisponibles.empty()) {
        choix = 0;   // si il n'y a pas des actions dans la bourse on fait rien
    }
    if (titresDisponibles.empty()) {
        choix = 2;           // si le portefeuille est vide on achete
    }
    if (portefeuille.getSolde()<2 && !titresDisponibles.empty())
    {
        choix = 1;
    }
    typeTransaction type = rien;
    int i = 0, taille = 0, quantite = 0;
    switch (choix) {
        case 0:
            type = rien;
            break;
        case 1:
            type = vente;
            taille = titresDisponibles.size();
            i = rand() % taille;
            quantite = 1 + rand() % (titresDisponibles[i].getQuantite()); // la quantite a vendre ne doit pas d�passer ce qu'il y a dans le portefeuille
            return Transaction(Titre(titresDisponibles[i].getNomAction(), quantite), type);
        case 2:
            type = achat;
            taille = actionsDisponibles.size();
            i = rand() % taille;
            int quantiteMax = ceil(portefeuille.getSolde()/actionsDisponibles[i].getPrix());
            do{
                i = rand() % taille;
                quantiteMax = floor(portefeuille.getSolde()/actionsDisponibles[i].getPrix());
            }while(quantiteMax<1);

            quantite = 1+ rand() % quantiteMax;
            return Transaction(Titre(actionsDisponibles[i].getNomAction(), quantite), type);
    }
    return tx;
}

///////////////////////////////// Trader Cheapest And Most Valuable /////////////////////////////////////////////


class TraderCheapestAndMostValuable : public Trader {
public:
    Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille);
};

Transaction TraderCheapestAndMostValuable::choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille) {
    Transaction tx;
    vector<PrixJournalier> actionsDisponibles = bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), portefeuille.getSolde());
    vector<Titre> titresDisponibles = portefeuille.getTitres();
    if (actionsDisponibles.empty() && titresDisponibles.empty()) return Transaction ();

    // Find the cheapest and most valuable titles
    string cheapestTitre;
    double cheapestPrice = numeric_limits<double>::max();
    string mostValuableTitre;
    double mostValuablePrice = 0.0;

    for (const PrixJournalier& prix : actionsDisponibles) {
        if (prix.getPrix() < cheapestPrice) {
            cheapestPrice = prix.getPrix();
            cheapestTitre = prix.getNomAction();
        }
        if (prix.getPrix() > mostValuablePrice) {
            mostValuablePrice = prix.getPrix();
            mostValuableTitre = prix.getNomAction();
        }
    }
        double SeuilProfit = 5.15;
    // Sell profitable titles
    for (const Titre& titre : titresDisponibles) {
        if (titre.getQuantite() > 0) {
            bool isProfitable = false;

            for (const PrixJournalier& price : actionsDisponibles) {
                if (price.getNomAction() == titre.getNomAction()) {
                    if (price.getPrix() > cheapestPrice) {
                        isProfitable = true;
                        break;
                    }
                }
            }

            if (!isProfitable) {
                tx = Transaction(Titre(titre.getNomAction(),rand()%100), vente);
                return tx;
            }
        }
    }



        // Sell highly appreciated titles
        for (const PrixJournalier& price : actionsDisponibles) {
            for (const Titre& titre : titresDisponibles) {
                if (titre.getQuantite() > 0 && price.getNomAction() == titre.getNomAction() && price.getPrix() > SeuilProfit+SeuilProfit) {
                    tx = Transaction(Titre(price.getNomAction(),1), vente);
                    return tx;
                }
            }
        }
    // Buy the cheapest title if affordable
    if (cheapestPrice != numeric_limits<double>::max()) {
        double maxAffordableQuantity = portefeuille.getSolde() / cheapestPrice;
        int quantityToBuy = static_cast<int>(floor(maxAffordableQuantity))-25;
        if (quantityToBuy > 0) {
            tx = Transaction(Titre(cheapestTitre, quantityToBuy), achat);
            return tx;
        }
    }

    return tx; // Return empty transaction if no suitable action is found
}

///////////////////////////////// Trader Moyenne /////////////////////////////////////////////

class TraderMoyenne : public Trader {
private:
    float pourcentage;
public:
    TraderMoyenne (int pourcentage=1) : pourcentage(pourcentage){};
    Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille);
    double calculerMoyenne(const vector<double>& valeurs) const;
    double calculerPourcentage(double d1, double d2) const;
    float getPourcentage() const {return pourcentage;};
    void setPourcentage(float pourcent) {pourcentage=pourcent;};
};

double TraderMoyenne::calculerMoyenne(const vector<double>& valeurs) const {
    double somme = 0.0;
    for (const auto& valeur : valeurs) {
        somme += valeur;
    }
    return somme / valeurs.size();
}

double TraderMoyenne::calculerPourcentage(double d1, double d2) const {
    return ((d1 - d2) / d1) * 100.0;
}

Transaction TraderMoyenne::choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille) {
    Transaction tx;
    vector<PrixJournalier> actionsDisponibles = bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), portefeuille.getSolde()); // avoir une copie des prix journaliers disponibles
    vector<Titre> titresDisponibles = portefeuille.getTitres();
    if (actionsDisponibles.empty() && titresDisponibles.empty()) return tx;
    map<string, vector<double>> actions = bourse.getPrixActionParMois();
    if (titresDisponibles.empty()) {
        tuple<string, double> actionMin = make_tuple("", portefeuille.getSolde());
        for (PrixJournalier pj : actionsDisponibles) {
            actionMin = (pj.getPrix() < get<1>(actionMin)) ? make_tuple(pj.getNomAction(), pj.getPrix()) : actionMin;
        }
        return Transaction(Titre(get<0>(actionMin), floor(portefeuille.getSolde() / (10 * get<1>(actionMin)))), achat);
    }
    double pourcent;
    tuple<typeTransaction, string, double, double> benefitMax = make_tuple(rien, "", 0.0, 0.0);
    for (PrixJournalier pj : actionsDisponibles) {
        auto action = actions.find(pj.getNomAction());
        double prixMoyen;
        if (action != actions.end()) {
            prixMoyen = this->calculerMoyenne(action->second);
            if (prixMoyen > pj.getPrix()) {
                pourcent = calculerPourcentage(prixMoyen, pj.getPrix());
                benefitMax = (pourcent >= pourcentage && pourcent >= get<3>(benefitMax)) ? make_tuple(achat, pj.getNomAction(), pj.getPrix(), pourcent) : benefitMax;
            }
            else if (prixMoyen < pj.getPrix() && portefeuille.chercherTitre(pj.getNomAction())) {
                pourcent = -1 * calculerPourcentage(prixMoyen, pj.getPrix());
                benefitMax = (pourcent >= pourcentage && pourcent >= get<3>(benefitMax)) ? make_tuple(vente, pj.getNomAction(), pj.getPrix(), pourcent) : benefitMax;
            }
        }
    }
    pourcent = (get<3>(benefitMax) < 1) ? 1 : get<3>(benefitMax);
    if (get<0>(benefitMax) == achat){
            return Transaction(Titre(get<1>(benefitMax),floor((pourcent/100)*portefeuille.getSolde()/get<2>(benefitMax))),achat);
    }
    if (get<0>(benefitMax)==vente) {
            return Transaction(Titre(get<1>(benefitMax),floor((pourcent/100)*portefeuille.getQuantiteTitre(get<1>(benefitMax)))),vente);
    }
    return tx;
}

///////////////////////////////// Trader Pondere /////////////////////////////////////////////

class TraderPondere : public Trader
{
    public:
        Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille);
};

Transaction TraderPondere::choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille) {
    Transaction tx;
    vector<PrixJournalier> actionsDisponibles = bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), portefeuille.getSolde());
    vector<Titre> titresDisponibles = portefeuille.getTitres();
    if (actionsDisponibles.empty() && portefeuille.getTitres().empty()) {
        return Transaction(); // Rien �  faire si le portefeuille est vide et qu'il n'y a pas d'actions disponibles
    }
    typeTransaction type = rien;
    int quantite = 0;
    double meilleurRatio = 0;

    // Calculate the moyenneMobile values for all PrixJournalier objects in actionsDisponibles
    map<string, double> moyenneMobiles;
    for (const PrixJournalier& prixJournalier : actionsDisponibles) {
        vector<PrixJournalier> prixJournaliers = bourse.getPrixJournaliersParDate(prixJournalier.getDate(), 5);
        double sommePrix = 0;
        for (int j = 0; j < 5; j++) {
            sommePrix += prixJournaliers[j].getPrix();
        }
        double moyenneMobile = sommePrix / 5;
        moyenneMobiles[prixJournalier.getNomAction()] = moyenneMobile;
    }

    for (const PrixJournalier& prixJournalier : actionsDisponibles) {
        double moyenneMobile = moyenneMobiles[prixJournalier.getNomAction()];
        double ratio = prixJournalier.getPrix() / moyenneMobile;
        if (ratio > meilleurRatio) {
            meilleurRatio = ratio;
            type = achat;
            quantite = floor(portefeuille.getSolde() / prixJournalier.getPrix()); // acheter autant que possible avec le solde disponible
            tx = Transaction(Titre(prixJournalier.getNomAction(), quantite), type);
        }
    }
    map<string, PrixJournalier> prixJournaliersMap;
    for (const PrixJournalier& prixJournalier : bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), 5)) {
        prixJournaliersMap[prixJournalier.getNomAction()] = prixJournalier;
    }
    for (const Titre& titre : titresDisponibles) {
        const PrixJournalier& prixJournalier = prixJournaliersMap[titre.getNomAction()];
        double moyenneMobile = moyenneMobiles[titre.getNomAction()];
        double ratio = prixJournalier.getPrix() / moyenneMobile;
        if (ratio < meilleurRatio) {
            meilleurRatio = ratio;
            type = vente;
            quantite = titre.getQuantite(); // vendre toutes les actions de ce type disponibles dans le portefeuille
            tx = Transaction(Titre(titre.getNomAction(), quantite), type);
        }
    }
    return tx;
}

//////////////////////////////////// Tradeur Humain //////////////////////////////////////

class TraderHumain : public Trader{
    public:
        Transaction choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille);

};

Transaction TraderHumain::choisirTransaction(const Bourse& bourse, const Portefeuille& portefeuille){
    fflush(stdin);
    cout << "Les actions dans la bourse : " << endl;
    vector<PrixJournalier> prixJournaliers = bourse.getPrixJournaliersParDate(bourse.getDateAujourdHui(), portefeuille.getSolde());
    for (const PrixJournalier& prix : prixJournaliers){
        cout << " " << prix.getNomAction() << "\t| prix : " << prix.getPrix() << endl;
    }

    if (portefeuille.getTitres().empty()) cout<<endl<<"Votre portefeuille est vide "<<endl;

    else {
        cout<<endl<<"Vous avez dans votre portefeuille : "<<endl;
        vector<Titre> titres = portefeuille.getTitres();
        for (const Titre& titre : titres){
            cout << titre.getNomAction() << ": " << titre.getQuantite() << endl;
        }
    }
    string transaction;
    string nomAction;
    int quantite;
    cout<<"Votre solde est : "<<portefeuille.getSolde()<<endl;
    do{cout<<endl<<"Que voulez-vous faire : ( achat ou vente ou rien ) : ";cin>>transaction;}while(!(transaction == "achat" || transaction == "vente" || transaction == "rien"));
    if (transaction=="rien") return Transaction();
    cout<<endl<<"Donnez le nom de l'action : ";cin>>nomAction;
    do{cout<<endl<<"Donnez la quantite : ";}while(scanf("%d",&quantite)&&quantite<0);
    Titre titre(nomAction,quantite);
    return (transaction=="achat") ? Transaction(titre,achat): Transaction(titre,vente);
}

#endif // TRADER_H_INCLUDED

