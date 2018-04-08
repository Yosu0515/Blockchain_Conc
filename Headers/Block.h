#pragma once

#include "TransactionData.h"

class Block
{
private:
	int index;
	bool is_genesis_block;
	std::string blockHash;
	std::string previousHash;
	std::string generateHash();

public:

	std::string tryGenerateHash(int32_t number);
	int32_t randHashNumber;
	size_t minNumber;
	size_t maxNumber;

	Block();
	// Constructor
	Block(int idx, TransactionData d, std::string prevHash, bool is_genesis_block);

	// Get Index
	int getIndex();

	// Get Original Hash
	std::string getHash();

	// Get Previous Hash
	std::string getPreviousHash();

	// Transaction Data
	// Would ordinarily be a private member with a "getter": getData()
	TransactionData data;

	// Validate Hash
	bool isHashValid();
};

