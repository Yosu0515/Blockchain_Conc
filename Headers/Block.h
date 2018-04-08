#pragma once

#include "TransactionData.h"

class Block
{
private:
	int index_{};
	bool is_genesis_block_{};
	std::string block_hash_;
	std::string previous_hash_;
	std::string generate_hash();

public:

	std::string try_generate_hash(int32_t number) const;
	int32_t rand_hash_number{};
	size_t min_number{};
	size_t max_number{};

	Block();
	// Constructor
	Block(int idx, TransactionData d, std::string prev_hash, bool is_genesis_block);

	// Get Index
	int get_index() const;

	// Get Original Hash
	std::string get_hash() const;

	// Get Previous Hash
	std::string get_previous_hash() const;

	// Transaction Data
	// Would ordinarily be a private member with a "getter": getData()
	TransactionData data;

	// Validate Hash
	bool is_hash_valid();
};

