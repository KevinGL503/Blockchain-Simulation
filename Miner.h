#include <string>
#include <iostream>
#include "Blockchain.h"
#include <tuple>
using namespace std;

class Miner {
    public:
        int id; 
        int PoET;
        int blockCount;
        string notes;
        Blockchain localBlockchain;

        Miner();
        void initMiner(int);
        void setPoET(Miner*, int, int);
        bool PoETDue (Miner*, int,int);
        bool activePoET(Miner*, int);
        int getMiner(Miner*,int,int);
        void addPendingBlock(Block *);
        void appendBlock(Blockchain&);
        void deleteDuplicate(Block);
        Block* firstBlock();
        void JoinRemoveMiners(vector<Miner>&, tuple<bool,int,int>&, ofstream&, 
                                tuple<int,int>&);
};