#pragma once

struct TransactionData
{
	double amount;
	std::string senderKey{};
	std::string receiverKey{};
	time_t timestamp;

	TransactionData() = default;

	TransactionData(double amt, std::string sender, std::string receiver, time_t time) : amount(amt), senderKey(sender), receiverKey(receiver), timestamp(time) {}
};
