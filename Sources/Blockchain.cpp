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
	chain.push_back(genesis);
}

// Public Chain Getter
std::vector<Block> Blockchain::getChain() {
	return chain;
}

// Create Genesis Block
Block Blockchain::createGenesisBlock()
{
	// Get Current Time
	std::time_t current;

	// Setup Initial Transaction Data
	TransactionData d(0, "Genesis", "Genesis", time(&current));

	// Return Genesis Block
	Block genesis(0, d, 0, true);
	return genesis;
}

// We only need pointer here
// to demonstrate manipulation of transaction data
Block *Blockchain::getLatestBlock()
{
	return &chain.back();
}

void Blockchain::addBlock(TransactionData d)
{
	const int i = chain.size();

	// add a new space for the next block
	chain.resize(i);

	// get the previous hash
	const std::size_t previousHash = i > 0
		? getLatestBlock()->getHash()
		: 0;

	// now make the new block
	Block newBlock(i, std::move(d), previousHash, false);

	// now place it at the predefined location
	chain.emplace(chain.begin() + i, newBlock);
}

bool Blockchain::isChainValid()
{
	for (auto& block : chain)
	{
		if (!block.isHashValid())
			return false;

		// Don't forget to check if this is the first item
		if (block.getIndex() > 0)
		{
			Block previousBlock = chain.at(block.getIndex() - 1);
			if (block.getPreviousHash() != previousBlock.getHash())
				return false;
		}
	}

	return true;
}

void Blockchain::printChain() {
	for (auto& block : chain)
	{
		printf("\n\nBlock ===================================");
		printf("\nIndex: %d", block.getIndex());
		printf("\nAmount: %f", block.data.amount);
		printf("\nSenderKey: %s", block.data.senderKey.c_str());
		printf("\nReceiverKey: %s", block.data.receiverKey.c_str());
		printf("\nTimestamp: %ld", block.data.timestamp);
		printf("\nHash: %zu", block.getHash());
		printf("\nPrevious Hash: %zu", block.getPreviousHash());
		printf("\nIs Block Valid?: %s", btoa(block.isHashValid()));
		printf("\nIs Chain Valid?: %s", btoa(isChainValid()));
	}
}