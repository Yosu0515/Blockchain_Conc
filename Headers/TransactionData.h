#pragma once
#include <utility>

struct TransactionData
{
	double amount;
	std::string sender_key{};
	std::string receiver_key{};
	time_t timestamp;

	TransactionData() = default;

	TransactionData(const double amt, std::string sender, std::string receiver, const time_t time)
		: amount(amt), sender_key(std::move(sender)), receiver_key(std::move(receiver)), timestamp(time)
	{

	}
};
