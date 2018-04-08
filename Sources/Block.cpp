#include <cstdio>
#include <string>
#include <utility>
#include <thread>

#include "../Headers/Block.h"
#include "../Headers/Threadpool.h"
#include <iostream>
#include <random>
#include <c++/4.8.3/atomic>

// required for increasing vector size (calls default constructor)
Block::Block() = default;

// Constructor with params
Block::Block(int idx, TransactionData d, size_t prevHash, bool is_genesis_block) : index(idx), data(std::move(d)), previousHash(prevHash), is_genesis_block(is_genesis_block)
{
	blockHash = generateHash();

	// We don't want proof of work on the genesis block
	if (!is_genesis_block) {
		std::cout << "Starting proof of work for block " << idx << std::endl;
		const bool success = proofOfWork(minNumber, maxNumber);
		std::cout << "Proof of work " << (success ? "succeeded" : "failed") << " for block " << idx << std::endl;
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
	// obtain a seed from the system clock:
	unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	std::mt19937 generator(seed);// mt19937 is a standard mersenne_twister_engine
	randHashNumber = static_cast<int32_t>(generator()); // generate random 32 bit number
    randHashNumber = abs(randHashNumber);
    std::cout << "Random hash number: " << randHashNumber << std::endl;
	// TODO make more random? 20% now
	minNumber = static_cast<size_t>(randHashNumber * 0.9995);
	std::cout << "Min number: " << minNumber << std::endl;
	maxNumber = static_cast<size_t>(randHashNumber * 1.0005);
	std::cout << "Max number: " <<  maxNumber << std::endl;
	return tryGenerateHash(randHashNumber);
}

size_t Block::tryGenerateHash(int32_t number)
{
	// creating string of transaction data
	std::string toHashS = std::to_string(data.amount) + data.receiverKey + data.senderKey + std::to_string(data.timestamp) + std::to_string(number);

	// 2 hashes to combine
	std::hash<std::string> tDataHash; // hashes transaction data string
	std::hash<std::string> prevHash; // re-hashes previous hash (for combination)

	// combine hashes and get size_t for block hash
	return tDataHash(toHashS) ^ (prevHash(std::to_string(previousHash)) << 1);
}

bool Block::isHashValid() { return generateHash() == getHash(); }

bool Block::proofOfWork(size_t min, size_t max)
{
	// TODO make some start at min, some at max (maximize efficiency!)

    ThreadPool thread_pool{ 100 };
    std::mutex m;

	// try getting the correct hash
	size_t foundHash = 0;
	size_t n = min;

        // simulate trying to get the correct hash
    while (foundHash != blockHash && n <= max)
    {
        thread_pool.enqueue([&] {

        m.lock();
        foundHash = tryGenerateHash(n);


        std::cout << "Trying to solve from thread: " << std::this_thread::get_id() << std::endl;
        std::cout << "Block hash: " << blockHash << std::endl;
        std::cout << "Found hash: " << foundHash << std::endl;
        m.unlock();

        if (foundHash == blockHash)
            return true; // found it!
        });

        // try again
        n++;
    }

	// not found
	return false;
}

