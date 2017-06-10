#pragma once
#include <vector>

using namespace std;

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
    void update(gameEntry& ge, pair<int, int> scores);
private:
    int numOfPlayers;
    vector<ScoreBoardEntry> entries;
};
