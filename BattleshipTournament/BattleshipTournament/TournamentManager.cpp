#include "TournamentManager.h"
#include <iostream>
#include <iso646.h>
#include "GameManager.h"

ThreadPool::ThreadPool(int numOfthreads) : shutdown_(false)
{
    // Create the specified number of threads
    threads_.reserve(numOfthreads);
    for (int i = 0; i < numOfthreads; ++i)
        threads_.emplace_back(bind(&ThreadPool::threadEntry, this, i));
}

ThreadPool::ThreadPool(ThreadPool& other) : ThreadPool(other.threads_.size()){}

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
            unique_lock<mutex> lock(lock_);
            if (jobs_.empty())
            {
                cout << "termination detection" << endl;
                return;
            }
        }//--lock dies here
        //-- maybe next time...
    }
}


TournamentManager::TournamentManager(
    int numOfThreads, vector<GetAlgoFunc>& functions, vector<Board>& boards) :
    score_board(ScoreBoard(2)),             ///< todo
    scheduler(Scheduler(2, 4)),             ///< todo
    boards(boards),
    get_algos_vector(functions),
    pool(ThreadPool(numOfThreads))
{
}

void TournamentManager::runTournament()
{
    while(!tournamentOver())
    {
        auto round_games = vector<gameEntry>();
        scheduler.scheduleNextRound(round_games);
        runNextRound(round_games);
        displayScores();
    }
    displayScores_tournamentEnd();
}

void TournamentManager::runGame(gameEntry ge)
{
    ////////////////////////////
    ///  create algos one-by-one
    ////////////////////////////
    get_algos_lock.lock();
    GameManager gm(ge, *this);
    get_algos_lock.unlock();

    pair<int,int> scores = gm.runGame();
    score_board.update(ge, scores);
    //todo: scoreboard update
}

const GetAlgoFunc& TournamentManager::getAlgo(int inx) const
{
    return get_algos_vector[inx];
}

const Board& TournamentManager::getBoard(int inx) const
{
    return boards[inx];
}

void TournamentManager::runNextRound(vector<gameEntry>& roundGames)
{
    for (const auto& game : roundGames)
    {
        pool.doJob(bind(&TournamentManager::runGame, this, game));
    }
    pool.terminationDetection();
}

void TournamentManager::displayScores() const
{
    cout << "displayScores #boards=" << boards.size() << " #players=" << get_algos_vector.size() << endl;
}

void TournamentManager::displayScores_tournamentEnd()
{
    cout << "Tournament Ended" << endl;
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


bool TournamentManager::tournamentOver()
{
    return (scheduler.getNumOfAllBoardRotations() > 1);
}

// CTOR
Scheduler::Scheduler(int numOfPlayers, int numOfBoards) : 
    scheduling(vector<int>(numOfPlayers, -1)),
    numOfPlayers(numOfPlayers),    
    numOfBoards(numOfBoards),
    currBoard(0),
    num_of_times_passed_all_boards(0)
{
    auto schedulingEntries = ((numOfPlayers + 1) / 2) * 2;
    for (int i = 0; i < schedulingEntries; i++)
    {
        scheduling[i] = i;
    }
}

void Scheduler::scheduleNextRound(vector<gameEntry>& roundGames)
{
    int halfSize = (numOfPlayers + 1) / 2;
    roundGames.clear();
    roundGames.reserve(halfSize);

    for (int i = 0; i < halfSize; i++)
    {
        if (scheduling[i] == -1 or scheduling[i+halfSize] == -1)
            continue;
        roundGames.push_back(gameEntry(                 //TODO: what about emplace?
            scheduling[i], scheduling[i + halfSize],
            currBoard, num_of_times_passed_all_boards == 0));
    }
    updateSchedulingVector();
}

void Scheduler::updateSchedulingVector()
{
    //https://en.wikipedia.org/wiki/Round-robin_tournament#Scheduling_algorithm
    auto halfSize = (numOfPlayers + 1) / 2;
    auto schedulingEntries = halfSize * 2;

    auto newScheduling = vector<int>(scheduling.begin(), scheduling.end());
    //-- walk:
    
    for (auto i = 1; i < halfSize - 1; i++) //-- top row
    {
        newScheduling[i + 1] = scheduling[i];
    }

    //-- two crossings:
    newScheduling[schedulingEntries - 1] = scheduling[halfSize - 1];
    newScheduling[1] = scheduling[halfSize];

    for (auto i = schedulingEntries - 1; i > halfSize; i--) //-- bottom row
    {
        newScheduling[i - 1] = scheduling[i];
    }

    //set new scheduling
    scheduling = newScheduling;

    if (completedBoard())
    {
        currBoard++;
        if (currBoard > numOfBoards)
        {
            currBoard = 0;
            num_of_times_passed_all_boards++;
        }
    }
}

bool Scheduler::completedBoard()
{
    return scheduling[1] == 1;
}

int Scheduler::getNumOfAllBoardRotations()
{
    return num_of_times_passed_all_boards;
}
