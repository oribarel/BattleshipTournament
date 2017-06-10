#include <ostream>
#include <iostream>
#include <mutex>
#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numOfthreads) : shutdown_(false)
{
    // Create the specified number of threads
    threads_.reserve(numOfthreads);
    for (unsigned int i = 0; i < numOfthreads; ++i)
        threads_.emplace_back(bind(&ThreadPool::threadEntry, this, i));
}

ThreadPool::ThreadPool(ThreadPool& other) : ThreadPool(static_cast<unsigned int>(other.threads_.size())) {}

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

void ThreadPool::doJob(function <void(void)> func)
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

/*
* This function makes sure all jobs are done
*/
void ThreadPool::terminationDetection()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(lock_);
            if (jobs_.empty())
            {
                std::cout << "termination detection" << std::endl;
                return;
            }
        }//--lock dies here
         //-- maybe next time...
    }
}
