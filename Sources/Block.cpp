#include <cstdio>
#include <string>
#include <utility>
#include <thread>

#include "../Headers/Block.h"
#include "../Headers/Threadpool.h"
#include <iostream>
#include <random>
#include "../sha256.h"
#include <sstream>
//#include <c++/4.8.3/atomic>

// required for increasing vector size (calls default constructor)
Block::Block() = default;

// Constructor with params
Block::Block(int idx, TransactionData d, string prevHash, bool is_genesis_block) : index(idx), data(std::move(d)), previousHash(prevHash), is_genesis_block(is_genesis_block)
{
	blockHash = generateHash();
}

// Private Functions
int Block::getIndex() { return index; }

// Public Functions
string Block::getHash() { return blockHash; }

string Block::getPreviousHash() { return previousHash; }

/*
 Generates hash for current block
 - Includes previousHash in generation
 - ^ Very important
*/
string Block::generateHash()
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

// copy paste from Boost lib
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

string Block::tryGenerateHash(int32_t number)
{
	// creating string of transaction data
	const std::string toHashS =
		std::to_string(data.amount) + data.receiverKey + data.senderKey + std::to_string(data.timestamp)
		+ std::to_string(number); // important part: include the given number in the attempt

	// 2 hashes to combine
	const std::hash<std::string> tDataHash; // hashes transaction data string
	const std::hash<std::string> prevHash; // re-hashes previous hash (for combination)

	// combine hashes and get size_t for block hash
	//return sha256(tDataHash(toHashS) ^ (prevHash(std::to_string(previousHash)) << 1));
	auto hash1 = tDataHash(toHashS);
	const auto hash2 = prevHash(previousHash) << 1;
	hash_combine(hash1, hash2);
	stringstream ss;
	ss << hash1;
	return sha256(ss.str());

}

bool Block::isHashValid() { return generateHash() == getHash(); }
