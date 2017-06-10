#include <iostream>
#include <iso646.h>
#include "GameManager.h"
#include "TournamentManager.h"


//-- ctor
TournamentManager::TournamentManager(
    int numOfThreads, vector<GetAlgoFunc>& functions, vector<Board>& boards) :
score_board(ScoreBoard(functions.size())),
scheduler(Scheduler(functions.size(), boards.size())),
boards(boards),
get_algos_vector(functions),
pool(ThreadPool(numOfThreads))
{}

void TournamentManager::runTournament()
{
    while(!tournamentOver())
    {
        auto round_games = vector<gameEntry>();
        scheduler.scheduleNextRound(round_games);
        runNextRound(round_games); //-- concurrent
        score_board.displayScores();
    }
    displayScores_tournamentEnd();
}

/* a thread job */
void TournamentManager::runGame(gameEntry ge)
{
    ////////////////////////////
    ///  create algos one-by-one
    ////////////////////////////
    get_algos_lock.lock();
    GameManager gm(ge, *this);
    get_algos_lock.unlock();


    /////////////////////////////
    /// run game manager
    /////////////////////////////
    pair<int,int> scores = gm.runGame();
    score_board.update(ge, scores);
}

//-- concurrently
void TournamentManager::runNextRound(vector<gameEntry>& roundGames)
{
    for (const auto& game : roundGames)
    {
        pool.doJob(bind(&TournamentManager::runGame, this, game));
    }
    pool.terminationDetection();
}

void TournamentManager::displayScores_tournamentEnd()
{
    cout << "Tournament Ended" << endl;
}

bool TournamentManager::tournamentOver()
{
    return (scheduler.getNumOfAllBoardRotations() > 1);
}

////////////////////////
///    getters      ////
////////////////////////

const GetAlgoFunc& TournamentManager::getAlgo(int inx) const
{
    return get_algos_vector[inx];
}

const Board& TournamentManager::getBoard(int inx) const
{
    return boards[inx];
}




///////////////////////////////////////////////////////
///                     Scheduler                   ///
///////////////////////////////////////////////////////

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

int Scheduler::getNumOfAllBoardRotations() const
{
    return num_of_times_passed_all_boards;
}
