#pragma once
#include "BoardClass.h"
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include "ScoreBoard.h"

typedef IBattleshipGameAlgo *(*GetAlgoFunc)();

/*************************
*      gameEntry         *
*************************/

struct gameEntry
{
    pair<int, int> players_indices;
    int board_inx;
    bool A_is_smaller_B_is_larger;

    gameEntry(int inx1, int inx2, int brdinx, bool a_is_smaller_b_is_larger) :
        players_indices(pair<int, int>(inx1, inx2)),
        board_inx(brdinx),
        A_is_smaller_B_is_larger(a_is_smaller_b_is_larger)
    {}
};


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
    void doJob(function <void(void)> func);
    void terminationDetection();

protected:
    void threadEntry(int i);

    mutex lock_;
    condition_variable condVar_;
    bool shutdown_;
    queue <function <void(void)>> jobs_;
    vector <thread> threads_;
};

/****************************
*      Scheduler            *
*****************************/

class Scheduler
{
public:
    Scheduler(int numOfPlayers, int numOfBoards);
    void scheduleNextRound(vector<gameEntry>& roundGames);
    void updateSchedulingVector();
    bool completedBoard();
    int getNumOfAllBoardRotations();

private:
    vector<int> scheduling;
    int numOfPlayers;
    int numOfBoards;
    int currBoard;
    int num_of_times_passed_all_boards;
};

/****************************
*      TournamentManager    *
*****************************/

class TournamentManager
{
public:
    TournamentManager(int numOfThreads);
    void runTournament();
    void runNextRound(vector<gameEntry>& roundGames);
    bool tournamentOver();
    void displayScores() const;
    void displayScores_tournamentEnd();
    void runGame(gameEntry ge);

protected:
    ScoreBoard score_board;                  ///< score board
    Scheduler scheduler;                     ///< tournament scheduler
    vector<Board> boards;                    ///< boards of tournament
    vector<GetAlgoFunc> get_algos_vector;    ///< functions to algo instance making
    mutex get_algos_lock;                    ///< no cuncurrent algo instance making (underlying impl might fail)
    ThreadPool pool;                         ///< ThreadPool
};
