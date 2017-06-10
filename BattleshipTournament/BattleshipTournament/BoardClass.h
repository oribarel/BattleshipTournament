#pragma once
#include <string>
#include <vector>
#include "Ship.h"
#include "IBattleshipGameAlgo.h"
#include <memory>

using namespace std;

istream& safeGetline(istream& is, string& t);

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
	Board::Board(int rows, int columns, int depth);//non-default
	Board(const Board&); //copy ctor
	~Board();
	
	
	//-- inherited methods
	char charAt(Coordinate c) const override;
	
	void readDimensionsFromFile(bool& is_valid, std::string line);
	Board& operator=(const Board&);
	char operator()(Coordinate crd) const;
	static void revealSurroundings(int row, int col, int depth, char ship_char, Board &board, vector<Coordinate> &coords);
	static void revealSurroundings(Coordinate c, char ship_char, Board &board, vector<Coordinate> &coords);
	void findShips(int player_id, vector<Ship>& ships) const;

	bool isInBoard(Coordinate c) const;
	bool isInBoard(int row, int col, int depth) const;

	static bool isLegalBoardElement(char c);
	static bool isUserShip(int user_id, char marineObject);
	static bool isAShip(char marineObject);
	static bool isBShip(char marineObject);

	void setSlot(Coordinate c, char marineObject);
	bool SetBoardFromFile(const char* path);



	//pair<int, int> Board::getNextCoord(int row, int col) const;

	int coordToDataIndex(Coordinate c) const;
	int coordToDataIndex(int row, int col, int depth) const;


private:
    //todo: change to char vector
	unique_ptr<char[]>  data_;

};