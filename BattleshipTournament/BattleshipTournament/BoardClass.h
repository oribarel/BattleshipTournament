#pragma once
#include <string>
#include <vector>
#include "Ship.h"
#include "IBattleshipGameAlgo.h"

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
	Board::Board(const char** board, int rows, int columns, int depth); //non-default2
	Board(const Board&); //copy ctor
	Board& operator=(const Board&);
	char operator()(int, int) const;
	friend std::ostream& operator<<(std::ostream &strm, const Board &brd);
	~Board();

	void Board::revealSurroundings(int row, int col, char ship_char, Board &board, vector<pair<int, int>> &coords) const;
	void Board::findShips(int player_id, vector<Ship>& ships) const;

	bool Board::isInBoard(int row, int col) const;
    bool Board::isInBoard(pair<int,int> crd) const;

	static bool isLegalBoardElement(char c);
	static bool Board::isUserShip(int user_id, char marineObject);
	static bool isAShip(char marineObject);
	static bool isBShip(char marineObject);

	void setSlot(int row, int col, char marineObject);
	bool SetBoardFromFile(const char* path);

	int getNumOfRows() const;
	int getNumOfCols() const;
	const char* Board::getData() const;

	pair<int, int> Board::getNextCoord(int row, int col) const;

private:
	char *data_;

};