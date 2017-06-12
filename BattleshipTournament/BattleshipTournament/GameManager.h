#pragma once
#include "IBattleshipGameAlgo.h"
#include <queue>
#include "BoardClass.h"
#include "Utils.h"


struct gameEntry;
class TournamentManager;

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
	//GameManager();
	GameManager(GameManager & other) = delete;
	GameManager& GameManager::operator=(GameManager& other) const = delete;
	GameManager::GameManager(gameEntry ge, const TournamentManager& tm);

	//-- getters
	int getNumOfPlayers() const;
	int numOfValidShips(char user) const;

	//--queries
	bool isGameOn(bool*) const;
	bool isValidBoard() const;
	bool GameManager::selfHit(PlayerData& player, Coordinate attack);

	Ship *GameManager::getShipAtCrd(Coordinate c);

	//-- manager logic
	bool initialize(int argc, char *argv[]);
	void GameManager::mainLoopEndOfGamePrint() const;
	pair<int, int> GameManager::runGame();
	void mainLoop();


	//-- utils
	static bool GameManager::find_dll(string dir_path, int player_id, string& dll);
	void GameManager::getBoardOfPlayer(int player_id, Board& board) const;
	bool GameManager::initialize_board(string file_board);
	bool initialize_player(string dir_path, int player_id);
	bool GameManager::initialize_players(string dir_path);
	bool GameManager::validate_same_ships_quantities() const;
	bool GameManager::validate_ships_shape(int player_id) const;
	bool GameManager::validate_no_adjacent_ships() const;
	void GameManager::notify_players(int currPlayerInx, Coordinate attack, const Ship *shipPtr, bool is_hit = true) const;
	void GameManager::make_hit(int currPlayerInx, Coordinate attack, bool is_self_hit = false);
	static bool allSunk(const vector<Ship>& ships);
#ifdef _2D_
	void GameManager::update_board_print(int player_color, Coordinate attack, int hit_color);
#endif
	static pair<bool, string> GameManager::parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir);
	static const int number_of_players = 2;
	static const int PLAYER_A = 0;
	static const int PLAYER_B = 1;
	static const int NUMBER_OF_SHIP_TYPES = 4;

private:
	int				currPlayerInx;
	int				numOfPlayers;
	PlayerData		players[number_of_players];
	Board			brd;


};
