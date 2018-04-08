#include <cstdio>
#include <ctime>
#include <string>
#include <utility>

#include "../Headers/Block.h"
#include "../Headers/Blockchain.h"

#define btoa(x) ((x)?"true":"false")

// Blockchain Constructor
Blockchain::Blockchain()
{
	Block genesis = createGenesisBlock();
	chain_.push_back(genesis);
}

// Public Chain Getter
std::vector<Block> Blockchain::get_chain() {
	return chain_;
}

// Create Genesis Block
Block Blockchain::createGenesisBlock()
{
	// Get Current Time
	std::time_t current;

	// Setup Initial Transaction Data
	TransactionData d(0, "Genesis", "Genesis", time(&current));

	// Return Genesis Block
	Block genesis(0, d, "0", true);
	return genesis;
}

// We only need pointer here
// to demonstrate manipulation of transaction data
Block *Blockchain::get_latest_block()
{
	return &chain_.back();
}

void Blockchain::add_block(TransactionData d)
{
	const int i = chain_.size();

	// add a new space for the next block
	chain_.resize(i + 1);

	// get the previous hash
	const std::string previousHash = i > 0
		? chain_[i - 1].get_hash()
		: 0;

	// now make the new block
	Block newBlock(i, std::move(d), previousHash, false);

	// now place it at the predefined location
	chain_.at(i) = newBlock;
}

bool Blockchain::is_chain_valid()
{
	for (auto& block : chain_)
	{
		if (!block.is_hash_valid())
			return false;

		// Don't forget to check if this is the first item
		if (block.get_index() > 0)
		{
			Block previousBlock = chain_.at(block.get_index() - 1);
			if (block.get_previous_hash() != previousBlock.get_hash())
				return false;
		}
	}

	return true;
}

void Blockchain::print_chain() {
	for (auto& block : chain_)
	{
		printf("\n\nBlock ===================================");
		printf("\nIndex: %d", block.get_index());
		printf("\nAmount: %f", block.data.amount);
		printf("\nSenderKey: %s", block.data.sender_key.c_str());
		printf("\nReceiverKey: %s", block.data.receiver_key.c_str());
		printf("\nTimestamp: %ld", block.data.timestamp);
		printf("\nHash: %zu", block.get_hash());
		printf("\nPrevious Hash: %zu", block.get_previous_hash());
		printf("\nIs Block Valid?: %s", btoa(block.is_hash_valid()));
		printf("\nIs Chain Valid?: %s", btoa(is_chain_valid()));
	}
}