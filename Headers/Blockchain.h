#pragma once

#include <vector>

class Blockchain
{
private:
	Block createGenesisBlock();
	std::vector<Block> chain_;

public:
	// Constuctor
	Blockchain();

	// Public Functions
	std::vector<Block> get_chain();
	Block *get_latest_block();
	bool is_chain_valid();
	void add_block(TransactionData data);
	void print_chain();
};
