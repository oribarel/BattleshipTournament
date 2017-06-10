#pragma once
#include <vector>
#include <list>

using namespace std;
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
///////////////////////////
///      gameEntry      
///   GameManager input
///////////////////////////

struct gameEntry
{
    pair<int, int> players_indices; ///< player_A, player_B
    int board_inx;

    gameEntry(int inx1, int inx2, int brdinx, bool a_is_smaller_b_is_larger) :
        board_inx(brdinx)
    {
        int max_inx = max(inx1, inx2);
        int min_inx = min(inx1, inx2);
        if (a_is_smaller_b_is_larger)
        {
            players_indices = make_pair(min_inx, max_inx);
        }
        else
        {
            players_indices = make_pair(max_inx, min_inx);
        }
    }
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
    static gameHistory add_game_histories(gameHistory gh1, gameHistory gh2);
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
    ScoreBoard(unsigned int numPlayers);
    void update(gameEntry& ge, pair<int, int> scores);
    void displayScores() const;
    int ScoreBoard::common_history_length() const;
private:
    int numOfPlayers;
    int last_reported_round;
    vector<ScoreBoardEntry> entries;
};
