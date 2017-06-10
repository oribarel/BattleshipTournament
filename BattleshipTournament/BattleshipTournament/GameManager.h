#pragma once
#include "IBattleshipGameAlgo.h"
#include <queue>
#include "BoardClass.h"
#include "Utils.h"
#include "TournamentManager.h"

typedef struct PlayerData
{
	unique_ptr<IBattleshipGameAlgo> algo;
	char id;
	int score;
	vector<Ship> ships;
	int color;
} PlayerData;

class GameManager
{
public:

	//-- ctors and dtors
    GameManager(gameEntry ge, const TournamentManager& tm);
    GameManager(GameManager & other) = delete;
	GameManager& GameManager::operator=(GameManager& other) const = delete;

	//-- getters
	int getNumOfPlayers() const;
	int numOfValidShips(char user) const;
	Ship *getShipAtCrd(Coordinate c);

	//--queries
	bool isGameOn(bool*) const;
	bool isValidBoard() const;
	bool selfHit(PlayerData& player, Coordinate attack);

	//-- manager logic
    pair<int, int> runGame();
	void mainLoopEndOfGamePrint() const;
	void mainLoop();

	//-- utils
	unique_ptr<Board> getBoardOfPlayer(int player_id) const;
	bool validate_same_ships_quantities() const;
	bool validate_ships_shape(int player_id) const;
	bool validate_no_adjacent_ships() const;
	void notify_players(int currPlayerInx, Coordinate attack, const Ship *shipPtr, bool is_hit = true) const;
	void make_hit(int currPlayerInx, Coordinate attack, bool is_self_hit = false);
	static bool allSunk(const vector<Ship>& ships);

	static const int number_of_players = 2;
	static const int PLAYER_A = 0;
	static const int PLAYER_B = 1;
	static const int NUMBER_OF_SHIP_TYPES = 4;

    ///////////////////////////////////////
    ///    Delete or move! 
    ///////////////////////////////////////
	static bool find_dll(string dir_path, int player_id, string& dll);
    bool initialize(int argc, char *argv[]);
	bool initialize_board(string file_board);
	bool initialize_player(string dir_path, int player_id);
	bool initialize_players(string dir_path);
	static pair<bool, string> parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir);

private:
	int				currPlayerInx;
	int				numOfPlayers;
	PlayerData		players[number_of_players];
	Board			brd;


};
