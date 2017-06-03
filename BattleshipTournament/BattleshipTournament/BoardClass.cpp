#include "BoardClass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iso646.h>
#include "Utils.h"


using namespace std;

std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

char Board::charAt(Coordinate c) const
{
	return '\0';
}

//non-default ctor
Board::Board(int rows, int columns, int depth) : BoardData(), data_(new char[rows * columns])
{
	 _rows = rows;
	 _cols = columns;
	 _depth = depth;
	DEBUG("non-default Board ctor activated - board with only spaces created");
	memset(data_, ' ', rows*columns);
}

Board::Board(const char** board, int rows, int columns, int depth) : BoardData(), data_(new char[rows * columns])
{
	_rows = rows;
	_cols = columns;
	_depth = depth;
	DEBUG("non-default Board ctor activated");
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= columns; j++)
		{
			setSlot(i , j, board[i-1][j-1]);
		}
	}
}


bool Board::SetBoardFromFile(const char* path)
{
	std::ifstream infile(path);
	std::string line;
	
	int row = 0;
	if (infile.is_open())
	{
		while (safeGetline(infile, line) && row < _rows)
		{
			std::stringstream ss(line);
			int column = 0;
			while (column < static_cast<int>(line.size()) && column < _cols)
			{
				if (isLegalBoardElement(ss.peek()))
				{
					data_[_cols*row + column] = ss.peek();
					ss.ignore();
				}
				else
					ss.ignore();
				column++;
			}
			row++;
		}
		infile.close();
		return true;
	}

	DEBUG("SetBoardFromFile: Unable to open file");
	return false;

}

//dtor
Board::~Board()
{
	DEBUG("Board dtor activated");
	delete[] data_;
}

//copy ctor
Board::Board(const Board &brd) : data_(new char[brd._rows * brd._cols])
{
	_rows = brd._rows;
	_cols = brd._cols;
	_depth = brd._depth;
	DEBUG("copy Board ctor activated");
	memcpy(data_, brd.data_, brd._rows * brd._cols);
}

Board& Board::operator=(const Board& other)
{
	DEBUG("operator= of Board was called");
	if (this != &other)
	{
		if (_cols*_rows != other._cols*other._rows) //bummer
		{
			delete[] data_;
			data_ = new char[other._cols*other._rows];
		}
		_cols = other._cols;
		_rows = other._rows;
		memcpy(data_, other.data_, _rows*_cols);
	}
	//self assignment
	return *this;
}


/*In the game, indexing starts at 1*/
char Board::operator()(int row, int column) const
{
	row -= 1;
	column -= 1;
	if (-1 < row && row < _rows && -1 < column && column < _cols)
	{
		return data_[_cols*row + column];
	}
	throw std::out_of_range("Trying to access invalid index in the board.");
}

bool Board::isLegalBoardElement(char marineObject)
{
	if (marineObject == Board::SEA ||
		marineObject == Board::A_BOAT ||
		marineObject == Board::A_SATIL ||
		marineObject == Board::A_SUBMARINE ||
		marineObject == Board::A_DESTROYER ||
		marineObject == Board::B_BOAT ||
		marineObject == Board::B_SATIL ||
		marineObject == Board::B_SUBMARINE ||
		marineObject == Board::B_DESTROYER)
	{
		return true;
	}
	return false;
}

//user should be 'A' or 'B'
bool Board::isUserShip(int user_id, char marineObject)
{
	if (user_id == PLAYER_A)
		return isAShip(marineObject);
	else if (user_id == PLAYER_B)
		return isBShip(marineObject);
	else
		throw std::logic_error("Invalid User");
}
bool Board::isAShip(char marineObject)
{
	if (marineObject == Board::A_BOAT ||
		marineObject == Board::A_SATIL ||
		marineObject == Board::A_SUBMARINE ||
		marineObject == Board::A_DESTROYER)
	{
		return true;
	}
	return false;
}

bool Board::isBShip(char marineObject)
{
	if (marineObject == Board::B_BOAT ||
		marineObject == Board::B_SATIL ||
		marineObject == Board::B_SUBMARINE ||
		marineObject == Board::B_DESTROYER)
	{
		return true;
	}
	return false;
}

void Board::setSlot(int row, int col, char marineObject)
{
	if (marineObject == Board::SEA         ||
		marineObject == Board::A_BOAT	   ||
		marineObject == Board::A_SATIL	   ||
		marineObject == Board::A_SUBMARINE ||
		marineObject == Board::A_DESTROYER ||
		marineObject == Board::B_BOAT	   ||
		marineObject == Board::B_SATIL	   ||
		marineObject == Board::B_SUBMARINE ||
		marineObject == Board::B_DESTROYER)
	{
		if (isInBoard(row,col))
		{
			row -= 1;
			col -= 1;
			data_[row*_cols + col] = marineObject;
		}
		else
		{
			throw std::out_of_range("Indices out of range of the board!");
		}
	}
	else
	{
		throw std::logic_error("Invalid Marine Object!");
	}
}

int Board::getNumOfRows() const
{
	return _rows;
}

int Board::getNumOfCols() const
{
	return _cols;
}

bool Board::isInBoard(int row, int col) const
{
	if (row >=1 and row <= _rows and col >= 1 and col <= _cols)
		return true;
	return false;
}

bool Board::isInBoard(pair<int, int> crd) const
{
    return isInBoard(crd.first, crd.second);
}


std::ostream& operator<<(std::ostream &strm, const Board &brd) {
	// making columns
	strm << "    ";
	for(int i = 1 ; i <= brd._cols ; i++)
		strm << i << " ";
	strm << endl;

	for (int i = 1; i <= brd._rows; i++)
	{
		if (i == 10)
			strm << i << "  ";
		else
			strm << i << "   ";
		for(int j = 1; j <= brd._cols; j++)
		{
			if (Board::isAShip(brd(i, j)))
				Utils::setcolor(Utils::MAGNETA_COLOR);
			else if(Board::isBShip(brd(i, j)))
				Utils::setcolor(Utils::GREEN_COLOR);
			strm << brd(i,j) << " ";
			Utils::setcolor(Utils::WHITE_COLOR);
		}
		strm << endl;
	}
	return strm;
}



void Board::revealSurroundings(int row, int col, char ship_char, Board &board, vector<pair<int, int>> &coords) const
{
	if (board(row, col) == ship_char)
	{
		board.setSlot(row, col, Board::SEA);
		coords.push_back(make_pair(row, col));
		int rows[2] = { row + 1, row - 1 };
		int cols[2] = { col + 1, col - 1 };
		for (int i = 0; i < 2; i++)
		{
			if (board.isInBoard(row, cols[i]))
				revealSurroundings(row, cols[i], ship_char, board, coords);
			if (board.isInBoard(rows[i], col))
				revealSurroundings(rows[i], col, ship_char, board, coords);
		}
	}
}


void Board::findShips(int player_id, vector<Ship>& ships) const
{
	Board copiedBoard(*this);
	for (int i = 1; i <= copiedBoard.getNumOfRows(); i++)
	{
		for (int j = 1; j <= copiedBoard.getNumOfCols(); j++)
		{
			if (Board::isUserShip(player_id, copiedBoard(i, j))) //if ship found
			{
				vector<pair<int, int>> coords = vector<pair<int, int>>();
				char ship_char = copiedBoard(i, j);
				Ship::ship_type ship_type = static_cast<Ship::ship_type>(copiedBoard(i, j));
				revealSurroundings(i, j, ship_char, copiedBoard, coords);
				ships.push_back(Ship(ship_type, &coords));
			}
		}
	}

}

pair<int,int> Board::getNextCoord(int row, int col) const
{
	if (row == this->_rows && col == this->_cols)
		return make_pair(1, 1);
	if (col == this->_cols)
		return make_pair(row + 1, 1);
	return make_pair(row, col + 1);
}

const char* Board::getData() const
{
	return data_;
}
