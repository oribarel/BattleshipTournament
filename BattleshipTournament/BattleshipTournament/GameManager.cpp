#include "GameManager.h"
#include "Ship.h"
#include <iso646.h>
#include <direct.h>
#include <map>

using namespace std;



GameManager::GameManager(GameManager& other)
{
	DEBUG("Oh no! copy ctor of GameManager was called!");
}

GameManager& GameManager::operator=(GameManager& other) const
{
	DEBUG("Oh no, operator= of GameManager was used!");
	return other;
}


bool GameManager::initialize_board(string file_board)
{
	bool set_boards_sucess = true;
	//clear terminal
	//system("cls");
	// set board using file
	set_boards_sucess = brd.SetBoardFromFile((file_board).c_str());
	if (!set_boards_sucess)
	{
		std::cout << "Error while open board file" << std::endl;
		return false;
	}
	//DEBUG(*brd);
	//if (!Utils::get_quiet())
	//	cout << brd << endl;

	// find ships
	this->brd.findShips(PLAYER_A, players[PLAYER_A].ships);
	this->brd.findShips(PLAYER_B, players[PLAYER_B].ships);


	// check board validity
	if (!this->isValidBoard())
		return false;
	DEBUG("*** board is valid ***");

	// printing coordinates of ships:
	//this->printShipsCoordinates('A');
	//this->printShipsCoordinates('B');
	return true;
}

/* OBSOLETE */
/*void GameManager::free_board(const char** board) const
{
	for (int i = 0; i < brd.rows(); i++)
		delete[] board[i];
	delete[] board;
}*/

bool GameManager::find_dll(string dir_path, int player_id, string& dll)
{
	string path = dir_path;
	if (dir_path.compare(".") == 0)
		path = "Working Directory";
	string s_dll = "\\*.dll"; // only .dll endings
	bool file_found = true;
	dll = Utils::find_file(dir_path + s_dll, player_id, true, file_found);
	if (!file_found)
	{
		cout << "Missing an algorithm (dll) file looking in path: " << path << endl;
		return false;
	}
	return true;
}

bool GameManager::initialize_player(string dir_path, int player_id)
{

	string dll;

	/* find DLLs*/
	bool retVal = find_dll(dir_path, player_id, dll);
	if (!retVal)
		return false;

	// Load dynamic library
	char full_path[_MAX_PATH];
	string full_dll_path = dir_path + "\\" + dll;
	_fullpath(full_path, full_dll_path.c_str(), _MAX_PATH);
	HINSTANCE hDll = LoadLibraryA(full_path); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll)
	{
		std::cout << "Cannot load dll: " << full_path <<  std::endl;
		return false;
	}

	/* create A instance*/
	// Get function pointer
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoFunc)();
	GetAlgoFunc getFunc = reinterpret_cast<GetAlgoFunc>(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getFunc)
	{
		std::cout << "Cannot load dll: " << full_path << std::endl;
		return false;
	}
	players[player_id].algo = getFunc();
	auto board = getBoardOfPlayer(player_id);
	players[player_id].algo->setBoard(*board);
	/* TODO: check this */
	players[player_id].algo->setPlayer(player_id);
	if (!retVal)
	{
		std::cout << "Algorithm initialization failed for dll: " << full_path << std::endl;
		return false;
	}

	// cleanup
	//free_board(board);

	return retVal;
}

bool GameManager::initialize_players(string dir_path)
{
	return initialize_player(dir_path, PLAYER_A) and initialize_player(dir_path, PLAYER_B);
}


pair<bool, string> GameManager::parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir)
{
	string dir_path = ".";
	is_working_dir = (argc == 1);
	/* parsing command line arguments */
	for (int i = 1; i < argc; i++)
	{
		string argument = argv[i];
		if (argument == "-quiet")
		{
			Utils::set_quiet(true);
			if (i == 1)
				is_working_dir = true;
		}
		if (argument == "-delay")
		{
			Utils::set_delay(atoi(argv[i + 1]));
			if (i == 1)
				is_working_dir = true;
		}
	}
	if (!is_working_dir)
	{
		if (!Utils::is_valid_dir_path(argv[1]))
		{
			cout << "Wrong path: " << argv[1] << endl; //ReqPrnt
			return make_pair(false, dir_path);
		}
		dir_path = string(argv[1]);
	}
	return make_pair(true, dir_path);
}
bool GameManager::initialize(int argc, char *argv[])
{
	bool find_board = false, find_a = false, find_b = false, is_working_dir = false, find_path = true;
	string dir_path = ".", file_board, file_a, file_b, find_file_ret_val;
	pair<bool, string> parser_ret_val = parse_command_line_arguments(argc, argv, is_working_dir);
	char cCurrentPath[FILENAME_MAX];
	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		cout << "Error: can't extract current directory path" << endl;
		return false;

	}
	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	dir_path = parser_ret_val.second;
	dir_path = is_working_dir ? string(cCurrentPath) : string(argv[1]);
	if (parser_ret_val.first == false)
		return false;
	/* find files */
	find_file_ret_val = Utils::find_path(dir_path + "\\*.sboard", find_path);
	if (find_path)
	{
		file_board = dir_path + "\\" + find_file_ret_val;
		find_board = true;
	}

	if (!find_board)
	{
		cout << "Missing board file (*.sboard) looking in path: " << dir_path << endl; //ReqPrnt
		string dummy_str;
		/* even if dll is missing for both players an error message will be printed only once, intentionally*/
		find_dll(dir_path, PLAYER_A, dummy_str);
	}
	else
	{
		if (initialize_board(file_board) == false)
			return false;
		return initialize_players(dir_path);
	}
	return false;
}


GameManager::GameManager() : currPlayerInx(0),
numOfPlayers(number_of_players), brd(Board())
{
	PlayerData player1;
	PlayerData player2;

	player1.score = 0;
	player2.score = 0;

	player1.id = 'A';
	player2.id = 'B';

	player1.algo = nullptr;
	player2.algo = nullptr;

	//player1.color = Utils::MAGNETA_COLOR;
	//player2.color = Utils::GREEN_COLOR;

	players[PLAYER_A] = player1;
	players[PLAYER_B] = player2;	
}

GameManager::~GameManager()
{
	for(PlayerData player: players)
	{
		delete player.algo;
	}
}

//user should be 'A' or 'B'
int GameManager::numOfValidShips(char user) const
{
	int cnt = 0;
	const vector<Ship> *ships;
	if (user == 'A')
		ships = &players[PLAYER_A].ships;
	else
		ships = &players[PLAYER_B].ships;

	for (vector<Ship>::const_iterator it = ships->begin(); it != ships->end(); ++it)
		if (it->getValid())
			cnt++;
	return cnt;
}


bool GameManager::validate_same_ships_quantities() const
{
	map<Ship::ship_type, int> ship_types_A = { {Ship::ship_type::A_BOAT,0}, {Ship::ship_type::A_SATIL,0}, {Ship::ship_type::A_SUBMARINE,0}, {Ship::ship_type::A_DESTROYER,0} };
	map<Ship::ship_type, int> ship_types_B = { { Ship::ship_type::B_BOAT,0 },{ Ship::ship_type::B_SATIL,0 },{ Ship::ship_type::B_SUBMARINE,0 },{ Ship::ship_type::B_DESTROYER,0 } };

	/* build histogram for A ships */
	for (auto it = players[PLAYER_A].ships.begin(); it != players[PLAYER_A].ships.end(); ++it)
	{
		ship_types_A[it->getType()]++;
	}
	/* build histogram for B ships */
	for (auto it = players[PLAYER_B].ships.begin(); it != players[PLAYER_B].ships.end(); ++it)
	{
		ship_types_A[it->getType()]++;
	}
	/* compare histograms */
	return ship_types_A[Ship::ship_type::A_BOAT] == ship_types_B[Ship::ship_type::B_BOAT] and
		ship_types_A[Ship::ship_type::A_SATIL] == ship_types_B[Ship::ship_type::B_SATIL] and
		ship_types_A[Ship::ship_type::A_SUBMARINE] == ship_types_B[Ship::ship_type::B_SUBMARINE] and
		ship_types_A[Ship::ship_type::A_DESTROYER] == ship_types_B[Ship::ship_type::B_DESTROYER];


}

bool GameManager::validate_ships_shape(int player_id) const
{
	Ship::ship_type ship_types_A[NUMBER_OF_SHIP_TYPES] = { Ship::ship_type::A_BOAT, Ship::ship_type::A_SATIL, Ship::ship_type::A_SUBMARINE, Ship::ship_type::A_DESTROYER };
	Ship::ship_type ship_types_B[NUMBER_OF_SHIP_TYPES] = { Ship::ship_type::B_BOAT, Ship::ship_type::B_SATIL, Ship::ship_type::B_SUBMARINE, Ship::ship_type::B_DESTROYER };
	Ship::ship_type *ship_types;
	if (player_id == PLAYER_A)
		ship_types = ship_types_A;
	else
		ship_types = ship_types_B;
	bool is_valid = true;
	// validate shape of A ships
	for (int i = 0; i < NUMBER_OF_SHIP_TYPES; i++)
	{
		for (vector<Ship>::const_iterator it = players[player_id].ships.begin(); it != players[player_id].ships.end(); ++it)
		{
			if (it->getType() == ship_types[i] && !it->getValid())
			{
				//Requirement print
				if (player_id == PLAYER_A)
					cout << "Wrong size or shape for ship " << static_cast<char> (ship_types_A[i]) << " for player A" << endl;
				else
					cout << "Wrong size or shape for ship " << static_cast<char> (ship_types_B[i]) << " for player B" << endl;
				is_valid = false;
				break;
			}
		}
	}
	return is_valid;
}

bool GameManager::validate_no_adjacent_ships() const
{
	vector<Ship> all_ships;
	all_ships.insert(all_ships.end(), players[PLAYER_A].ships.begin(), players[PLAYER_A].ships.end());
	all_ships.insert(all_ships.end(), players[PLAYER_B].ships.begin(), players[PLAYER_B].ships.end());
	for (int i = 0; i < static_cast<int>(all_ships.size()) - 1; i++)
	{
		for (unsigned int j = i + 1; j < all_ships.size(); j++)
		{
			if (all_ships[i].isAdjacentShips(all_ships[j]))
			{
				//Requirement print
				cout << "Adjacent Ships on Board" << endl;
				return false;
			}
		}
	}
	return true;
}
bool GameManager::isValidBoard() const
{
	// validate shape of ships
	bool is_valid = validate_ships_shape(PLAYER_A) && validate_ships_shape(PLAYER_B);
	// validate number of A ships
	//int num_of_a_ships = numOfValidShips('A');
	// validate number of B ships
	//int num_of_b_ships = numOfValidShips('B');
	// validate no adjacent ships
	is_valid &= validate_no_adjacent_ships();
	// validate same quantities of ships
	is_valid &= validate_same_ships_quantities();
	return is_valid;
}


bool GameManager::isGameOn(bool *outOfAttacks) const
{
	if (outOfAttacks[0] && outOfAttacks[1])
		return false;

	if (allSunk(players[PLAYER_A].ships) || allSunk(players[PLAYER_B].ships))
		return false;

	return true;
}

bool GameManager::allSunk(const vector<Ship>& ships)
{
	for (vector<Ship>::const_iterator it = ships.begin(); it != ships.end(); ++it)
	{
		if (!it->isSunk())
			return false;
	}
	return true;
}

/* OBSOLETE */
/*void GameManager::update_board_print(int player_color, Coordinate attack, int hit_color)
{
	Board& board = brd;
	COORD hit_coord;
	hit_coord.Y = attack.row;
	hit_coord.X = attack.col;
	if (board(attack.row, attack.col) == Board::SEA)
		Utils::updateBoardPrint(player_color, hit_coord, Board::SEA, hit_color);
	else
		Utils::updateBoardPrint(player_color, hit_coord, Utils::HIT_SIGN, hit_color);
}*/

void GameManager::notify_players(int currPlayerInx, Coordinate attack, const Ship *shipPtr, bool is_hit) const
{
	if(is_hit)
	{
		players[PLAYER_A].algo->notifyOnAttackResult(currPlayerInx, attack, shipPtr->isSunk() ? AttackResult::Sink : AttackResult::Hit);
		players[PLAYER_B].algo->notifyOnAttackResult(currPlayerInx, attack, shipPtr->isSunk() ? AttackResult::Sink : AttackResult::Hit);
	}
	else
	{
		players[PLAYER_A].algo->notifyOnAttackResult(currPlayerInx, attack, AttackResult::Miss);
		players[PLAYER_B].algo->notifyOnAttackResult(currPlayerInx, attack, AttackResult::Miss);
	}
}

void GameManager::mainLoopEndOfGamePrint() const
{
	if (allSunk(players[PLAYER_A].ships))
		cout << "Player B won" << endl; //Requirement print
	if (allSunk((players[PLAYER_B].ships)))
		cout << "Player A won" << endl; //Requirement print
	cout << "Points:" << endl; 	//Requirement print
	cout << "Player A: " << players[PLAYER_A].score << endl; //Requirement print
	cout << "Player B: " << players[PLAYER_B].score << endl; //Requirement print
}
void GameManager::make_hit(int currPlayerInx, Coordinate attack, bool is_self_hit)
{
	Ship *shipPtr = getShipAtCrd(attack); //player hits itself
	shipPtr->hitAt(attack);
	//update_board_print(players[currPlayerInx % 2].color, attack, is_self_hit ? players[currPlayerInx % 2].color: players[(currPlayerInx + 1) % 2].color); //update board print				
	if (shipPtr->isSunk()) //if ship sinks grant points to enemy
	{
		if (is_self_hit)
			players[(currPlayerInx + 1) % 2].score += shipPtr->getScoreForSinking();
		else
			players[currPlayerInx].score += shipPtr->getScoreForSinking();
	}
	notify_players(currPlayerInx, attack, shipPtr);	// notify players			
}

void GameManager::mainLoop()
{
	Utils::ShowConsoleCursor(false);
	Board& board = brd;
	bool OutOfAttacks[2] = { false, false };
	while (isGameOn(OutOfAttacks))
	{
		PlayerData& currPlayer = players[currPlayerInx];
		do
		{
			auto attack = currPlayer.algo->attack();
			DEBUG("player " << currPlayerInx << " attack is "  << attack.row << ", " << attack.col ", " << attack.depth);
			DEBUG("Points:");
			DEBUG("Player A: " << players[PLAYER_A].score);
			DEBUG("Player B: " << players[PLAYER_B].score);
			if (attack.row == -1 and attack.col == -1 and attack.depth == -1) //no more attacks for this player
			{
				OutOfAttacks[currPlayerInx] = true;				
				currPlayerInx = (currPlayerInx + 1) % 2;
				break;
			}
			if (board.charAt(attack) == Board::SEA || getShipAtCrd(attack)->isSunk())
			{	
				//int hit_color = board.charAt(attack) == Board::SEA || getShipAtCrd(attack)->isAShip() ? players[PLAYER_A].color : players[PLAYER_B].color;
				//update_board_print(players[(currPlayerInx) % 2].color, attack, hit_color); //update board print			
				notify_players(currPlayerInx, attack, nullptr, false); //notify players				
				currPlayerInx = (currPlayerInx + 1) % 2; //nothing happens and the turn passes
				break;
			}
			else if (selfHit(currPlayer, attack))
			{
				make_hit(currPlayerInx, attack, true);
				currPlayerInx = (currPlayerInx + 1) % 2; //pass turn
				break;
			}
			else
			{
				make_hit(currPlayerInx, attack);
				if (!isGameOn(OutOfAttacks)) //which means this hit caused victory
					break;
			}
		} while (true);
	}
	Utils::ShowConsoleCursor(true);
	mainLoopEndOfGamePrint();
}

int GameManager::getNumOfPlayers() const
{
	return numOfPlayers;
}

bool GameManager::selfHit(PlayerData& player, Coordinate attack)
{
	Board& board = brd;
	Ship *shipPtr = this->getShipAtCrd(attack);
	if (shipPtr != nullptr)
	{
		if (player.id == 'A')
			return (shipPtr->isAShip() and !shipPtr->isSunk());
		else
			return (shipPtr->isBShip() and !shipPtr->isSunk());
	}

	return false;
}

Ship *GameManager::getShipAtCrd(Coordinate c)
{
	Board& board = brd;
	vector<Ship> *ships;
	if ((board.isInBoard(c)) && (board.charAt(c) != Board::SEA))
	{
		if (isupper(board.charAt(c)))
			ships = &players[PLAYER_A].ships;
		else
			ships = &players[PLAYER_B].ships;		
		for (vector<Ship>::iterator it = ships->begin(); it != ships->end(); ++it)
		{
			if (it->containsCoord(c))
				return &(*it);
		}
	}
	return nullptr;
}


// the board that is alocated here should be freed
unique_ptr<Board> GameManager::getBoardOfPlayer(int player_id) const
{
	const int rows = brd.rows();
	const int cols = brd.cols();
	const int depths = brd.depth();

	unique_ptr<Board> boardP = make_unique<Board>(rows, cols, depths);
	for (int row = 1; row <= rows; row++)
	{
		for (int col = 1; col <= cols; col++)
		{
			for (int depth = 1; depth <= depths; depth++)
			{
				if (player_id == PLAYER_A and Board::isBShip(brd.charAt(Coordinate(row,col,depth))) or
					(player_id == PLAYER_B and Board::isAShip(brd.charAt(Coordinate(row, col, depth)))))
					boardP->setSlot(Coordinate(row, col, depth), Board::SEA);
				else
					boardP->setSlot(Coordinate(row, col, depth), brd.charAt(Coordinate(row, col, depth)));
			}
		}		
	}
	return boardP;


}

