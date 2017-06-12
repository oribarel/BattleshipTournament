#include "ScoreBoard.h"
#include <iostream>
#include <sstream> 

double gameHistory::precentage() const
{
        if (win == 0.0)
            return 0.0;
        return static_cast<double>(win * 100) / (win + loss);
}

bool gameHistory::compare(const gameHistory first, const gameHistory second)
{
    return (first.precentage() > second.precentage());
}

gameHistory gameHistory::add_game_histories(gameHistory gh1, gameHistory gh2)
{
    gameHistory ret_gh(gh1.pts_for + gh2.pts_for, gh1.pts_against + gh2.pts_against);
    ret_gh.loss = gh1.loss + gh2.loss;
    ret_gh.win = gh1.win + gh2.win;
    return ret_gh;
}

//////////////////////////////
///       ScoreBoard
/// represents the state of 
/// the tournament
///////////////////////////////

ScoreBoard::ScoreBoard(vector<DLLData>& functions) : entries(vector<ScoreBoardEntry>())
{
    numOfPlayers = static_cast<int>(functions.size());
    last_reported_round = 0;
    int i = 0;
    for (const auto& func : functions)
    {
        entries.push_back(ScoreBoardEntry(i, func.file_name));
        i++;
    }
}

void ScoreBoard::update(gameEntry ge, pair<int, int> scores)
{
    //cerr << "in game " << ge.players_indices.first << " vs " << ge.players_indices.second << " on board "
    //    << ge.board_inx << ": score " << scores.first << "/" << scores.second << endl;

    gameHistory gh1(scores.first, scores.second);
    gameHistory gh2(scores.second, scores.first);

    entries[ge.players_indices.first].history.push_back(gh1);
    entries[ge.players_indices.first].history_len++;
    entries[ge.players_indices.second].history.push_back(gh2);
    entries[ge.players_indices.second].history_len++;

    int min_history_len = common_history_length();
    if (min_history_len > last_reported_round)
    {
        displayScores();
        last_reported_round++;
    }
}

int ScoreBoard::common_history_length() const
{
    int min_history_len = INT32_MAX;
    for (auto const& ge : entries)
    {
        min_history_len = min(min_history_len, ge.history_len);
    }
    return min_history_len;
}

void ScoreBoard::displayScores() const
{
    int min_history_len = common_history_length();
    if (min_history_len == 0)
    {
        return;
    }

    //abuse gameHistory as data container
    auto partial_results = list<pair<ScoreBoardEntry, gameHistory>>();
    for (auto const& score_board_entry : entries)
    {
        gameHistory ge_partial_sum(0, 0);
        auto add_game_histories = [](gameHistory gh1, gameHistory gh2) {
            gameHistory ret_gh(0, 0);
            ret_gh.pts_for = gh1.pts_for + gh2.pts_for;
            ret_gh.pts_against = gh1.pts_against + gh2.pts_against;
            ret_gh.loss = gh1.loss + gh2.loss;
            ret_gh.win = gh1.win + gh2.win;
            return ret_gh;
        };

        auto it = score_board_entry.history.begin();
        for (int i = 0; i < min_history_len; i++)
        {
            ge_partial_sum = add_game_histories(ge_partial_sum, *it);
            ++it;
        }
        partial_results.push_back(pair<ScoreBoardEntry, gameHistory>(score_board_entry, ge_partial_sum));
    }
    partial_results.sort([](pair<ScoreBoardEntry, gameHistory> p1, pair<ScoreBoardEntry, gameHistory> p2) {
        return gameHistory::compare(p1.second, p2.second); }
    );

    int j = 0;
    cout << "#       Team Name               Wins    Losses  %       Pts For Pts Against" << endl;
    for ( auto it = partial_results.begin(); it != partial_results.end(); ++it)
    {
        std::stringstream fmt;
        fmt << j++ << ".\t" << it->first.algoName /*todo: change to name*/
            << '\t' << it->second.win << '\t'
            << it->second.loss << '\t'
            << it->second.precentage() << '\t'
            << it->second.pts_for << '\t'
            << it->second.pts_against;
        cout << fmt.str() << endl;
    }
}
