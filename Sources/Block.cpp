#include <cstdio>
#include <string>
#include <utility>
#include <thread>

#include "../Headers/Block.h"
#include <iostream>

// required for increasing vector size (calls default constructor)
Block::Block() = default;

// Constructor with params
Block::Block(int idx, TransactionData d, size_t prevHash, bool is_genesis_block) : index(idx), data(std::move(d)), previousHash(prevHash), is_genesis_block(is_genesis_block)
{
	blockHash = generateHash();

	// We don't want proof of work on the genesis block
	if (!is_genesis_block) {
		std::cout << "Starting proof of work for block " << idx << std::endl;
		proofOfWork();
		std::cout << "Proof of work done for block " << idx << std::endl;
	}
}

// Private Functions
int Block::getIndex() { return index; }

// Public Functions
size_t Block::getHash() { return blockHash; }

size_t Block::getPreviousHash() { return previousHash; }

/*
 Generates hash for current block
 - Includes previousHash in generation
 - ^ Very important
*/
size_t Block::generateHash()
{
	// creating string of transaction data
	std::string toHashS = std::to_string(data.amount) + data.receiverKey + data.senderKey + std::to_string(data.timestamp);

	// 2 hashes to combine
	std::hash<std::string> tDataHash; // hashes transaction data string
	std::hash<std::string> prevHash; // re-hashes previous hash (for combination)

	// combine hashes and get size_t for block hash
	return tDataHash(toHashS) ^ (prevHash(std::to_string(previousHash)) << 1);
}

bool Block::isHashValid() { return generateHash() == getHash(); }

void Block::proofOfWork() {

	// ??? wtf is dit?
	std::this_thread::sleep_for(std::chrono::seconds(10));
}

