#include "Ship.h"
#include <iostream>
#include <iso646.h>
#include <set>
#include <math.h>
#include "BoardClass.h"
#include "Utils.h"

Ship::Ship()
{
	
}
Ship::Ship(const Ship & ship) : type(ship.type), size(ship.size),
	coordinates(ship.coordinates), valid(ship.valid)
{
}

Ship& Ship::operator=(Ship& other) const
{
	DEBUG("Oh no, operator= of Ship was used!");
	return other;
}


int Ship::getScoreForSinking() const
{
	switch (type)
	{
	case (ship_type::A_BOAT):
	case (ship_type::B_BOAT):
		return BOAT_SCORE;
	case (ship_type::A_SATIL):
	case(ship_type::B_SATIL):
		return SATIL_SCORE;
	case (ship_type::A_SUBMARINE):
	case(ship_type::B_SUBMARINE):
		return SUBMARINE_SCORE;
	case (ship_type::A_DESTROYER):
	case(ship_type::B_DESTROYER):
		return DESTROYER_SCORE;
	default:
		return BOAT_SCORE;
	}
}

void Ship::hitAt(Coordinate c)
{
	for (vector<pair<Coordinate, bool>>::iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (Utils::doesCoordsEqual(c,it->first))
		{
			it->second = true;
		}
	}
}

bool Ship::isSunk() const
{
	for (vector<pair<Coordinate, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (!it->second) //second is a bool which means "is Hit"
			return false;
	}
	return true;

}
bool Ship::containsCoord(Coordinate c) const
{
	bool contains = false;
	for (vector<pair<Coordinate, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (Utils::doesCoordsEqual(c, it->first))
		{
			contains = true;
			break;
		}
	}
	return contains;
}

int Ship::getRow(vector<pair<Coordinate, bool>>::const_reference pair) 
{
	return pair.first.row;
}
int Ship::getCol(vector<pair<Coordinate, bool>>::const_reference pair)
{
	return pair.first.col;
}
int Ship::getDepth(vector<pair<Coordinate, bool>>::const_reference pair)
{
	return pair.first.depth;
}

Ship::ship_type Ship::getType() const
{
	return type;
}

bool Ship::isAShip() const
{
	if (type == ship_type::A_BOAT || type == ship_type::A_SATIL || 
		type == ship_type::A_SUBMARINE || type == ship_type::A_DESTROYER)
	{
		return true;
	}
	return false;
}

bool Ship::isAShip(char c)
{
	return (c == static_cast<char>(ship_type::A_BOAT) or
		c == static_cast<char>(ship_type::A_SATIL) or
		c == static_cast<char>(ship_type::A_SUBMARINE) or
		c == static_cast<char>(ship_type::A_DESTROYER));
		
}
bool Ship::isBShip() const
{
	if (type == ship_type::B_BOAT || type == ship_type::B_SATIL ||
		type == ship_type::B_SUBMARINE || type == ship_type::B_DESTROYER)
	{
		return true;
	}
	return false;
}
bool Ship::isBShip(char c)
{
	return (c == static_cast<char>(ship_type::B_BOAT) or
		c == static_cast<char>(ship_type::B_SATIL) or
		c == static_cast<char>(ship_type::B_SUBMARINE) or
		c == static_cast<char>(ship_type::B_DESTROYER));
}


int Ship::getSizeOfShipType(Ship::ship_type type)
{
	int size;
	switch (type)
	{
	case (ship_type::A_BOAT):
	case (ship_type::B_BOAT):
		size = BOAT_SIZE;
		break;
	case (ship_type::A_SATIL):
	case(ship_type::B_SATIL):
		size = SATIL_SIZE;
		break;
	case (ship_type::A_SUBMARINE):
	case(ship_type::B_SUBMARINE):
		size = SUBMARINE_SIZE;
		break;
	case (ship_type::A_DESTROYER):
	case(ship_type::B_DESTROYER):
		size = DESTROYER_SIZE;
		break;
	}
	return size;
}


bool Ship::isValidShip(const vector<pair<Coordinate, bool>>& coordinates, ship_type type)
{
	int cur_row, cur_col, cur_depth;
	bool dif_row = false, dif_col = false, dif_depth = false;
	size_t size = coordinates.size();

	int row = Ship::getRow((coordinates)[0]);
	int col = Ship::getCol((coordinates)[0]);
	int depth = Ship::getDepth((coordinates)[0]);
	for(size_t i = 0; i < size; i++)
	{
		cur_row = Ship::getRow((coordinates)[i]);
		cur_col = Ship::getCol((coordinates)[i]);
		cur_depth = Ship::getDepth((coordinates)[i]);		
		if (cur_row != row)
			dif_row = true;
		if (cur_col != col)
			dif_col = true;
		if (cur_depth != depth)
			dif_depth = true;
	}
	return !((dif_row and dif_col) or
		(dif_row and dif_depth) or
		(dif_col and dif_depth) or
		size > MAXIMUM_SHIP_SIZE or
		getSizeOfShipType(type) != size);

}

const vector<pair<Coordinate, bool>>& Ship::getCoordinates() const
{
	return coordinates;
}

vector<Coordinate> Ship::getOnlyCoords() const
{
    auto onlyCrds = vector<Coordinate>();
    for (auto const& crdHitPair : coordinates)
    {
        onlyCrds.push_back(crdHitPair.first);
    }
    return onlyCrds;
}

vector<Coordinate> Ship::getAdjacentCoordinates(const Board& brd) const
{
    auto onlyCrds = getOnlyCoords();
    auto coordsSet = set<Coordinate, classcomp>(onlyCrds.begin(), onlyCrds.end());
    auto adjacents = set<Coordinate, classcomp>();
    for (auto const& crd : coordsSet)
    {
        for (int k = 1; k < 7; k++)
        {
            int a = (k == 1 or k == 3) ? k - 2 : 0;
            int b = (k == 2 or k == 4) ? k - 3 : 0;
			int c = (k == 5 or k ==6) ? (k-5)+(k == 5 ? -1: 1): 0;

            /* Truth table for convenience:
             * k    a   b	c
             * 0    -1  0	0
             * 1    0   -1	0
             * 2    1   0	0
             * 3    0   1	0
             * 4	0	0	1
             * 5	0	0	-1
             */
            auto currCrd = Coordinate(crd.row + a, crd.col + b, crd.depth + c);

            //continue if currCrd is either not in board or a part of the ship itself
            if (!brd.isInBoard(currCrd) || coordsSet.count(currCrd) == 1)
                continue;
            adjacents.insert(currCrd);
        }
    }
    return vector<Coordinate>(adjacents.begin(), adjacents.end());
}

Ship::Ship(ship_type type, vector<Coordinate> *coordinates_only) :type(type), coordinates(vector<pair<Coordinate, bool>>())
{	
	this->size = coordinates_only->size();
	for (int i = 0; i < size; i++)
	{
		coordinates.push_back(make_pair((*coordinates_only)[i], false));
	}
	this->valid = isValidShip(coordinates, type);
}

bool Ship::getValid() const
{
	return valid;
}

bool Ship::isAdjacentCoordinates(Coordinate c1, Coordinate c2)
{
	if (c1.row == c2.row and c1.col == c1.col and  c1.depth == c2.depth - 1)
		return true;
	if (c1.row == c2.row and c1.col == c1.col and  c1.depth == c2.depth + 1)
		return true;
	if (c1.row == c2.row and c1.col == c1.col -1 and  c1.depth == c2.depth)
		return true;
	if (c1.row == c2.row and c1.col == c1.col + 1 and c1.depth == c2.depth)
		return true;	
	if (c1.row == c2.row - 1 and c1.col == c1.col and c1.depth == c2.depth)
		return true;
	if (c1.row == c2.row + 1 and c1.col == c1.col and c1.depth == c2.depth)
		return true;
	return false;
}

bool Ship::isAdjacentShips(Ship other_ship) const
{
	for (vector<pair<Coordinate, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
		for (vector<pair<Coordinate, bool>>::const_iterator it2 = other_ship.coordinates.begin(); it2 != other_ship.coordinates.end(); ++it2)
			if (Ship::isAdjacentCoordinates(it->first, it2->first))
				return true;
	return false;
}



Ship::~Ship()
{
	DEBUG("dtor is called for a ship of type " << static_cast<char>(type));
}
