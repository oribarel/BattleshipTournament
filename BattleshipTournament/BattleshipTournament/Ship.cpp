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

Ship& Ship::operator=(Ship& other)
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

void Ship::hitAt(int row, int col)
{
	for (vector<pair<pair<int, int>, bool>>::iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (make_pair(row, col) == it->first)
		{
			it->second = true;
		}
	}
}

bool Ship::isSunk() const
{
	for (vector<pair<pair<int, int>, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (!it->second) //second is a bool which means "is Hit"
			return false;
	}
	return true;

}
bool Ship::containsCoord(int row, int col) const
{
	bool contains = false;
	for (vector<pair<pair<int, int>, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
	{
		if (it->first == make_pair(row, col))
		{
			contains = true;
			break;
		}
	}
	return contains;
}

int Ship::getRow(vector<pair<pair<int, int>, bool>>::const_reference pair) 
{
	return pair.first.first;
}
int  Ship::getCol(vector<pair<pair<int, int>, bool>>::const_reference pair) 
{
	return pair.first.second;
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


bool Ship::isValidShip(const vector<pair<pair<int, int>, bool>>& coordinates, ship_type type)
{
	int cur_row, cur_col;
	bool dif_row = false, dif_col = false;
	size_t size = coordinates.size();

	int row = Ship::getRow((coordinates)[0]);
	int col = Ship::getCol((coordinates)[0]);
	for(size_t i = 0; i < size; i++)
	{
		cur_row = Ship::getRow((coordinates)[i]);
		cur_col = Ship::getCol((coordinates)[i]);
		if (cur_row != row)
			dif_row = true;
		if (cur_col != col)
			dif_col = true;
	}
	return !((dif_row && dif_col) || size > MAXIMUM_SHIP_SIZE || getSizeOfShipType(type) != size);

}

const vector<pair<pair<int, int>, bool>>& Ship::getCoordinates() const
{
	return coordinates;
}

vector<pair<int, int>> Ship::getOnlyCoords() const
{
    auto onlyCrds = vector<pair<int, int>>();
    for (auto const& crdHitPair : coordinates)
    {
        onlyCrds.push_back(crdHitPair.first);
    }
    return onlyCrds;
}

vector<pair<int, int>> Ship::getAdjacentCoordinates(const Board& brd) const
{
    auto onlyCrds = getOnlyCoords();
    auto coordsSet = set<pair<int, int>>(onlyCrds.begin(), onlyCrds.end());
    auto adjacents = set<pair<int, int >>();
    for (auto const& crd : coordsSet)
    {
        for (int k = 0; k < 4; k++)
        {
            int i = (k % 2 == 0) ? k - 1 : 0;
            int j = (k % 2 == 0) ? 0 : k - 2;

            /* Truth table for convenience:
             * k    i   j
             * 0    -1  0
             * 1    0   -1
             * 2    1   0
             * 3    0   1
             */
            auto currCrd = make_pair(crd.first + i, crd.second + j);

            //continue if currCrd is either not in board or a part of the ship itself
            if (!brd.isInBoard(currCrd) || coordsSet.count(currCrd) == 1)
                continue;
            adjacents.insert(currCrd);
        }
    }
    return vector <pair<int, int>>(adjacents.begin(), adjacents.end());
}

Ship::Ship(ship_type type, vector<pair<int, int>> *coordinates_only) :type(type), coordinates(vector<pair<pair<int, int>, bool>>())
{	
	this->size = coordinates_only->size();
	for (int i = 0; i < size; i++)
	{
		int row = (*coordinates_only)[i].first;
		int col = (*coordinates_only)[i].second;
		coordinates.push_back(make_pair(make_pair(row,col), false));
	}
	this->valid = isValidShip(coordinates, type);
}

bool Ship::getValid() const
{
	return valid;
}

bool Ship::isAdjacentCoordinates(pair<int, int> a, pair<int, int> b)
{
	int a_x = a.first;
	int a_y = a.second;
	int b_x = b.first;
	int b_y = b.second;

	if (a_x == b_x && a_y == b_y - 1)
		return true;
	if (a_x == b_x && a_y == b_y + 1)
		return true;
	if (a_y == b_y && a_x == b_x - 1)
		return true;
	if (a_y == b_y && a_x == b_x + 1)
		return true;	
	return false;
}

bool Ship::isAdjacentShips(Ship other_ship) const
{
	for (vector<pair<pair<int, int>, bool>>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
		for (vector<pair<pair<int, int>, bool>>::const_iterator it2 = other_ship.coordinates.begin(); it2 != other_ship.coordinates.end(); ++it2)
			if (Ship::isAdjacentCoordinates(it->first, it2->first))
				return true;
	return false;
}



Ship::~Ship()
{
	DEBUG("dtor is called for a ship of type " << static_cast<char>(type));
}
