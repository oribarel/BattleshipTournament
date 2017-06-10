#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>

using namespace std;

/*************************
*      ThreadPool       *
*************************/
class ThreadPool
{
    /*
    * example ThreadPool: https://stackoverflow.com/a/29742586
    */
public:
    ThreadPool(int threads);
    ThreadPool(ThreadPool &other);
    ~ThreadPool();
    void doJob(std::function <void(void)> func);
    void terminationDetection();

protected:
    void threadEntry(int i);

    std::mutex lock_;
    condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void(void)>> jobs_;
    std::vector <std::thread> threads_;
};


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

