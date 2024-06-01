#include <string>
#include <iostream>
#include <iomanip> 
#include "Miner.h"
using namespace std;

Miner::Miner() {
}

void Miner:: initMiner(int id){
    this->id = id; 
    PoET = 0;
    blockCount = 0;
}

void Miner::addPendingBlock(Block* newBlock) {
    Block *block = new Block;
    block->TXNsBlock = newBlock->TXNsBlock;
    this->localBlockchain.addBlock(block);
}

void Miner::appendBlock(Blockchain& chain) {
    Block* newBlock = new Block;
    newBlock->TXNsBlock = this->localBlockchain.genesis->TXNsBlock;
    this->localBlockchain.deletePrevBlock();
    newBlock->next=nullptr;
    newBlock->minerID = this->id;
    this->blockCount++;
    chain.addBlock(newBlock);
}

Block* Miner::firstBlock(){
    return this->localBlockchain.genesis;
}

void Miner::deleteDuplicate(Block newBlock){
    Block *mp = this->firstBlock();
    Block *temp;
    bool flag = 0;
    while (mp || !flag){
        if (newBlock.equalBlocks(mp)){
            temp = this->firstBlock();
            while (!mp->equalBlocks(temp)) {
                this->localBlockchain.deletePrevBlock();
            }
            if (mp->equalBlocks(temp)){
                this->localBlockchain.deletePrevBlock();
                flag = 1;
            }
        }
        mp= mp->next;
    }
    
}

void Miner::setPoET(Miner* miners, int tic, int size) {
    int delay;
    for (int i = 0; i < size; i++) {
        delay = (rand() % size + 1) + 2;
        miners[i].PoET = tic + delay;
        while (delay + tic != miners[i].PoET) {
            delay = (rand() % size + 1) + 2;
        }
    }
}

bool Miner::PoETDue(Miner* miners, int tic, int size) {
    for (int i = 0; i < size; i++){
        if (miners[i].PoET == tic && miners[i].PoET != 0){
            return true;
        }
    }
    return false;
}

bool Miner::activePoET(Miner* miners, int size) {
    for (int i = 0; i < size; i++){
        if (miners[i].PoET > 0){
            return true;
        }
    }
    return false;
}

int Miner::getMiner(Miner* miners, int tic, int size) {
    int miner = 0;
    for (int i = 0; i < size; i++){
        if (miners[i].PoET == tic){
            miner = i;
        }
        //reset PoET
        miners[i].PoET = 0;
    }
    return miner;
}


void Miner::JoinRemoveMiners(vector<Miner>& miners, tuple<bool, int,int>& event, 
                            ofstream& minerEvents, tuple<int,int>& totalEvents){
    if (get<1>(event) == 1) {
                Miner newMiner;
                newMiner.initMiner(int(rand()));
                newMiner.localBlockchain = miners[0].localBlockchain;
                miners.push_back(newMiner);
                minerEvents << get<2>(event) << "," << "New Miner: " << 
                                    newMiner.id << " joined" << endl;
                get<0>(totalEvents)++;
            }
            else if (miners.size() > 1){
                int indexRemove = 1+(rand() % (miners.size()-1 - 1 + 1));
                minerEvents << get<2>(event) << "," << "Miner " << 
                        miners[indexRemove].id << ": Left" << endl;
                        get<1>(totalEvents)++;
                vector<Miner>::iterator itr= miners.begin();
                    while( itr != miners.end()) {
                        if((*itr).id == miners[indexRemove].id){
                            miners.erase(itr);
                            break;
                        }
                        ++itr;
                    }
            }
}