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

	const size_t pool_size = 8;
	// Start Thread Pool
	ThreadPool thread_pool{ pool_size };

	const string senders[3] = {

		"Daniel", "Jos", "Aswin"
	};

	const string receivers[3] = {
		"Kleis", "Jan", "Obe"
	};

	// the current block index in the blockchain, used by the tasks
	atomic<int> cur_block_i(0);

	// the total amount of blocks we 'simulate'
	const int max_block_i = 10;

	// the index of the last proven block
	atomic<int> lastProven(0);

	// the number of finished tasks
	atomic<int> num_finished(0);

	// the number of total steps taken by tasks
	atomic<int> steps_taken(0);

	// let's make # blocks for 'simulation'
	srand(time(nullptr)); //initialize the random seed
	for (int i = 0; i < max_block_i; ++i)
	{
		// random sender
		const auto sender = senders[rand() % sizeof senders / sizeof senders[0]];
		// random receiver
		const auto receiver = receivers[rand() % sizeof receivers / sizeof receivers[0]];
		// generate a random transaction amount
		const size_t transaction_amount = rand() % 4950 + 50;

		// add new block to the blockchain
		time_t data_time;
		const TransactionData data(transaction_amount, sender, receiver, time(&data_time));
		blockchain.add_block(data);
	}

	// quick check for some clashes, mainly used for debug
	//vector<string> hashes;
	//vector<int32_t> numbers;
	//for (auto &block : blockchain.getChain())
	//{
	//	if (std::find(hashes.begin(), hashes.end(), block.getHash()) != std::end(hashes))
	//	{
	//		// clash!
	//		std::ostringstream msg;
	//		msg << "Clash at " << block.getIndex() << ", hash:" << block.getHash() << std::endl;
	//		printf_s(msg.str().c_str());
	//	}
	//	else
	//		hashes.push_back(block.getHash());

	//	if (std::find(numbers.begin(), numbers.end(), block.randHashNumber) != std::end(numbers))
	//	{
	//		// clash!
	//		std::ostringstream msg;
	//		msg << "Clash at " << block.getIndex() << ", number:" << block.randHashNumber << std::endl;
	//		printf_s(msg.str().c_str());
	//	}
	//	else
	//		numbers.push_back(block.randHashNumber);

	//}

	const auto start_all = std::chrono::system_clock::now();

	// all workers try to 'prove the work'
	while (true)
	{
		// if the last proven is not the one we work on, keep going
		if (lastProven != cur_block_i && num_finished < 7)
			continue;

		// wait for all wasks to finish
		if (lastProven == cur_block_i && cur_block_i != 0 && num_finished < 7)
			continue;

		// the last one, we're done
		if (cur_block_i >= max_block_i)
		{
			const auto end_all = std::chrono::system_clock::now();
			std::chrono::duration<double> diff_all = end_all - start_all;
			std::cout << "End of proof of work, total time taken: " << diff_all.count() << "s" << std::endl;
			break;
		}

		// reset stuff
		num_finished = 0;
		steps_taken = 0;

		// move on to next block
		++cur_block_i;

		// for pool size, enqueue new tasks
		for (int i = 0; i < pool_size; ++i)
		{
			int task_i = i;
			thread_pool.enqueue([&, task_i]
			{
				auto start_thread = std::chrono::system_clock::now();
				chrono::system_clock::time_point end_thread;
				std::chrono::duration<double> diff_thread;
				// current block to prove
				Block block = blockchain.get_chain()[cur_block_i];

				// the real hash we need to find
				string realHash = block.get_hash();
				int blockI = block.get_index();
				string blockPrevHash = block.get_previous_hash();
				string prevBlockHash = blockchain.get_chain()[blockI - 1].get_hash();

				// if the previous hash doesnt match the hash of the previous block
				// then this block is invalid
				if (blockPrevHash != prevBlockHash)
				{
					lastProven = cur_block_i.load();
					std::ostringstream msg;
					msg << "Thread " << std::this_thread::get_id() << " failed to prove block " << cur_block_i << " because previous hash does not match"
						<< std::endl << "Block prev hash: " << blockPrevHash
						<< std::endl << "Actaul prev hash: " << block.get_hash()
						<< std::endl << std::endl;
					printf_s(msg.str().c_str());

					return;
				}

				// spread search space to fasten the search
				// half tasks go from min->max, other half max->min, and each task at different increments
				// essentially, each task's increment is offset by 1
				int halfpool = int(pool_size / 2);
				bool startAtMin = task_i < halfpool;
				int n = int(startAtMin ? block.min_number + task_i : block.max_number - task_i + halfpool);
				int step = startAtMin ? halfpool : -halfpool;

				// simulate trying to get the correct hash
				while ((startAtMin && n <= block.max_number) || (!startAtMin && n >= block.min_number))
				{
					// if we were proven, go out
					if (lastProven >= block.get_index())
						break;

					++steps_taken;

					// try getting hash with number n
					string foundHash = block.try_generate_hash(n);

					// the hash we found is a match
					if (foundHash == realHash)
					{
						// if we were proven, go out
						if (lastProven >= block.get_index())
							break;

						// we are the one to prove this block
						lastProven = cur_block_i.load();

						end_thread = std::chrono::system_clock::now();
						diff_thread = end_thread - start_thread;

						std::ostringstream msg;
						msg << "Thread " << std::this_thread::get_id() << " proved block " << cur_block_i << " in " << steps_taken << " steps and " << diff_thread.count() << "s"
							<< std::endl << "Found: " << foundHash
							<< std::endl << "Real: " << block.get_hash()
							<< std::endl << std::endl;
						printf_s(msg.str().c_str());

						break;
					}

					// try again with next value
					n += step;

					// if you have trouble because it takes full cpu load, add this
					// but adding this makes it very slow, 100-1000ms
#ifndef  DEBUG
					//std::this_thread::sleep_for(100ms);
#endif

				}

				++num_finished;
			});
		}
	}

	// Someone's getting sneaky
	//Block *hackBlock = blockchain.getLatestBlock();
	//hackBlock->data.amount = 10000; // Oh yeah!
	//hackBlock->data.receiverKey = "Jon"; // mwahahahaha!

	// await threads
	cin.ignore();

	return EXIT_SUCCESS;
}
