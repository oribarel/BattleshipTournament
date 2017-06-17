#pragma once
#include "BoardClass.h"
#include <mutex>
#include "ScoreBoard.h"
#include "ThreadPool.h"

typedef IBattleshipGameAlgo *(*GetAlgoFunc)();

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
    int getNumOfAllBoardRotations() const;

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
    TournamentManager(int numOfThreads, vector<DLLData>& functions, vector<BoardFullData>& boards);
    void runTournament();
    void runNextRound(vector<gameEntry>& roundGames);
    bool tournamentOver() const;
    void runGame(gameEntry ge);
    const DLLData& getAlgo(int inx) const;
    const BoardFullData& getBoard(int inx) const;

protected:
    ScoreBoard score_board;                                ///< score board
    Scheduler scheduler;                                   ///< tournament scheduler
    //vector<Board> boards;                                  ///< boards of tournament
	vector<BoardFullData> boards;                                  ///< boards of tournament
    //vector<pair<GetAlgoFunc, string>> get_algos_vector;    ///< functions to algo instance making
	vector<DLLData> get_algos_vector;					   ///< functions to algo instance making
    mutex get_algos_lock;                                  ///< no cuncurrent algo instance making (underlying impl might fail)
    mutex round_m;                                           ///< round mutex
    condition_variable round_cv;                           ///< round condition variable
    ThreadPool pool;                                       ///< ThreadPool
    ThreadPool reporter;                                   ///< reporter
};
