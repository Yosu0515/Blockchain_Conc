#include <iostream>
#include <ctime>
#include <string>

#include "Headers/TransactionData.h"
#include "Headers/Block.h"
#include "Headers/Blockchain.h"
#include "Headers/Threadpool.h"
#include <random>

int main()
{
	// Start Blockchain
	Blockchain blockchain;

	//const size_t pool_size = 8;
	// Start Thread Pool
	ThreadPool thread_pool{ 8 };

	const string senders[3] = {

		"Daniel", "Jos", "Aswin"
	};

	const string receivers[3] = {
		"Kleis", "Jan", "Obe"
	};

	while (true) {

		// let's make 10 blocks for 'simulation'
		srand(time(nullptr)); //initialize the random seed
		for (int i = 0; i < 10; ++i)
		{
			const auto sender = senders[rand() % sizeof senders / sizeof senders[0]];
			const auto receiver = receivers[rand() % sizeof receivers / sizeof receivers[0]];
			int randt = rand() % 4950 + 50;
			const size_t transaction_amount = randt; // generate a random transaction amount

			time_t data_time;
			const TransactionData data(transaction_amount, sender, receiver, time(&data_time));
			blockchain.addBlock(data);
		}

		bool b = true;

		//std::cout << "\n\nEnter your name\n";
		//std::cin >> sender;

		//std::cout << "Enter receivers name\n";
		//std::cin >> receiver;

		//std::cout << "Enter transaction amount\n";
		//std::cin >> transaction_amount;

		//// Check if last cin was valid
		//while (std::cin.fail()) {
		//	std::cout << "Invalid input\n";
		//	std::cin.clear();
		//	std::cin.ignore(256, '\n');
		//	std::cout << "Enter transaction amount\n";
		//	std::cin >> transaction_amount;
		//}

		// TODO hard-code some amount of blocks that are going to be checked in the proof of work

		// TODO for the entire pool size, enqueue a new task for each worker to try and get the correct block hash
		// when one of them succeeds, proceed to the next block



		// We don't want proof of work on the genesis block
		//if (!is_genesis_block) {
		//	std::cout << "Starting proof of work for block " << idx << std::endl;
		//	const bool success = proofOfWork(minNumber, maxNumber, );
		//	std::cout << "Proof of work " << (success ? "succeeded" : "failed") << " for block " << idx << std::endl;
		//}

		thread_pool.enqueue([&] {

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
			blockchain.printChain();

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
