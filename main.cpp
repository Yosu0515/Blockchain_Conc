#include <iostream>
#include <ctime>
#include <string>

#include "Headers/TransactionData.h"
#include "Headers/Block.h"
#include "Headers/Blockchain.h"
#include "Headers/Threadpool.hpp"

int main()
{
	// Start Blockchain
	Blockchain alpha_chain;

	// Start Thread Pool
	ThreadPool thread_pool{ 8 };

	while (true) {

		std::string sender;
		std::string receiver;
		size_t transaction_amount;

		std::cout << "\n\nEnter your name\n";
		std::cin >> sender;

		std::cout << "Enter receivers name\n";
		std::cin >> receiver;

		std::cout << "Enter transaction amount\n";
		std::cin >> transaction_amount;

		// Check if last cin was valid
		while (std::cin.fail()) {
			std::cout << "Invalid input\n";
			std::cin.clear();
			std::cin.ignore(256, '\n');
			std::cout << "Enter transaction amount\n";
			std::cin >> transaction_amount;
		}

		/**
		 * TODO
		 * Make concurrent
		 */
		time_t data_time;

		thread_pool.enqueue([&] {
			TransactionData data(transaction_amount, sender, receiver, time(&data_time));
			alpha_chain.addBlock(data);
		});

		char decider;
		std::cout << "Continue or check blockchain? (y/n/b)\n";
		std::cin >> decider;

		// Check if last cin was valid
		while (std::cin.fail()) {
			std::cout << "Invalid input\n";
			std::cin.clear();
			std::cin.ignore(256, '\n');
			std::cout << "Continue or check blockchain? (y/n/b)\n";
			std::cin >> decider;
		}

		if (decider == 'b') {
			alpha_chain.printChain();

			std::cout << "\n\nAdd another transaction? (y/n)\n";
			std::cin >> decider;

			if (decider == 'n') {
				break;
			}

		}
		else if (decider == 'n') {
			break;
		}
	}

	//    // Someone's getting sneaky
	//    Block *hackBlock = awesomeCoin.getLatestBlock();
	//    hackBlock->data.amount = 10000; // Oh yeah!
	//    hackBlock->data.receiverKey = "Jon"; // mwahahahaha!

	return 0;
}