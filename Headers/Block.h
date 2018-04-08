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

	size_t tryGenerateHash(int32_t number);

	int32_t randHashNumber;
	size_t minNumber;
	size_t maxNumber;

public:
	Block();
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
	bool proofOfWork(size_t min, size_t max);
};

