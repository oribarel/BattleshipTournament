#pragma once
#include "BoardClass.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>


class ThreadPool
{
    /*
     * example ThreadPool: https://stackoverflow.com/a/29742586
     */
public:
    ThreadPool(int threads);
    ThreadPool(const ThreadPool & other);
    ~ThreadPool();
    void doJob(function <void()> func);

protected:
    void threadEntry(int i);

    mutex lock_;
    condition_variable condVar_;
    bool shutdown_;
    queue <function <void()>> jobs_;
    vector <thread> threads_;
};


class TournamentManager
{
public:
    TournamentManager(int numOfThreads);
    void runTournament();
    void runNextRound();

protected:
    ThreadPool pool;
    vector<Board> boards;
    vector<unique_ptr<IBattleshipGameAlgo>> players;

};

