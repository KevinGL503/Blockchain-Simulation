#include <iostream> 
#include <iomanip> 
#include <stdlib.h> 
#include <fstream> 
#include <vector>
#include "Miner.h"
#include "sha256.h"
#include <tuple>
using namespace std; 

int main(int argc, char *argv[]) { 
    /****** Set srand, number of customers & miners, and tics ******/
    cout << argv[2] << endl;
    srand(atoi(argv[2])); 
    int NUMCUSTOMERS = 5;
    int NUMMINERS = 1;
    int NUMTICS = 0; 
    double INITDS = 10000;
    int totalRequests = 0;

    /******Create lists for customers, miners, events, and blockchain *******/
    vector<Customer> customers; 
    vector<Miner> miners;
    TXNQueue events; 

    //Events CSV File
    ofstream EventsFile;
    ofstream TXNsFile;
    ofstream BlockchainFile;
    ofstream minerEvents;

    //Variables to be used
    tuple<int,int>totalCustEvents = tuple<int,int>{0,0};
    tuple<int, int> totalMinerEvents = tuple<int,int>{0,0}; 
 
    //initial status for customers 
    for (int i = 0; i < NUMCUSTOMERS; i++) { 
        Customer newCustomer;
        newCustomer.initCustomer(int(rand()),INITDS);
        customers.push_back(newCustomer); 
    }
    // initial status for miners
    for (int i = 0; i < NUMMINERS; i++) { 
        Miner newMiner;
        newMiner.initMiner(int(rand()));
        miners.push_back(newMiner);
    }


    // Generate TXNs and events given a number of total tics
    EventsFile.open ("./csvFiles/TXNsRequests.csv");
    EventsFile << "Tic, Customer ID, TXN, Amount($), Receiver ID" << endl;
    //Generate initial 200 events
    events.generateInitialEvents(NUMTICS, EventsFile, customers, totalRequests, 
                        totalCustEvents);
    // EventsFile.close();
    
    //variables to be used for block/blockchain creations and data stats
    TXN currTXN;
    Block *newBlock = new Block;
    tuple<bool,int,int> minerJoinLeft;
    int currentTic = 0;
    int performedTXNs = 0;
    int newEvents = 0;
    int beforeTXNs; 
    
    //File for miner events
    minerEvents.open ("./csvFiles/minerEvents.csv");
    minerEvents << "Tic, Event" << endl;

    // file for sucessful transactions
    TXNsFile.open ("./csvFiles/TXNsSuccess.csv");
    TXNsFile << "Tic, Customer ID, TXN, Amount($), Receiver ID" << endl;
    
    while (!events.emptyQueue() && performedTXNs < 3000) {
        
        //Check if any miners join or left
        minerJoinLeft = events.minersJoinRemove.front();
        events.minersJoinRemove.pop();
        if (get<0>(minerJoinLeft) == true) {
            miners[0].JoinRemoveMiners(miners,minerJoinLeft,minerEvents,
                                        totalMinerEvents);
        }

        //Transactions and Block Creation
        while (events.transactions.front().time == currentTic){
            currTXN = events.transactions.front();
            events.transactions.pop();
            newBlock->TXNsBlock.push_back(currTXN);
            performedTXNs++;
            currTXN.print(TXNsFile);
            beforeTXNs = events.transactions.size();
            //Generate new events
            events.generateNewEvents(NUMTICS, EventsFile, customers, 
                                    totalRequests, totalCustEvents);
            newEvents+=(events.transactions.size() - beforeTXNs);

            //Block creation when 6 valid transanctions
            if (newBlock->fullBlock()){
                for (int i = 0; i < miners.size(); i++) {
                    //addBlock() calculates the hash and appends block to local
                    miners[i].localBlockchain.addBlock(newBlock); 
                }
                newBlock = new Block;
            }

            if (events.emptyQueue()){
                break;
            }
        }
        currentTic++;
    }
    minerEvents.close();
    TXNsFile.close();

    //print local blockchain to file
    BlockchainFile.open ("./csvFiles/Blockchain.csv");
    BlockchainFile << "Block ID, Previous Hash" << endl;
    miners[0].localBlockchain.printFileData(BlockchainFile);
    BlockchainFile.close();

    //Print Stats
    cout << "Total Transaction Requests: " << totalRequests << endl;
    cout << "Sucessful/Performed Transactions: " << performedTXNs << endl;
    cout << "Average number of Transaction attempts: ";
    cout << totalRequests/double(NUMTICS) << " per tic" << endl;
    cout << "Average number of Sucessful Transactions: ";
    cout << performedTXNs/double(NUMTICS) << " per tic" << endl;
    cout << endl;
    cout << "Number of events created after initial queue: " <<newEvents<< endl;
    cout << endl;
    cout << "Number of Blocks: " << miners[0].localBlockchain.blockCount << endl;
    cout << endl;
    cout << "Initial Number of Miners: 1" << endl;
    cout << "Number of Miners Joined: "<< get<0>(totalMinerEvents) << endl;
    cout << "Number of Miners Removed: "<< get<1>(totalMinerEvents) << endl;
    cout << "Final Number of Miners: " << miners.size() << endl;
    cout << endl;
    cout << "Initial Number of Customers: 4" << endl;
    cout << "Number of Customers Joined: "<< get<0>(totalCustEvents) << endl;
    cout << "Number of Miners Removed: "<< get<1>(totalCustEvents) << endl;
    cout << "Final Number of Customers: " << customers.size() << endl;
    cout << endl;
    cout << "See csv files for transanction, customer and miner events" 
            << " and blockchain details." << endl;
    return 0;
}