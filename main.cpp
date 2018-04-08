#include <iostream>
#include <ctime>
#include <string>
#include <sstream>

#include "Headers/TransactionData.h"
#include "Headers/Block.h"
#include "Headers/Blockchain.h"
#include "Headers/Threadpool.h"
#include <random>
#include <atomic>


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

	atomic<int> cur_block_i(0); // at 0, +1 = 1, don't do genesis block
	const int max_block_i = 10;
	atomic<int> lastProven(0);
	atomic<int> num_finished(0);
	atomic<int> steps_taken(0);
	std::mutex mtx;           // mutex for critical section
	//string foundHash = "0";

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

	vector<string> hashes;
	vector<int32_t> numbers;
	for (auto &block : blockchain.getChain())
	{
		if (std::find(hashes.begin(), hashes.end(), block.getHash()) != std::end(hashes))
		{
			// clash!
			std::ostringstream msg;
			msg << "Clash at " << block.getIndex() << ", hash:" << block.getHash() << std::endl;
			printf_s(msg.str().c_str());
		}
		else
			hashes.push_back(block.getHash());

		if (std::find(numbers.begin(), numbers.end(), block.randHashNumber) != std::end(numbers))
		{
			// clash!
			std::ostringstream msg;
			msg << "Clash at " << block.getIndex() << ", number:" << block.randHashNumber << std::endl;
			printf_s(msg.str().c_str());
		}
		else
			numbers.push_back(block.randHashNumber);

	}

	vector<string> hashes2;

	while (true)
	{
		if (lastProven != cur_block_i)
			continue;

		// wait for all wasks to finish
		if (lastProven == cur_block_i && cur_block_i != 0 && num_finished < 7)
			continue;

		// the last one, we're done
		if (cur_block_i >= max_block_i)
		{
			std::cout << "End of proof of work" << std::endl;
			break;
		}

		// reset stuff
		num_finished = 0;
		steps_taken = 0;

		// move on to next block
		++cur_block_i;



		for (int i = 0; i < 8; ++i)
		{
			int task_i = i;
			thread_pool.enqueue([&, task_i]
			{
				// current block to prove
				Block block = blockchain.getChain()[cur_block_i];

				// the real hash we need to find
				string realHash = block.getHash();
				int blockI = block.getIndex();
				string blockPrevHash = block.getPreviousHash();
				string prevBlockHash = blockchain.getChain()[blockI - 1].getHash();

				if (blockPrevHash != prevBlockHash)
				{
					lastProven = cur_block_i.load();
					std::ostringstream msg;
					msg << "Thread " << std::this_thread::get_id() << " failed to prove block " << cur_block_i << " because previous hash does not match"
						<< std::endl << "Block prev hash: " << blockPrevHash
						<< std::endl << "Actaul prev hash: " << block.getHash()
						<< std::endl << std::endl;
					printf_s(msg.str().c_str());

					return;
				}

				// spread search space to fasten the search
				bool startAtMin = task_i < 4;
				int incrementStep = task_i % 4 + 1;
				size_t n = startAtMin ? block.minNumber : block.maxNumber;

				// simulate trying to get the correct hash
				while ((startAtMin && n <= block.maxNumber) || (!startAtMin && n >= block.minNumber))
				{
					// if we were proven
					if (lastProven >= block.getIndex())
						break;

					++steps_taken;

					// try getting hash with number n
					string foundHash = block.tryGenerateHash(n);

					// the hash we found is a match
					if (foundHash == realHash
						&& prevBlockHash == blockPrevHash)
					{
						if (lastProven >= block.getIndex())
							break;

						lastProven = cur_block_i.load();
						std::ostringstream msg;
						msg << "Thread " << std::this_thread::get_id() << " proved block " << cur_block_i << " in " << steps_taken << " steps"
							<< std::endl << "Found: " << foundHash
							<< std::endl << "Real: " << block.getHash()
							<< std::endl << std::endl;
						printf_s(msg.str().c_str());

						break;
					}

					// try again with next value
					n += startAtMin ? incrementStep : -incrementStep;
				}

				++num_finished;
			});
		}
	}

	//    // Someone's getting sneaky
	//    Block *hackBlock = awesomeCoin.getLatestBlock();
	//    hackBlock->data.amount = 10000; // Oh yeah!
	//    hackBlock->data.receiverKey = "Jon"; // mwahahahaha!
	// await threads
	cin.ignore();

	return EXIT_SUCCESS;
}
