#include "BoardClass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iso646.h>
#include <regex>
#include <map>

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
Board::Board(int rows, int columns, int depth) : BoardData(), data_(vector<char>(rows * columns * depth,' '))
{
	 _rows = rows;
	 _cols = columns;
	 _depth = depth;
	DEBUG("non-default Board ctor activated - board with only spaces created");
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
		data_ = vector<char>(_rows * _cols * _depth, ' ');
	}
	else				
		is_valid = false;
}

bool Board::SetBoardFromFile(const char* path)
{
	bool is_valid = true, not_first_iter = false;
	std::ifstream infile(path);
	std::string line;	
	int depth = 1;
	if (infile.is_open())
	{
		// read dimenstions
		if (safeGetline(infile, line))
		{
			readDimensionsFromFile(is_valid, line);
		}
		
		//read 2-D boards
		while (is_valid and (not_first_iter or safeGetline(infile, line)) && depth <= _depth)
		{
			not_first_iter = true;
			if (line.empty())
			{
				int row = 1;
				while (is_valid and safeGetline(infile, line) and row <= _rows)
				{
					int column = 1;
					std::stringstream ss(line);
					while (column < static_cast<int>(line.size()) + 1 and column <= _cols)
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
/* OBSOLETE */
//dtor
/*Board::~Board()
{
	DEBUG("Board dtor activated");
	delete[] data_;
}*/

//copy ctor
Board::Board(const Board &brd) : data_(vector<char>(brd.data_))
{
	_rows = brd._rows;
	_cols = brd._cols;
	_depth = brd._depth;
	DEBUG("copy Board ctor activated");
}

Board& Board::operator=(const Board& other)
{
	DEBUG("operator= of Board was called");
	if (this != &other)
	{
		_cols = other._cols;
		_rows = other._rows;
		_depth = other._depth;
		data_ = vector<char>(other.data_);
	}
	//self assignment
	return *this;
}


/*TODO In the game, indexing starts at 1*/
char Board::operator()(Coordinate crd) const
{
	return data_[coordToDataIndex(crd)];
    /*crd.row -= 1;
    crd.col -= 1;
    crd.depth -= 1;

    if (-1 < crd.row   && crd.row < _rows    &&
        -1 < crd.col   && crd.col < _cols &&
        -1 < crd.depth && crd.depth < _depth)
    {
        
        return data_[coordToDataIndex(crd)];
    }
	else
		throw std::out_of_range("Trying to access invalid index in the Board.");*/
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
#ifdef _2D_
std::ostream& operator<<(std::ostream &strm, const Board &brd) {
	// making columns
	for (int k = 1; k <= brd.depth(); k++)
	{
		strm << "    ";
		for (int i = 1; i <= brd.cols(); i++)
			strm << i << " ";
		strm << endl;

		for (int i = 1; i <= brd.rows(); i++)
		{
			if (i == 10)
				strm << i << "  ";
			else
				strm << i << "   ";
			for (int j = 1; j <= brd.cols(); j++)
			{
				if (Board::isAShip(brd.charAt(Coordinate(i, j, k))))
					Utils::setcolor(Utils::MAGNETA_COLOR);
				else if (Board::isBShip(brd.charAt(Coordinate(i, j, k))))
					Utils::setcolor(Utils::GREEN_COLOR);
				strm << brd.charAt(Coordinate(i, j, k)) << " ";
				Utils::setcolor(Utils::WHITE_COLOR);
			}
			strm << endl;
		}
	}
	return strm;
}
#endif


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
	for (int k = 1; k <= copiedBoard.depth(); k++)
	{
		for (int i = 1; i <= copiedBoard.rows(); i++)
		{
			for (int j = 1; j <= copiedBoard.cols(); j++)
			{

				if (Board::isUserShip(player_id, copiedBoard.charAt(Coordinate(i, j, k)))) //if ship found
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

bool BoardFullData::initialize_board(string file_board)
{
	bool set_boards_sucess = true;
	// set board using file
	set_boards_sucess = this->brd.SetBoardFromFile((file_board).c_str());
	if (!set_boards_sucess)
	{
		std::cout << "Error while open board file" << std::endl;
		return false;
	}

	// find ships
	this->brd.findShips(Board::PLAYER_A, this->shipsA);
	this->brd.findShips(Board::PLAYER_B, this->shipsB);


	// check board validity
	if (!this->isValidBoard())
		return false;
	DEBUG("*** board is valid ***");
	this->valid = true;
	return true;
}


bool BoardFullData::isValidBoard() const
{
	// validate shape of ships
	bool is_valid = validate_ships_shape(Board::PLAYER_A) && validate_ships_shape(Board::PLAYER_B);
	// validate number of A ships
	//int num_of_a_ships = numOfValidShips('A');
	// validate number of B ships
	//int num_of_b_ships = numOfValidShips('B');
	// validate no adjacent ships
	is_valid &= validate_no_adjacent_ships();
	// validate same quantities of ships
	if (!validate_same_ships_quantities())
	{
		//TODO: print to log
	}
	return is_valid;
}



bool BoardFullData::validate_same_ships_quantities() const
{
	map<Ship::ship_type, int> ship_types_A = { { Ship::ship_type::A_BOAT,0 },{ Ship::ship_type::A_SATIL,0 },{ Ship::ship_type::A_SUBMARINE,0 },{ Ship::ship_type::A_DESTROYER,0 } };
	map<Ship::ship_type, int> ship_types_B = { { Ship::ship_type::B_BOAT,0 },{ Ship::ship_type::B_SATIL,0 },{ Ship::ship_type::B_SUBMARINE,0 },{ Ship::ship_type::B_DESTROYER,0 } };

	/* build histogram for A ships */
	for (auto it = shipsA.begin(); it != shipsA.end(); ++it)
	{
		ship_types_A[it->getType()]++;
	}
	/* build histogram for B ships */
	for (auto it = shipsB.begin(); it != shipsB.end(); ++it)
	{
		ship_types_A[it->getType()]++;
	}
	/* compare histograms */
	return ship_types_A[Ship::ship_type::A_BOAT] == ship_types_B[Ship::ship_type::B_BOAT] and
		ship_types_A[Ship::ship_type::A_SATIL] == ship_types_B[Ship::ship_type::B_SATIL] and
		ship_types_A[Ship::ship_type::A_SUBMARINE] == ship_types_B[Ship::ship_type::B_SUBMARINE] and
		ship_types_A[Ship::ship_type::A_DESTROYER] == ship_types_B[Ship::ship_type::B_DESTROYER];


}

bool BoardFullData::validate_ships_shape(int player_id) const
{
	Ship::ship_type ship_types_A[Utils::NUMBER_OF_SHIP_TYPES] = { Ship::ship_type::A_BOAT, Ship::ship_type::A_SATIL, Ship::ship_type::A_SUBMARINE, Ship::ship_type::A_DESTROYER };
	Ship::ship_type ship_types_B[Utils::NUMBER_OF_SHIP_TYPES] = { Ship::ship_type::B_BOAT, Ship::ship_type::B_SATIL, Ship::ship_type::B_SUBMARINE, Ship::ship_type::B_DESTROYER };
	Ship::ship_type *ship_types;
	if (player_id == Board::PLAYER_A)
		ship_types = ship_types_A;
	else
		ship_types = ship_types_B;
	bool is_valid = true;
	auto ships = player_id == Board::PLAYER_A ? shipsA : shipsB;
	// validate shape of A ships
	for (int i = 0; i < Utils::NUMBER_OF_SHIP_TYPES; i++)
	{
		for (vector<Ship>::const_iterator it = ships.begin(); it != ships.end(); ++it)
		{
			if (it->getType() == ship_types[i] && !it->getValid())
			{
				//Requirement print
				if (player_id == Board::PLAYER_A)
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

bool BoardFullData::validate_no_adjacent_ships() const
{
	vector<Ship> all_ships;
	all_ships.insert(all_ships.end(), shipsA.begin(), shipsA.end());
	all_ships.insert(all_ships.end(), shipsB.begin(), shipsB.end());
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