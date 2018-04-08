#include <cstdio>
#include <string>
#include <utility>
#include <thread>

#include "../Headers/Block.h"
#include "../Headers/Threadpool.h"
#include <iostream>
#include <random>
#include "../Headers/sha256.h"
#include <sstream>
//#include <c++/4.8.3/atomic>

// required for increasing vector size (calls default constructor)
Block::Block() = default;

// Constructor with params
Block::Block(int idx, TransactionData d, string prev_hash, const bool is_genesis_block)
	: index_(idx), is_genesis_block_(is_genesis_block), previous_hash_(std::move(prev_hash)), data(std::move(d))
{
	block_hash_ = generate_hash();
}

// Private Functions
int Block::get_index() const { return index_; }

// Public Functions
string Block::get_hash() const { return block_hash_; }

string Block::get_previous_hash() const { return previous_hash_; }

/*
 Generates hash for current block
 - Includes previousHash in generation
 - ^ Very important
*/
string Block::generate_hash()
{
	// obtain a seed from the system clock:
	const unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	std::mt19937 generator(seed);// mt19937 is a standard mersenne_twister_engine
	rand_hash_number = abs(static_cast<int32_t>(generator())); // generate random 32 bit number
	// generate a range for search
#ifndef DEBUG
	min_number = static_cast<size_t>(rand_hash_number * 0.9999);
	max_number = static_cast<size_t>(rand_hash_number * 1.0001);
#else
	min_number = static_cast<size_t>(rand_hash_number * 0.9995);
	max_number = static_cast<size_t>(rand_hash_number * 1.0005);
#endif
	return try_generate_hash(rand_hash_number);
}

// copy paste from Boost lib
// combines 2 hashes
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

string Block::try_generate_hash(int32_t number) const
{
	// creating string of transaction data
	const std::string toHashS =
		std::to_string(data.amount) + data.receiver_key + data.sender_key + std::to_string(data.timestamp)
		+ std::to_string(number); // important part: include the given number in the attempt

	// 2 hashes to combine
	const std::hash<std::string> tDataHash; // hashes transaction data string
	const std::hash<std::string> prevHash; // re-hashes previous hash (for combination)

	// combine hashes and get size_t for block hash
	//return sha256(tDataHash(toHashS) ^ (prevHash(std::to_string(previousHash)) << 1));
	auto hash1 = tDataHash(toHashS);
	const auto hash2 = prevHash(previous_hash_) << 1;
	hash_combine(hash1, hash2);
	stringstream ss;
	ss << hash1;
	return sha256(ss.str());

}

bool Block::is_hash_valid() { return generate_hash() == get_hash(); }
