#pragma once
#include "IBattleshipGameAlgo.h"
#include <queue>
#include "BoardClass.h"
#include "Utils.h"

typedef struct PlayerData
{
	IBattleshipGameAlgo *algo;
	char id;
	int score;
	vector<Ship> ships;
	int color;

} PlayerData;
class GameManager
{
public:

	//-- ctors and dtors
	GameManager();
	GameManager(GameManager & other);
	GameManager & operator=(GameManager & other);
	~GameManager();

	//-- getters
	int getNumOfPlayers() const;
	int numOfValidShips(char user) const;

	//--queries
	bool isGameOn(bool*) const;
	bool isValidBoard() const;
	bool GameManager::selfHit(PlayerData& player, Coordinate attack);

	Ship * getShipAtCrd(int row, int col);

	//-- manager logic
	bool initialize(int argc, char *argv[]);
	//bool GameManager::initialize_file_and_naive(int argc, char *argv[]);
	void GameManager::mainLoopEndOfGamePrint() const;
	void mainLoop();
	//void GameManager::findShips(char user, vector<Ship>& ships); 
	//void revealSurroundings(int row, int col, char ship_type, Board &brd, vector<pair<int, int>> &coords);
	void setPlayersBoards();

	//-- utils
	static bool GameManager::find_dll(string dir_path, int player_id, string& dll);
	const char** GameManager::getBoardOfPlayer(int player_id) const;
	bool GameManager::initialize_board(string file_board);
	void GameManager::free_board(const char** board) const;
	bool initialize_player(string dir_path, int player_id);
	bool GameManager::initialize_players(string dir_path);
	bool GameManager::validate_ships_shape(int player_id) const;
	bool GameManager::validate_no_adjacent_ships() const;
	void GameManager::notify_players(int currPlayerInx, Coordinate attack, const Ship *shipPtr, bool is_hit = true) const;
	void GameManager::make_hit(int currPlayerInx, Coordinate attack, bool is_self_hit = false);
	void printShipsCoordinates(char user);
	static bool allSunk(const vector<Ship>& ships);
	void GameManager::update_board_print(int player_color, Coordinate attack, int hit_color);
	pair<bool, string> GameManager::parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir);
	static const int number_of_players = 2;
	static const int PLAYER_A = 0;
	static const int PLAYER_B = 1;

private:
	int				currPlayerInx;
	int				numOfPlayers;
	PlayerData		players[number_of_players];
	Board			brd;


};
