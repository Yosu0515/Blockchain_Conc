#include <iostream>
#include <ctime>
#include <string>
#include <sstream>

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

	int cur_block_i = 0; // at 0, +1 = 1, don't do genesis block
	const int max_block_i = 10;
	bool blockProven = true;

	// let's make 10 blocks for 'simulation'
	srand(time(nullptr)); //initialize the random seed
	for (int i = 0; i < max_block_i; ++i)
	{
		const auto sender = senders[rand() % sizeof senders / sizeof senders[0]];
		const auto receiver = receivers[rand() % sizeof receivers / sizeof receivers[0]];
		int randt = rand() % 4950 + 50;
		const size_t transaction_amount = randt; // generate a random transaction amount

		time_t data_time;
		const TransactionData data(transaction_amount, sender, receiver, time(&data_time));
		blockchain.addBlock(data);
	}

	// We don't want proof of work on the genesis block
	//if (!is_genesis_block) {
	//	std::cout << "Starting proof of work for block " << idx << std::endl;
	//	const bool success = proofOfWork(minNumber, maxNumber, );
	//	std::cout << "Proof of work " << (success ? "succeeded" : "failed") << " for block " << idx << std::endl;
	//}

	// all workers try to 'prove the work'



	while (true)
	{
		if (!blockProven)
			continue;
		else if (cur_block_i >= max_block_i)
			break;
		else
		{
			// wait for all wasks to finish
			if (!thread_pool.no_tasks())
				continue;

			cur_block_i++;
			blockProven = false;

			for (int i = 0; i < 8; ++i)
			{
				int i2 = i;
				thread_pool.enqueue([&, i2] {

					// current block to prove
					Block block = blockchain.getChain()[cur_block_i];

					// try getting the correct hash
					size_t foundHash = 0;
					bool startAtMin = i2 < 4;
					int incrementStep = i2 % 4 + 1;
					size_t n = startAtMin ? block.minNumber : block.maxNumber;

					// simulate trying to get the correct hash
					while ((foundHash != block.getHash()
						&& (startAtMin && n <= block.maxNumber)) || (!startAtMin && n >= block.minNumber))
					{

						if (blockProven)
							return;

						foundHash = block.tryGenerateHash(n);

						if (foundHash == block.getHash())
						{
							if (blockProven)
								return;

							blockProven = true;
							stringstream msg;
							msg << "Thread " << std::this_thread::get_id() << " proved block " << cur_block_i << " found: " << foundHash << ", and real: " << block.getHash() << std::endl;
							cout << msg.str();

							return;
						}

						// try again with next value
						n += startAtMin ? incrementStep : -incrementStep;
					}

				});
			}
		}
	}

	std::cout << "End of proof of work" << std::endl;

	// await threads
	cin.ignore();

	//    // Someone's getting sneaky
	//    Block *hackBlock = awesomeCoin.getLatestBlock();
	//    hackBlock->data.amount = 10000; // Oh yeah!
	//    hackBlock->data.receiverKey = "Jon"; // mwahahahaha!

	return EXIT_SUCCESS;
}
