#include <iostream>
#include <ctime>
#include <string>

#include "Headers/TransactionData.h"
#include "Headers/Block.h"
#include "Headers/Blockchain.h"
#include "Headers/Threadpool.h"

int main()
{
	// Start Blockchain
	Blockchain alpha_chain;

	//const size_t pool_size = 8;
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

		// TODO hard-code some amount of blocks that are going to be checked in the proof of work

		// TODO for the entire pool size, enqueue a new task for each worker to try and get the correct block hash
		// when one of them succeeds, proceed to the next block
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