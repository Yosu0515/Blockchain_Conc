#pragma once

#include "TransactionData.h"

class Block
{
private:
    int index;
    bool is_genesis_block;
    size_t blockHash;
    size_t previousHash;
    size_t generateHash();

public:
    // Constructor
    Block(int idx, TransactionData d, size_t prevHash, bool is_genesis_block);

    // Get Index
    int getIndex();

    // Get Original Hash
    size_t getHash();

    // Get Previous Hash
    size_t getPreviousHash();

    // Transaction Data
    // Would ordinarily be a private member with a "getter": getData()
    TransactionData data;

    // Validate Hash
    bool isHashValid();

    // Proof of Work
    void proofOfWork();
};

