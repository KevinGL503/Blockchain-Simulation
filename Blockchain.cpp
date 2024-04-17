#include <iostream>
#include <string>
#include "Blockchain.h"
#include <fstream>
#include <time.h>
#include "sha256.h"
using namespace std;

/******************************************************************************
    ****** Block/Blockchain class functions declarations********
******************************************************************************/
bool Block:: fullBlock(){
    return (this->TXNsBlock.size() == 6);
}

TXN Block::getFirstTXN() {
    TXN curr;
    if (TXNsBlock.size() != 0){
        curr = TXNsBlock.at(0);
    }
    return curr;
}

bool Block::equalBlocks(Block* block){
    if (block == NULL) {
        return false;
    }
    TXN thisOne = this->getFirstTXN();
    TXN otherOne = block->getFirstTXN();
    return (thisOne.equalTXNs(otherOne));
}

Blockchain:: Blockchain() {
    genesis = nullptr;
    front = genesis;
    blockCount = 0;
}

void Blockchain:: deletePrevBlock() {
    Block *p;
    if (genesis != NULL) {
        p = genesis;
        genesis = genesis->next;
        delete p;
        this->blockCount--;
    }
    return;
}

void Blockchain::addBlock(Block* block){
    this->blockCount++;
    block->blockID = this->blockCount;
    if (genesis == nullptr) {  //if genesis no need for hash pointer
        block->prevHashPointer = "0000000000000000000000000000000000000000000000000000000000000000";
        genesis = block;
        block->next = nullptr;
        front = block;
        return;
    }

    //calculate hash pointer
    string HashData = "";
    for (auto curr: this->front->TXNsBlock){
        HashData += ("TXN Code" + to_string(curr.TXNCode));
        HashData+= ("user" + to_string(curr.user));
        HashData+= to_string(curr.time);
        HashData+= to_string(curr.amountD);
    }
    // assign hash
    string hash = sha256(HashData);
    block->prevHashPointer = hash;

    front->next = block;
    block->next = nullptr;
    front = block;
    return;
}

void Blockchain::printFileData(ofstream& BlockchainFile){
    Block *temp = genesis;
    while (temp != nullptr){
        BlockchainFile << temp->blockID << "," << temp->prevHashPointer;
        BlockchainFile << endl;
        temp = temp->next;
    }
}

void Blockchain::printData(){
    Block *temp = genesis;
    while (temp != nullptr){
        cout << "Block ID: " << temp->blockID << endl;
        cout << temp->prevHashPointer << endl;
        temp = temp->next;
    }
}