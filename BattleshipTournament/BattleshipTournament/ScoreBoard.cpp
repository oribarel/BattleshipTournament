#include "ScoreBoard.h"
#include <iostream>
#include <sstream> 
#include <iomanip>

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
    auto add_game_histories = [](gameHistory gh1, gameHistory gh2) {
        gameHistory ret_gh(0, 0);
        ret_gh.pts_for = gh1.pts_for + gh2.pts_for;
        ret_gh.pts_against = gh1.pts_against + gh2.pts_against;
        ret_gh.loss = gh1.loss + gh2.loss;
        ret_gh.win = gh1.win + gh2.win;
        return ret_gh;
    };

    auto partial_results = list<pair<ScoreBoardEntry, gameHistory>>();
    for (auto const& score_board_entry : entries)
    {
        gameHistory ge_partial_sum(0, 0);
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

    const int sep = 3;
    size_t j = 1;

    size_t index_w = string("1.").length() + sep;
    size_t algo_name_w = string("Team Name").length() + sep;
    size_t win_w = string("Wins").length() + sep;
    size_t loss_w = string("Losses").length() + sep;
    size_t precentage_w = 6 + sep; //-- 6 for the precentage(max is 100.00), sep for space
    size_t pts_for_w = string("Pts For").length() + sep;
    size_t pts_against_w = string("Pts Against").length() + sep;

    for (auto it = partial_results.begin(); it != partial_results.end(); ++it)
    {
        index_w = max(index_w, std::to_string(j).length() + 1 /*for the dot*/ + sep /*for space*/);
        algo_name_w = max(algo_name_w, it->first.algoName.length() + sep);
        win_w = max(win_w, std::to_string(it->second.win).length() + sep);
        loss_w = max(loss_w, std::to_string(it->second.loss).length() + sep);
        pts_for_w = max(pts_for_w, std::to_string(it->second.pts_for).length() + sep);
        pts_against_w = max(pts_against_w, std::to_string(it->second.pts_against).length() + sep);
    }

    j = 1;
    cout << endl;
    cout << 
        "#"             << setw(index_w - string("#").length())                 << "" <<
        "Team Name"     << setw(algo_name_w - string("Team Name").length())     << "" <<
        "Wins"          << setw(win_w - string("Wins").length())                << "" <<
        "Losses"        << setw(loss_w - string("Losses").length())             << "" <<
        "%"             << setw(precentage_w - string("%").length())            << "" <<
        "Pts For"       << setw(pts_for_w - string("Pts For").length())         << "" <<
        "Pts Against"   << setw(pts_against_w - string("Pts Against").length()) << "" <<
        endl;
    cout << endl;
    for (auto it = partial_results.begin(); it != partial_results.end(); ++it)
    {
        std::stringstream fmt;
        string index = to_string(j++) + ".";
        string algoname = it->first.algoName;
        string wins = to_string(it->second.win);
        string losses = to_string(it->second.loss);
        stringstream precss;
        precss << setprecision(2) << fixed << it->second.precentage();
        string prec = precss.str();
        string forp = to_string(it->second.pts_for);
        string agap = to_string(it->second.pts_against);


        fmt <<
            index << setw(index_w - index.length())             << "" <<
            algoname << setw(algo_name_w - algoname.length())   << "" <<
            wins << setw(win_w - wins.length())                 << "" <<
            losses << setw(loss_w - losses.length())            << "" <<
            prec << setw(precentage_w - prec.length())          << "" <<
            forp << setw(pts_for_w - forp.length())             << "" <<
            agap << setw(pts_against_w - agap.length())         << "";
        cout << fmt.str() << endl;
    }
    //j = 1;
    //cout << endl;
    //cout << setw(index_w) << "#" << setw(algo_name_w) << "Team Name" <<
    //    setw(win_w) <<  "Wins" << setw(loss_w) << "Losses" <<
    //    setw(precentage_w) << "%" << setw(pts_for_w) << "Pts For" <<
    //    setw(pts_against_w) << "Pts Against" << endl;
    //cout << endl;
    //for ( auto it = partial_results.begin(); it != partial_results.end(); ++it)
    //{
    //    std::stringstream fmt;
    //    fmt << setw(index_w)        << j++
    //        << setw(algo_name_w)    << it->first.algoName
    //        << setw(win_w)          << it->second.win 
    //        << setw(loss_w)         << it->second.loss
    //        << setw(precentage_w)   << setprecision(2) << fixed << it->second.precentage()
    //        << setw(pts_for_w)      << it->second.pts_for
    //        << setw(pts_against_w)  << it->second.pts_against;
    //    cout << fmt.str() << endl;
    //}
}
