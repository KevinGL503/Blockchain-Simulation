#include <string>
#include <iostream>
#include <iomanip> 
#include <fstream>
#include "EventsQueue.h"
using namespace std;

/*** Block/Blockchain class ***/
class Block {
    public:
        int blockID;
        int minerID;
        int TXNCount;
        vector<TXN> TXNsBlock;
        Block *next;
        string prevHashPointer;

        bool fullBlock();
        TXN getFirstTXN();
        bool equalBlocks(Block*);
};
class Blockchain {
    public:
        int blockCount;
        Block* genesis;
        Block* front;
        
        Blockchain();
        void createBlock(int);
        void addBlock(Block*);
        void deletePrevBlock(); 
        void printFileData(ofstream&);
        void printData();
};