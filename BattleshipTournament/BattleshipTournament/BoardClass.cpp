#include "BoardClass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iso646.h>
#include "Utils.h"
#include <regex>

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

/*In the game, indexing starts at 1*/
char Board::charAt(Coordinate c) const
{
	if (isInBoard(c))
		return data_[coordToDataIndex(c)];
	throw std::out_of_range("Trying to access invalid index in the board.");
}

//non-default ctor
Board::Board(int rows, int columns, int depth) : BoardData(), data_(make_unique<char[]>(rows * columns * depth))
{
	 _rows = rows;
	 _cols = columns;
	 _depth = depth;
	DEBUG("non-default Board ctor activated - board with only spaces created");
	memset(data_.get(), ' ', rows*columns*depth);
}

/*Board::Board(const char** board, int rows, int columns, int depth) : BoardData(), data_(new char[rows * columns * depth])
{
	_rows = rows;
	_cols = columns;
	_depth = depth;
	DEBUG("non-default Board ctor activated");
	Coordinate c(rows, columns, depth);
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= columns; j++)
		{
			setSlot(c, board[i-1][j-1]);
		}
	}
}*/


void Board::readDimensionsFromFile(bool& is_valid, std::string line)
{
	if (std::regex_match(line, std::regex("[0-9]+[x][0-9]+[x][0-9](\\s*)")))
	{
		stringstream stream(line);
		int num;
		char ch;
		vector<int> ints;
		while (stream >> num)
		{
			if (stream >> ch)
				ints.push_back(num);
			else
				ints.push_back(num);
		}
		_cols = ints[0];
		_rows = ints[1];
		_depth = ints[2];
	}
	else				
		is_valid = false;
}

bool Board::SetBoardFromFile(const char* path)
{
	bool is_valid = true;
	std::ifstream infile(path);
	std::string line;	
	int row = 1, column = 1, depth = 1;
	if (infile.is_open())
	{
		// read dimenstions
		if (safeGetline(infile, line))
		{
			readDimensionsFromFile(is_valid, line);
		}
		
		//read 2-D boards
		while (is_valid and safeGetline(infile, line) && depth <= _depth)
		{
			if (!line.empty())
			{
				while (is_valid and safeGetline(infile, line) and row <= _rows)
				{
					std::stringstream ss(line);
					while (column < static_cast<int>(line.size()) and column <= _cols)
					{
						if (isLegalBoardElement(ss.peek()))
						{
							data_[coordToDataIndex(row, column,depth)] = ss.peek();
							ss.ignore();
						}
						else
							ss.ignore();
						column++;
					}
					row++;
				}
				depth++;
			}
			else
			{
				is_valid = false;
				break;
			}
		}
		infile.close();
		return is_valid;
	}
	DEBUG("SetBoardFromFile: Unable to open file");
	return false;
}

//dtor
Board::~Board() = default;

//copy ctor
Board::Board(const Board &brd) : data_(make_unique<char[]>(brd._rows * brd._cols * brd._depth))
{
	_rows = brd._rows;
	_cols = brd._cols;
	_depth = brd._depth;
	DEBUG("copy Board ctor activated");
	memcpy(data_.get(), brd.data_.get(), brd._rows * brd._cols);
}

Board& Board::operator=(const Board& other)
{
	DEBUG("operator= of Board was called");
	if (this != &other)
	{
		if (_cols*_rows*_depth != other._cols*other._rows * other._depth) //bummer
		{
			data_.reset();
			data_ = make_unique<char[]>(other._rows * other._cols * other._depth);
		}
		_cols = other._cols;
		_rows = other._rows;
		_depth = other._depth;
		memcpy(data_.get(), other.data_.get(), _rows*_cols*_depth);
	}
	//self assignment
	return *this;
}


/*TODO In the game, indexing starts at 1*/
char Board::operator()(Coordinate crd) const
{
    crd.row -= 1;
    crd.col -= 1;
    crd.depth -= 1;

    if (-1 < crd.row   && crd.row < _rows    &&
        -1 < crd.col   && crd.col < _cols &&
        -1 < crd.depth && crd.depth < _depth)
    {
        
        return data_[coordToDataIndex(crd)];
    }
    throw std::out_of_range("Trying to access invalid index in the Board.");
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

int Board::coordToDataIndex(Coordinate c) const
{
	int row = c.row - 1;
	int col = c.col - 1;
	int depth = c.depth - 1;
	return row*_cols*_depth + col*_depth + depth;
}

int Board::coordToDataIndex(int row, int col, int depth) const
{
	return coordToDataIndex(Coordinate(row, col, depth));
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

void Board::setSlot(Coordinate c, char marineObject)
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
		if (isInBoard(c))
		{
			data_[coordToDataIndex(c)] = marineObject;
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

/* OBSOLETE - user rows() instead */
/*int Board::getNumOfRows() const
{
	return _rows;
}*/

/* OBSOLETE - user cols() instead */
/*int Board::getNumOfCols() const
{
	return _cols;
}*/

bool Board::isInBoard(Coordinate c) const
{
	return c.row >= 1 and c.row <= _rows and c.col >= 1 and c.col <= _cols and c.depth >= 1 and c.depth <= _depth;			
}

bool Board::isInBoard(int row, int col, int depth) const
{
	return isInBoard(Coordinate(row, col, depth));
}


/* OBSOLETE */
/*std::ostream& operator<<(std::ostream &strm, const Board &brd) {
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
}*/


void Board::revealSurroundings(int row, int col, int depth , char ship_char, Board &board, vector<Coordinate> &coords)
{
	revealSurroundings(Coordinate(row, col, depth), ship_char, board, coords);
}


void Board::revealSurroundings(Coordinate c, char ship_char, Board &board, vector<Coordinate> &coords) 
{
	if (board.charAt(c) == ship_char)
	{
		board.setSlot(c, Board::SEA);
		coords.push_back(c);
		int rows[2] = { c.row + 1, c.row - 1 };
		int cols[2] = { c.col + 1, c.col - 1 };
		int depths[2] = { c.depth + 1, c.depth - 1 };
		for (int i = 0; i < 2; i++)
		{
			if (board.isInBoard(rows[i], c.col, c.depth))
				revealSurroundings(rows[i], c.col, c.depth, ship_char, board, coords);
			if (board.isInBoard(c.row, cols[i], c.depth))
				revealSurroundings(c.row, cols[i], c.depth, ship_char, board, coords);
			if (board.isInBoard(c.row, c.col,depths[i]))
				revealSurroundings(c.row, c.col, depths[i], ship_char, board, coords);
		}
	}
}


void Board::findShips(int player_id, vector<Ship>& ships) const
{
	Board copiedBoard(*this);
	for (int i = 1; i <= copiedBoard.rows(); i++)
	{
		for (int j = 1; j <= copiedBoard.cols(); j++)
		{
			for (int k = 1; j <= copiedBoard.depth(); k++)
			{
				if (Board::isUserShip(player_id, copiedBoard.charAt(Coordinate(i,j,k)))) //if ship found
				{
					vector<Coordinate> coords = vector<Coordinate>();
					char ship_char = copiedBoard.charAt(Coordinate(i, j, k));
					Ship::ship_type ship_type = static_cast<Ship::ship_type>(copiedBoard.charAt(Coordinate(i, j, k)));
					revealSurroundings(Coordinate(i, j, k), ship_char, copiedBoard, coords);
					ships.push_back(Ship(ship_type, &coords));
				}
			}
		}
	}

}

/* OBSOLETE */
/*pair<int,int> Board::getNextCoord(int row, int col) const
{
	if (row == this->_rows && col == this->_cols)
		return make_pair(1, 1);
	if (col == this->_cols)
		return make_pair(row + 1, 1);
	return make_pair(row, col + 1);
}*/

// https://stackoverflow.com/questions/16011113/getter-and-setter-for-unique-ptr-object-dependency-injection

