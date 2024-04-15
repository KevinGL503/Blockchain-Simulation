#include <string>
#include <iostream>
#include <iomanip> 
#include <queue>
#include <cmath>
#include <fstream>
#include "EventsQueue.h"
using namespace std;

/******************************************************************************
    ****** Customer class functions declarations********
******************************************************************************/
Customer :: Customer() {
    
}

void Customer::initCustomer(int id, double balance){
    this->id = id; 
    this->balance = balance; 
    this->probTXN = 0; 
    this->completedTXN = 0;
    this->TXNCount = 0; 
    this->description = "";  
}

/******************************************************************************
    ****Transaction class functions declarations ********
******************************************************************************/
TXN TXN::newTXN(int code, int buyer, int seller, int time, int buyIdx, 
                        int sellIdx, double amountD){
    TXN txn;
    txn.TXNCode = code;
    txn.user = buyer;
    txn.recipient = seller;
    txn.buyIdx = buyIdx;
    txn.sellIdx = sellIdx;
    txn.amountD = amountD;
    txn.time = time;
    return txn;
}

bool TXN::equalTXNs(TXN txn) {
    return (this->user == txn.user &&
            this->recipient == txn.recipient &&
            this->time == txn.time);
}

bool TXN::validTXN(vector<Customer>& customers) {
    if (this->TXNCode == 0) {
        return true;
    }
    else if (this->TXNCode == 1 && customers[buyIdx].balance > this->amountD) {
        return true;
    }
    else if (this->TXNCode == 2 && customers[buyIdx].balance > this->amountD) {
        return true;
    }
    return false;
}

void TXN::performTXN(vector<Customer>& customers){
    if (this->TXNCode == 0) {
        customers[buyIdx].balance+=this->amountD;
    }
    else if (this->TXNCode == 1 && customers[buyIdx].balance > this->amountD) {
        customers[buyIdx].balance-=this->amountD;
    }
    else if (this->TXNCode == 2 && customers[buyIdx].balance > this->amountD) {
        customers[buyIdx].balance-=this->amountD;
        customers[sellIdx].balance+=this->amountD;
    }
}

void TXN::print(ofstream& Events) {
    Events << fixed << setprecision(2);
    if (this->TXNCode == 0){
        Events << this->time << "," << this->user << "," <<"Deposit,"
                            << this->amountD << "," << endl;
    }
    else if (this->TXNCode == 1){
        Events << this->time << "," << this->user << "," <<"Withdraw,"
                            << this->amountD << "," << endl;
    }
    else if (this->TXNCode == 2){
        Events << this->time << "," << this->user << "," <<"Transfer,"
                            << this->amountD << "," << this->recipient << endl;
    }
}