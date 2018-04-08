#include <cstdio>
#include <string>
#include <utility>
#include <thread>

#include "../Headers/Block.h"
#include "../Headers/Threadpool.h"
#include <iostream>
#include <random>
//#include <c++/4.8.3/atomic>

// required for increasing vector size (calls default constructor)
Block::Block() = default;

// Constructor with params
Block::Block(int idx, TransactionData d, size_t prevHash, bool is_genesis_block) : index(idx), data(std::move(d)), previousHash(prevHash), is_genesis_block(is_genesis_block)
{
	blockHash = generateHash();
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
	// obtain a seed from the system clock:
	const unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	std::mt19937 generator(seed);// mt19937 is a standard mersenne_twister_engine
	randHashNumber = static_cast<int32_t>(generator()); // generate random 32 bit number
	randHashNumber = abs(randHashNumber);
	minNumber = static_cast<size_t>(randHashNumber * 0.9995);
	maxNumber = static_cast<size_t>(randHashNumber * 1.0005);
	return tryGenerateHash(randHashNumber);
}

size_t Block::tryGenerateHash(int32_t number)
{
	// creating string of transaction data
	std::string toHashS =
		std::to_string(data.amount) + data.receiverKey + data.senderKey + std::to_string(data.timestamp)
		+ std::to_string(number); // important part: include the given number in the attempt

	// 2 hashes to combine
	std::hash<std::string> tDataHash; // hashes transaction data string
	std::hash<std::string> prevHash; // re-hashes previous hash (for combination)

	// combine hashes and get size_t for block hash
	return tDataHash(toHashS) ^ (prevHash(std::to_string(previousHash)) << 1);
}

bool Block::isHashValid() { return generateHash() == getHash(); }
