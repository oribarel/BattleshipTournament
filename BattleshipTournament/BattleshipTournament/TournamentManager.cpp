#include <iostream>
#include <iso646.h>
#include "GameManager.h"
#include "TournamentManager.h"

unsigned int round_games_finished = 0;

//-- ctor
TournamentManager::TournamentManager(
	int numOfThreads, vector<DLLData>& functions, vector<BoardFullData>& boards) :
score_board(ScoreBoard(functions)),
scheduler(Scheduler(static_cast<unsigned int>(functions.size()), static_cast<unsigned int>(boards.size()))),
boards(boards),
get_algos_vector(functions),
pool(ThreadPool(numOfThreads)),
reporter(ThreadPool(1))
{}

void TournamentManager::runTournament()
{
    while(!tournamentOver())
    {
        auto round_games = vector<gameEntry>();
        scheduler.scheduleNextRound(round_games);
        runNextRound(round_games); //-- concurrent
        //score_board.displayScores();
    }
    displayScores_tournamentEnd();
}

/* a thread job */
void TournamentManager::runGame(gameEntry ge)
{
    //--  create algos one-by-one
    get_algos_lock.lock();
    GameManager gm(ge, *this);
    get_algos_lock.unlock();

    //-- run game manager
    auto scores = gm.runGame();
    //-- update (and report case needed)
    reporter.doJob(std::bind(&ScoreBoard::update, &score_board, ge, scores));
    
    {
        unique_lock<mutex> lk(round_m);
        round_games_finished++;
    }
    round_cv.notify_all();

}

//-- concurrently
void TournamentManager::runNextRound(vector<gameEntry>& roundGames)
{
    round_games_finished = 0;
    for (const auto& game : roundGames)
    {
        pool.doJob(bind(&TournamentManager::runGame, this, game));
    }

    {
        unique_lock<mutex> lk(round_m);
        round_cv.wait(lk, [roundGames]{
            return round_games_finished == roundGames.size();
        });
    }

}

void TournamentManager::displayScores_tournamentEnd()
{
    cout << "Tournament Ended" << endl;
    score_board.displayScores();
}

bool TournamentManager::tournamentOver() const
{
    return (scheduler.getNumOfAllBoardRotations() > 1);
}

////////////////////////
///    getters      ////
////////////////////////

const DLLData& TournamentManager::getAlgo(int inx) const
{
    return get_algos_vector[inx];
}

const BoardFullData& TournamentManager::getBoard(int inx) const
{
    return boards[inx];
}


///////////////////////////////////////////////////////
///                     Scheduler                   ///
///////////////////////////////////////////////////////

// CTOR
Scheduler::Scheduler(int numOfPlayers, int numOfBoards) : 
    numOfPlayers(numOfPlayers),    
    numOfBoards(numOfBoards),
    currBoard(0),
    num_of_times_passed_all_boards(0)
{
    auto schedulingEntries = ((numOfPlayers + 1) / 2) * 2;
	scheduling = vector<int>(schedulingEntries, -1);
    for (int i = 0; i < numOfPlayers; i++)
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
        if (currBoard >= numOfBoards)
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
