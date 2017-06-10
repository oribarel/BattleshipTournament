#pragma once
#include <vector>
#include <list>

using namespace std;

///////////////////////////
///      gameEntry      
///   GameManager input
///////////////////////////

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


//////////////////////////////
///       gameHistory
/// log game results with
/// a vector of these
/////////////////////////////

struct gameHistory
{
    int win;
    int loss;
    int pts_for;
    int pts_against;

    gameHistory(int pts_for, int pts_against) :
        pts_for(pts_for),
        pts_against(pts_against),
        win(static_cast<int>(pts_for > pts_against)),
        loss(static_cast<int>(pts_for < pts_against)){}

    double gameHistory::precentage() const;
    static bool compare(const gameHistory first, const gameHistory second);

};


//////////////////////////////////
///       ScoreBoardEntry
/// represents the state of 
/// the algo in the tournament
//////////////////////////////////

struct ScoreBoardEntry
{
    int algoID;
    // todo algo name
    int history_len;
    list<gameHistory> history;

    ScoreBoardEntry(int algoID) :
        algoID(algoID),
        history_len(0),
        history(list<gameHistory>()) {}
};


//////////////////////////////
///       ScoreBoard
/// represents the state of 
/// the tournament
///////////////////////////////

class ScoreBoard
{
public:
    ScoreBoard(int numPlayers);
    void update(gameEntry& ge, pair<int, int> scores);
    void displayScores() const;
private:
    int numOfPlayers;
    vector<ScoreBoardEntry> entries;
};
