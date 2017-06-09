#pragma once
#include <vector>
using namespace std;

struct ScoreBoardEntry
{
    int algoID;
    vector<int> played_A_player_against;

    int wins;
    int losses;
    int ptsFor;
    int ptsAgainst;

    double precentage();
    ScoreBoardEntry(int algoID);
};

class ScoreBoard
{
public:
    ScoreBoard(int numPlayers);
private:
    int numOfPlayers;
    vector<ScoreBoardEntry> entries;
};
