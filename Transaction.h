#ifndef TRANSACTION_H_INCLUDED
#define TRANSACTION_H_INCLUDED

using namespace std;

enum typeTransaction {vente,achat};

class Transaction {
    private :
        string nomAction;
        int quantit�;
        typeTransaction type;
};


#endif // TRANSACTION_H_INCLUDED
