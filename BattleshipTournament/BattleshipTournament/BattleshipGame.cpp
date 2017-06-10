// BattleshipGame.cpp : Defines the entry point for the console application.
//
#include "TournamentManager.h"
typedef IBattleshipGameAlgo *(*GetAlgoFunc)();

/*Main is for running games etc...*/
int main(int argc, char *argv[])
{
    //-- find all dlls and make a vector
    auto functions = vector<GetAlgoFunc>(); //todo
    //-- find all boards and make a vector
    auto boards = vector<Board>(); //todo
    //-- number of threads
    auto numOfThreads = 4; //todo

    //-- create tournament
    TournamentManager tm(numOfThreads, functions, boards);
    tm.runTournament();
	return 0;
}

