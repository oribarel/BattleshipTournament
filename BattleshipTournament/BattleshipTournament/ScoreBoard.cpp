#include "ScoreBoard.h"

double ScoreBoardEntry::precentage()
{
    double retVal = static_cast<double>(wins) / losses;
    return retVal;
}

ScoreBoardEntry::ScoreBoardEntry(int algoID) : 
    algoID(algoID),
    played_A_player_against(vector<int>()),
    wins(0),
    losses(0),
    ptsFor(0),
    ptsAgainst(0)
{
}

ScoreBoard::ScoreBoard(int numPlayers) : 
    numOfPlayers(numPlayers), 
    entries(vector<ScoreBoardEntry>())
{
}
