#include <string>
#include <iostream>
#include <iomanip> 
#include <queue>
#include <tuple>
using namespace std;

/*** Customer class ***/
class Customer {
    public:
        int id; 
        double balance; 
        double probTXN;
        bool completedTXN;
        int TXNCount; 
        string description; 

        Customer();
        void initCustomer(int id, double balance);
};

/*** Transanction queue Class ***/
class TXN {
    public: 
    int TXNCode;
    int user;
    int recipient;
    int time;
    double amountD;
    int buyIdx;
    int sellIdx;
    string note;
    
    TXN newTXN(int code,int buyer,int seller, int time, int, int, double);
    bool equalTXNs(TXN);
    bool validTXN(vector<Customer>&);
    void performTXN(vector<Customer>&);
    void print(ofstream&);
};

class TXNQueue {
    public:
        int sucessTXNs;
        queue<TXN> transactions;
        queue<tuple<bool,int,int>> minersJoinRemove;

        TXNQueue();
        void addTXN(int code,int buyer,int seller, int time, int, int, double);
        void deleteTXN();
        TXN getNextTXN();
        void generateInitialEvents(int&, ofstream&, vector<Customer>&, int&, 
                            tuple<int,int>&);
        void generateNewEvents(int&, ofstream&, vector<Customer>&, int&, 
        tuple<int,int>&);
        bool emptyQueue();
};