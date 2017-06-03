#pragma once
#include <utility>
#include <vector>

// Forward declaration for Board Class
class Board;


using namespace std;
class Ship
{
public:
	
	enum class ship_type : char { A_BOAT = 'B', A_SATIL = 'P' , A_SUBMARINE = 'M', A_DESTROYER = 'D',
		B_BOAT = 'b', B_SATIL = 'p', B_SUBMARINE = 'm', B_DESTROYER = 'd'};
	
	
	static const int BOAT_SIZE = 1;
	static const int SATIL_SIZE = 2;
	static const int SUBMARINE_SIZE = 3;
	static const int DESTROYER_SIZE = 4;

	static const int BOAT_SCORE			= 2;
	static const int SATIL_SCORE		= 3;
	static const int SUBMARINE_SCORE	= 7;
	static const int DESTROYER_SCORE	= 8;

	static const int MAXIMUM_SHIP_SIZE = DESTROYER_SIZE;

	Ship();
	Ship(const Ship& ship);
	Ship & operator=(Ship & other);
	Ship(ship_type type, vector<pair<int, int>> *coordinates_only);
	~Ship();
	
	//-- queries
	bool isAShip() const;
	static bool Ship::isAShip(char c);
	bool isBShip() const;
	static bool Ship::isBShip(char c);
	bool isSunk() const;
	bool containsCoord(int row, int col) const;
	bool Ship::isValidShip(const vector<pair<pair<int, int>, bool>>& coordinates, ship_type type);
	static bool isAdjacentCoordinates(pair<int, int> a, pair<int, int> b); 
	bool isAdjacentShips(Ship other_ship) const;

	//-- getters
	static int getSizeOfShipType(Ship::ship_type type);
	const vector<pair<pair<int, int>, bool>>& Ship::getCoordinates() const;
    vector<pair<int, int>> getOnlyCoords() const;
    vector<pair<int, int>> getAdjacentCoordinates(const Board& brd) const;
	static int getRow(vector<pair<pair<int, int>, bool>>::const_reference pair);
	static int getCol(vector<pair<pair<int, int>, bool>>::const_reference pair);
	int getScoreForSinking() const;
	bool getValid() const;
	Ship::ship_type getType() const;

	//-- operations
	void hitAt(int row, int col);


private:
	ship_type type;
	size_t size;
	//pair<int, int> start;
	//pair<int, int> end;
	vector<pair<pair<int, int>,bool>> coordinates; // bool is "isHit" (false if not hit)
	bool valid; //true is ship is of valid shape
};

