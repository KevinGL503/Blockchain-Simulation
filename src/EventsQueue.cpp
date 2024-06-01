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

/******************************************************************************
    ****Transaction Queue class functions declarations ********
******************************************************************************/

TXNQueue::TXNQueue(){
    sucessTXNs = 0;
}

void TXNQueue::addTXN(int code, int buyer, int seller, int time, int buyIdx, 
                        int sellIdx, double amountD){
    TXN txn;
    txn.TXNCode = code;
    txn.user = buyer;
    txn.recipient = seller;
    txn.buyIdx = buyIdx;
    txn.sellIdx = sellIdx;
    txn.amountD = amountD;
    txn.time = time;
    this->transactions.push(txn);
}

void TXNQueue::deleteTXN(){
    if (!this->transactions.empty()) {
        this->transactions.pop();
    }
    return;
}

TXN TXNQueue::getNextTXN(){
        return this->transactions.front();
}

//Generate the transaction queue given total tics and a list of customers
void TXNQueue::generateInitialEvents(int& tic, ofstream& Events,
                            vector<Customer> &customers, int& totalRequests, 
                            tuple<int,int>& custEvents){

    //variables to be used
    ofstream customerEvents;
    customerEvents.open ("./csvFiles/customerEvents.csv");
    customerEvents << "Tic, Event" << endl;

    int seller; 
    int availaCustomers;
    double dollars = 0;
    TXN newEvent;
    while (this->transactions.size() < 200) {
        // Probability of adding/removing at thi tic
        // tuple: <join/remove prob, code for event>
            // codes: 1:join customer, 2:join miner, 3: remove customer, 
            //  4: remove miner
        bool addRemove = (((double)rand() / (double) RAND_MAX) < .05);
        if (addRemove){
            //choose remove or join
            int finalEvent = rand() % 2;

            if (finalEvent == 0) { //choose join
                int minerOrCustomer = rand() % 2;
                if (minerOrCustomer == 0){
                    Customer newCustomer;
                    newCustomer.initCustomer(int(rand()),10000);
                    customers.push_back(newCustomer); 
                    this->minersJoinRemove.push(tuple<bool, int, int>{0, 0, tic});
                    customerEvents << tic << "," << "New Customer: " << 
                                    newCustomer.id << " joined" << endl;
                    get<0>(custEvents)++;
                }
                else {
                    this->minersJoinRemove.push(tuple<bool,int, int>{1,1,tic});
                }
            }
            else { //choose remove
            int minerOrCustomer = rand() % 2;
                if (minerOrCustomer == 0 && customers.size() > 4){
                    int indexRemove = 4+(rand() % (customers.size()-1 - 4 + 1));                  
                    //Remove customer
                    vector<Customer>::iterator itr= customers.begin();
                    customerEvents << tic << "," << "Customer " << 
                        customers[indexRemove].id << ": Left" << endl;
                    get<1>(custEvents)++;
                    while( itr != customers.end()) {
                        if((*itr).id == customers[indexRemove].id){
                            customers.erase(itr);
                            break;
                        }
                        ++itr;
                    }
                    this->minersJoinRemove.push(tuple<bool, int,int>{0, 0, tic});
                }
                else if (minerOrCustomer == 1){
                    this->minersJoinRemove.push(tuple<bool,int,int>{1,2,tic});; 
                }
                else{
                    this->minersJoinRemove.push(tuple<bool, int,int>{0, 0,tic});
                }
            }
        }
        else {
            this->minersJoinRemove.push(tuple<bool, int,int>{0, 0,tic});
        }


        //Get probability of transaction in this tic for each customer
        for (int i = 0; i < customers.size(); i++) {
            customers[i].probTXN = (((double)rand() / (double) RAND_MAX));
        }

        availaCustomers = customers.size();
        //Add a TXN to queue if customer will perform one and has not been part
        // of another one and there is still sellers left
        for (int user = 0; user < customers.size(); user++){
            if (!customers[user].completedTXN) {
                /*** Deposit Transaction ****/
                if (customers[user].probTXN < .1){
                    dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                    //Create Txn Request
                    newEvent = newEvent.newTXN(0,customers[user].id,0,tic,
                                                user,0, dollars);
                    // if valid add to transactions
                    if (newEvent.validTXN(customers)) { 
                        this->transactions.push(newEvent);
                        newEvent.performTXN(customers);
                        sucessTXNs++;
                    }
                    //Record to txns events file
                    Events << tic<< "," <<customers[user].id<< "," <<"Deposit,"
                            << dollars << "," << endl;
                    totalRequests++;
                    // set flags
                    availaCustomers-=1;
                    customers[user].completedTXN = 1;
                }
                /*** Withdraw Transaction ****/
                else if (customers[user].probTXN > .1 && 
                         customers[user].probTXN < .2){
                    dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                    //Create Txn Request
                    newEvent = newEvent.newTXN(1,customers[user].id,0,tic,
                                                user,0, dollars);
                    // if valid add to transactions
                    if (newEvent.validTXN(customers)) { 
                        this->transactions.push(newEvent);
                        newEvent.performTXN(customers);
                        sucessTXNs++;
                    }
                     //Record to txns events file
                    Events << tic<< "," <<customers[user].id<< "," <<"Withdraw,"
                            << dollars << "," << endl;
                    totalRequests++;
                    //Set flags
                    availaCustomers-=1;
                    customers[user].completedTXN = 1;
                }
                /*** Transfer Transaction ****/
                else if (customers[user].probTXN > .2 && 
                         customers[user].probTXN < .3 && availaCustomers > 1) {
                    seller = rand() % (customers.size()); 
                    while (seller == user || customers[seller].completedTXN) {
                        seller = rand() % customers.size();
                    }
                    dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                    //Create Txn Request
                    newEvent = newEvent.newTXN(2,customers[user].id,
                                customers[seller].id,tic,user,seller, dollars);
                    //if valid add to transanctions
                    if (newEvent.validTXN(customers)) { 
                        this->transactions.push(newEvent);
                        newEvent.performTXN(customers);
                        sucessTXNs++;
                    }
                    //Record to txns events file
                    Events << tic<< "," <<customers[user].id<< "," <<"Transfer,"
                            << dollars << "," << customers[seller].id << endl;
                    totalRequests++;
                    //set Flags
                    availaCustomers-=2;
                    customers[user].completedTXN = 1;
                    customers[seller].completedTXN = 1;
                }
            }
        }
        //Reset complete TXN flag for next tic
        for (int i = 0; i < customers.size(); i++ ) {
            customers[i].completedTXN = 0;
        }

        tic++;
    }

    customerEvents.close();
    return;
}

void TXNQueue::generateNewEvents(int& tic, ofstream& Events,
                            vector<Customer> &customers, int& totalRequests, 
                            tuple<int,int>& custEvents){

    //variables to be used
    ofstream customerEvents;
    customerEvents.open ("./csvFiles/customerEvents.csv", ios_base::app);

    int seller; 
    int availaCustomers;
    double dollars = 0;
    TXN newEvent;
    // Probability of adding/removing at thi tic
    // tuple: <join/remove prob, code for event>
        // codes: 1:join customer, 2:join miner, 3: remove customer, 
        //  4: remove miner
    bool addRemove = (((double)rand() / (double) RAND_MAX) < .05);
    if (addRemove){
        //choose remove or join
        int finalEvent = rand() % 2;

        if (finalEvent == 0) { //choose join
            int minerOrCustomer = rand() % 2;
            if (minerOrCustomer == 0){
                Customer newCustomer;
                newCustomer.initCustomer(int(rand()),10000);
                customers.push_back(newCustomer); 
                this->minersJoinRemove.push(tuple<bool, int, int>{0, 0, tic});
                customerEvents << tic << "," << "New Customer: " << 
                                newCustomer.id << " joined" << endl;
                get<0>(custEvents)++;
            }
            else {
                this->minersJoinRemove.push(tuple<bool,int, int>{1,1,tic});
            }
        }
        else { //choose remove
        int minerOrCustomer = rand() % 2;
            if (minerOrCustomer == 0 && customers.size() > 4){
                int indexRemove = 4+(rand() % (customers.size()-1 - 4 + 1));                  
                //Remove customer
                vector<Customer>::iterator itr= customers.begin();
                customerEvents << tic << "," << "Customer " << 
                    customers[indexRemove].id << ": Left" << endl;
                get<1>(custEvents)++;
                while( itr != customers.end()) {
                    if((*itr).id == customers[indexRemove].id){
                        customers.erase(itr);
                        break;
                    }
                    ++itr;
                }
                this->minersJoinRemove.push(tuple<bool, int,int>{0, 0, tic});
            }
            else if (minerOrCustomer == 1){
                this->minersJoinRemove.push(tuple<bool,int,int>{1,2,tic});; 
            }
            else{
                this->minersJoinRemove.push(tuple<bool, int,int>{0, 0,tic});
            }
        }
    }
    else {
        this->minersJoinRemove.push(tuple<bool, int,int>{0, 0,tic});
    }


    //Get probability of transaction in this tic for each customer
    for (int i = 0; i < customers.size(); i++) {
        customers[i].probTXN = (((double)rand() / (double) RAND_MAX));
    }

    availaCustomers = customers.size();
    //Add a TXN to queue if customer will perform one and has not been part
    // of another one and there is still sellers left
    for (int user = 0; user < customers.size(); user++){
        if (!customers[user].completedTXN) {
            /*** Deposit Transaction ****/
            if (customers[user].probTXN < .1){
                dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                //Create Txn Request
                newEvent = newEvent.newTXN(0,customers[user].id,0,tic,
                                            user,0, dollars);
                // if valid add to transactions
                if (newEvent.validTXN(customers)) { 
                    this->transactions.push(newEvent);
                    newEvent.performTXN(customers);
                    sucessTXNs++;
                }
                //Record to txns events file
                Events << tic<< "," <<customers[user].id<< "," <<"Deposit,"
                        << dollars << "," << endl;
                totalRequests++;
                // set flags
                availaCustomers-=1;
                customers[user].completedTXN = 1;
            }
            /*** Withdraw Transaction ****/
            else if (customers[user].probTXN > .1 && 
                        customers[user].probTXN < .2){
                dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                //Create Txn Request
                newEvent = newEvent.newTXN(1,customers[user].id,0,tic,
                                            user,0, dollars);
                // if valid add to transactions
                if (newEvent.validTXN(customers)) { 
                    this->transactions.push(newEvent);
                    newEvent.performTXN(customers);
                    sucessTXNs++;
                }
                    //Record to txns events file
                Events << tic<< "," <<customers[user].id<< "," <<"Withdraw,"
                        << dollars << "," << endl;
                totalRequests++;
                //Set flags
                availaCustomers-=1;
                customers[user].completedTXN = 1;
            }
            /*** Transfer Transaction ****/
            else if (customers[user].probTXN > .2 && 
                        customers[user].probTXN < .3 && availaCustomers > 1) {
                seller = rand() % (customers.size()); 
                while (seller == user || customers[seller].completedTXN) {
                    seller = rand() % customers.size();
                }
                dollars = ((double) rand() / (double) RAND_MAX ) * 1000;
                //Create Txn Request
                newEvent = newEvent.newTXN(2,customers[user].id,
                            customers[seller].id,tic,user,seller, dollars);
                //if valid add to transanctions
                if (newEvent.validTXN(customers)) { 
                    this->transactions.push(newEvent);
                    newEvent.performTXN(customers);
                    sucessTXNs++;
                }
                //Record to txns events file
                Events << tic<< "," <<customers[user].id<< "," <<"Transfer,"
                        << dollars << "," << customers[seller].id << endl;
                totalRequests++;
                //set Flags
                availaCustomers-=2;
                customers[user].completedTXN = 1;
                customers[seller].completedTXN = 1;
            }
        }
    }
    //Reset complete TXN flag for next tic
    for (int i = 0; i < customers.size(); i++ ) {
        customers[i].completedTXN = 0;
    }

    tic++;

    customerEvents.close();
    return;
}

bool TXNQueue::emptyQueue(){
    return this->transactions.empty();
}