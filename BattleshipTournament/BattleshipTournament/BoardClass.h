#pragma once
#include <string>
#include <vector>
#include "Ship.h"
#include "IBattleshipGameAlgo.h"
#include "Utils.h"

using namespace std;



std::istream& safeGetline(std::istream& is, std::string& t);

class Board : public BoardData
{
public:
	static const char SEA			= ' ';
	static const char A_BOAT		= 'B';
	static const char A_SATIL		= 'P';
	static const char A_SUBMARINE	= 'M';
	static const char A_DESTROYER	= 'D';
	static const char B_BOAT		= 'b';
	static const char B_SATIL		= 'p';
	static const char B_SUBMARINE	= 'm';
	static const char B_DESTROYER	= 'd';

	static const int PLAYER_A = 0;
	static const int PLAYER_B = 1;

	static const int DEFAULT_BOARD_WIDTH = 10;

	Board() : Board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_WIDTH) {}
	
	
	//-- inherited methods
	char charAt(Coordinate c) const override;
	
	
	
	//empty constructor
	Board::Board(int rows, int columns, int depth);//non-default
	void readDimensionsFromFile(bool& is_valid, std::string line);
	//Board::Board(const char** board, int rows, int columns, int depth); //non-default2
	Board(const Board&); //copy ctor
	Board& operator=(const Board&);
	char operator()(Coordinate crd) const;
	#ifdef _2D_
	friend std::ostream& operator<<(std::ostream &strm, const Board &brd);
	#endif
	~Board();
	static void Board::revealSurroundings(int row, int col, int depth, char ship_char, Board &board, vector<Coordinate> &coords);
	static void Board::revealSurroundings(Coordinate c, char ship_char, Board &board, vector<Coordinate> &coords);
	void Board::findShips(int player_id, vector<Ship>& ships) const;

	bool Board::isInBoard(Coordinate c) const;
	bool Board::isInBoard(int row, int col, int depth) const;

	static bool isLegalBoardElement(char c);
	static bool Board::isUserShip(int user_id, char marineObject);
	static bool isAShip(char marineObject);
	static bool isBShip(char marineObject);

	void Board::setSlot(Coordinate c, char marineObject);
	bool SetBoardFromFile(const char* path);



	//pair<int, int> Board::getNextCoord(int row, int col) const;

	int Board::coordToDataIndex(Coordinate c) const;
	int Board::coordToDataIndex(int row, int col, int depth) const;


private:
	vector<char>  data_;

};

struct BoardFullData
{
	Board brd;
	vector<Ship> shipsA;
	vector<Ship> shipsB;
	bool valid;
	BoardFullData() : brd(Board()), shipsA(vector<Ship>()), shipsB(vector<Ship>()), valid(false){}
	bool BoardFullData::initialize_board(string file_board);
	bool BoardFullData::isValidBoard() const;
	bool BoardFullData::validate_same_ships_quantities() const;
	bool BoardFullData::validate_ships_shape(int player_id) const;
	bool BoardFullData::validate_no_adjacent_ships() const;

};