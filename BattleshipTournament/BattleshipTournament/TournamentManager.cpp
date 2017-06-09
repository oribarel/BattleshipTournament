#include "TournamentManager.h"

ThreadPool::ThreadPool(int threads) : shutdown_(false)
{
    // Create the specified number of threads
    threads_.reserve(threads);
    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(bind(&ThreadPool::threadEntry, this, i));
}


ThreadPool::~ThreadPool()
{
    {
        // Unblock any threads and tell them to stop
        unique_lock<mutex> lock(lock_);

        shutdown_ = true;
        condVar_.notify_all();
    } //-- here lock dies

    // Wait for all threads to stop
    cerr << "Joining threads" << endl;
    for (auto& thread : threads_)
        thread.join();
}

void ThreadPool::doJob(function <void()> func)
{
    // Place a job on the queue and unblock a thread
    unique_lock<mutex> lock(lock_);

    jobs_.emplace(move(func));
    condVar_.notify_one();
}

void ThreadPool::threadEntry(int i)
{
    function <void()> job;
    while (true)
    {
        {
            unique_lock<mutex> lock(lock_);
            while (!shutdown_ && jobs_.empty())
                condVar_.wait(lock);

            if (jobs_.empty())
            {
                // No jobs to do and we are shutting down
                cerr << "Thread " << i << " terminates" << endl;
                return;
            }

            cerr << "Thread " << i << " does a job" << endl;
            job = move(jobs_.front());
            jobs_.pop();
        }//-- here lock dies

        // Do the job without holding any locks
        job();
    }
}

/* example for using ThreadPool*/
//void silly(int n)
//{
//    // A silly job for demonstration purposes
//    std::cerr << "Sleeping for " << n << " seconds" << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(n));
//}
//
//int main()
//{
//    // Create two threads
//    ThreadPool p(2);
//
//    // Assign them 4 jobs
//    p.doJob(std::bind(silly, 1));
//    p.doJob(std::bind(silly, 2));
//    p.doJob(std::bind(silly, 3));
//    p.doJob(std::bind(silly, 4));
//}

TournamentManager::TournamentManager(int numOfThreads) :
    pool(ThreadPool(numOfThreads)),
    boards(vector<Board>()),
    players(vector<unique_ptr<IBattleshipGameAlgo>>()) {}

void TournamentManager::runTournament()
{
}

void TournamentManager::runNextRound()
{
}

