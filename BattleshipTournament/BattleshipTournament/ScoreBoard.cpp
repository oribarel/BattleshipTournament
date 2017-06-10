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

//-- todo: fix ordering issue
void ScoreBoard::update(gameEntry & ge, pair<int, int> scores)
{
    if (scores.first != scores.second)
    {
        if (scores.first > scores.second)
        {
            entries[ge.players_indices.first].wins += 1;
            entries[ge.players_indices.second].losses += 1;
        }
        else
        {
            entries[ge.players_indices.second].wins += 1;
            entries[ge.players_indices.first].losses += 1;
        }
    }
    entries[ge.players_indices.first].ptsFor += scores.first;
    entries[ge.players_indices.first].ptsAgainst += scores.second;

    entries[ge.players_indices.second].ptsFor += scores.second;
    entries[ge.players_indices.second].ptsAgainst += scores.first;
}
