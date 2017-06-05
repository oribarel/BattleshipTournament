#pragma once
#include <utility>
#include <vector>
#include "IBattleshipGameAlgo.h"

// Forward declaration for Board Class
class Board;


using namespace std;
class Ship
{
public:
	struct classcomp {
		bool operator() (const Coordinate& c1, const Coordinate& c2) const
		{
			return c1.row < c2.row || (c1.row == c2.row && c1.col < c2.col) || (c1.row == c2.row && c1.col == c2.col && c1.depth < c2.depth);
		}
	};
	
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
	Ship & operator=(Ship & other) const;
	Ship::Ship(ship_type type, vector<Coordinate> *coordinates_only);
	~Ship();
	
	//-- queries
	bool isAShip() const;
	static bool Ship::isAShip(char c);
	bool isBShip() const;
	static bool Ship::isBShip(char c);
	bool isSunk() const;
	bool Ship::containsCoord(Coordinate c) const;
	static bool Ship::isValidShip(const vector<pair<Coordinate, bool>>& coordinates, ship_type type);
	static bool Ship::isAdjacentCoordinates(Coordinate c1, Coordinate c2);
	bool isAdjacentShips(Ship other_ship) const;

	//-- getters
	static int getSizeOfShipType(Ship::ship_type type);
	const vector<pair<Coordinate, bool>>& Ship::getCoordinates() const;
	vector<Coordinate> Ship::getOnlyCoords() const;
	vector<Coordinate> Ship::getAdjacentCoordinates(const Board& brd) const;
	static int Ship::getRow(vector<pair<Coordinate, bool>>::const_reference pair);
	static int Ship::getCol(vector<pair<Coordinate, bool>>::const_reference pair);
	static int Ship::getDepth(vector<pair<Coordinate, bool>>::const_reference pair);
	int getScoreForSinking() const;
	bool getValid() const;
	Ship::ship_type getType() const;

	//-- operations
	void Ship::hitAt(Coordinate c);


private:
	ship_type type;
	size_t size;
	//pair<int, int> start;
	//pair<int, int> end;
	vector<pair<Coordinate,bool>> coordinates; // bool is "isHit" (false if not hit)
	bool valid; //true is ship is of valid shape
};

