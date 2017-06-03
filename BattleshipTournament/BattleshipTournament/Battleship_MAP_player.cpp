#include "Battleship_MAP_player.h"
#include <set>
#include <exception>
#include <ctime>
#include <iso646.h>


/*************************************
*  Battleship_MAP_player Functions   *
**************************************/

Battleship_MAP_player::Battleship_MAP_player() :
    player_(0),
    brd(Board()),
    ships(vector<Ship>()),
    obstacles(HeatMap()),
    isSeek(true),
    destroySession(vector<pair<int,int>>()),
    uncompleteHitSpots(HeatMap())
{
	DEBUG("@@@@@@@@@@@@@@@@@@@@@@@@@@@ MAP player");
    srand(static_cast<int>(time(nullptr)));
}

Battleship_MAP_player::~Battleship_MAP_player()
{
}

bool Battleship_MAP_player::init(const std::string & path)
{
    return true;
}

void Battleship_MAP_player::setBoard(int player, const char ** board, int numRows, int numCols)
{
    player_ = player;
    brd = Board(board, numRows, numCols);
    this->brd.findShips(PLAYER_A, ships);
    if (ships.empty())
        this->brd.findShips(PLAYER_B, ships);

    initObstacles();
}

std::pair<int, int> Battleship_MAP_player::attack()
{
    HeatMap heatmap;
    pair<int, int> attackCrd;
    if (isSeek)
    {
        heatmap = computeSeekHeat();
        attackCrd = heatmap.getMaxHeatCoord();
    }
    else
    {
        heatmap = computeDestroyHeat();
        attackCrd = heatmap.getMaxHeatCoord(); //_destroy();
    }
    return attackCrd;
}

bool Battleship_MAP_player::myShipAt(int row, int col) const
{
    return brd(row, col) != Board::SEA;
}

void Battleship_MAP_player::notifyAttackMiss(int row, int col)
{
    obstacles.setCoord(row, col, 1);
}

/**
 * \brief: This is called to notify me only 
 * when ***I*** hit the enemy.
 * (Assuming I will never shoot myself - 
 * which I don't intend to do)
 */
void Battleship_MAP_player::notifyAttackHit(int row, int col)
{
    isSeek = false; //going to "Destroy" Mode (or already in this mode)

    // If not first hit then the hit must be adjacent to 
    // a previous hit - meaning same ship.
    auto hitCrd = make_pair(row, col);

    // maybe this hit connected other incomplete hits to the destroy session?
    uncompleteHitSpots.setCoord(hitCrd, 1); //set it as hit
    auto revealed = vector<pair<int, int>>();
    uncompleteHitSpots.reveal(hitCrd, revealed);

    for (auto const& crd : revealed)
    {
        destroySession.push_back(crd);
    }
}

void Battleship_MAP_player::initDestroyModeRightAfterSink()
{
    auto new_destroy_core = uncompleteHitSpots.getMaxHeatCoord(); //selects random uncompleted hit
    auto revealed = vector<pair<int, int>>();
    uncompleteHitSpots.reveal(new_destroy_core, revealed); //removes from uncompleteHitSpots!
    for (auto const& crd: revealed)
    {
        destroySession.push_back(crd);
    }
}

/**
 * \brief: This is called to notify me only
 * when ***I*** sunk an enemy ship.
 * (Assuming I will never shoot myself - 
 * which I don't intend to do)
 */
void Battleship_MAP_player::notifyAttackSink(int row, int col)
{
    //TODO: the same as in notify hit - reveal uncompleted
    destroySession.push_back(make_pair(row, col));
    turn_sunk_ship_to_obstacle(destroySession);
    destroySession.clear();

    // if uncompleteHitSpots is empty then just seek
    // else - I know where to find enemy ships!
    isSeek = (uncompleteHitSpots.getMaxHeat() == 0); //checks of empty
    if (!isSeek)
        initDestroyModeRightAfterSink();
}

void Battleship_MAP_player::notifyOpponentAttackHit(int row, int col)
{
    auto opponentHit = make_pair(row, col);
    if (!myShipAt(row, col)) // nice! the opponent hit itself and I discovered its location!
    {
        if (isSeek) 
        { //thank you, I was looking all over the place for you
            isSeek = false;
            destroySession.push_back(opponentHit);
        }
        else 
        {//I'm already in the middle of destroying a ship
            if (crdHelpsMeInDestroyMode(opponentHit))
                destroySession.push_back(opponentHit);
            else
                uncompleteHitSpots.setCoord(opponentHit, 1);
        }
    }
}

bool Battleship_MAP_player::crdHelpsMeInDestroyMode(pair<int,int>& coord) const
{
    bool helps_me_in_destroy_mode = false;
    for (auto const& crd : destroySession)
    {
        if (Ship::isAdjacentCoordinates(coord, crd))
        {//helps me in my destroy mode:
            helps_me_in_destroy_mode = true;
            break;
        }
    }
    return helps_me_in_destroy_mode;
}

void Battleship_MAP_player::turn_sunk_ship_to_obstacle(vector<pair<int, int>> sunkShipCoords)
{
    for (auto const& crd : sunkShipCoords)
    {
        obstacles.setCoord(crd, 1);
        for (int k = 0; k < 4; k++)
        { //runs over all adjacents in a single loop // k =  0  1  2  3
            int i = (k % 2 == 0) ? k - 1 : 0;        // i = -1  0  1  0
            int j = (k % 2 == 0) ? 0 : k - 2;        // j =  0 -1  0  1
            auto currCrd = make_pair(crd.first + i, crd.second + j);
            if (brd.isInBoard(currCrd))
                obstacles.setCoord(currCrd, 1); //set adjacent coords as obstacle
        }
    }
}

void Battleship_MAP_player::notifyOpponentAttackSink(int row, int col)
{
    if (!myShipAt(row, col)) // nice! the opponent sink itself!
    {
        auto sinkCrd = make_pair(row, col);
        if (crdHelpsMeInDestroyMode(sinkCrd))
        { //sets all destroy session coords to obstacles, turns on seek mode (or destroy),
          //and clears destroySession.
            //destroySession.push_back(sinkCrd);
            notifyAttackSink(row, col);
        }
        else
        { // either I can find adjacents in uncompleteHitSpots or it was a 1 coord ship.
            uncompleteHitSpots.setCoord(sinkCrd, 1); //set it as hit
            auto revealed = vector<pair<int, int>>();
            uncompleteHitSpots.reveal(sinkCrd, revealed);
            turn_sunk_ship_to_obstacle(revealed);
        }
    }
    //else opponent sunk my ship :(
}

void Battleship_MAP_player::notifyOnAttackResult(
    int player,
    int row,
    int col,
    AttackResult result
)
{
    /* Whoever made the shot, if missed, I'm interested.*/
    if (result == AttackResult::Miss)
    {
        notifyAttackMiss(row, col);
        return;
    }
    if (player == player_)
    {
        // tell me what happened with my attack:
        if (result == AttackResult::Hit)
            notifyAttackHit(row, col);
        else
            notifyAttackSink(row, col);
    }
    else
    {
        // tell me what happened with opponent's attack:
        if (result == AttackResult::Hit)
            notifyOpponentAttackHit(row, col);
        else
            notifyOpponentAttackSink(row, col);
    }
}

/**
 * \when: Only after board exists, and all ships are init'd
 * \brief: This method sets all of the player's ship 
 * coordinates and their adjacents as obstacles.
 */
void Battleship_MAP_player::initObstacles()
{
    // create heatmap the size of board, init'd to zero (meaning not obstacle)
    obstacles = HeatMap(brd);

    for (auto const& ship : ships) 
    {
        for (auto const& crdHitPair : ship.getCoordinates())
        {
            auto crd = crdHitPair.first;
            obstacles.setCoord(crd, 1);
        }
        for (auto const& crd : ship.getAdjacentCoordinates(brd))
        {
            obstacles.setCoord(crd, 1);
        }
    }
}

void Battleship_MAP_player::seekHeat_addHeat_tryShipSizes(HeatMap& seekHeatMap, int i, int j) const
{
    seekHeatMap.coordPlusX(i, j, 1); //adding for 1 slot ship since has no orientation
    for (int orientation = 0; orientation < 2; orientation++)
    {
        auto enemyCrds = vector<pair<int, int>>();
        enemyCrds.push_back(make_pair(i, j));
        for (int shipSize = 2; shipSize <= Ship::MAXIMUM_SHIP_SIZE; shipSize++)
        { //starting from shipSize 2 because ship of one slot has no orientation.
            int adv_i = orientation;
            int adv_j = 1 - orientation;

            // finally check all coords are not in obstacles:
            auto longer = make_pair(i + adv_i * (shipSize - 1), j + adv_j * (shipSize - 1));
            if (!brd.isInBoard(longer) || obstacles(longer) == 1)
                break;
            
            enemyCrds.push_back(longer);
            // if here means no obstacles:
            for (const auto& crd : enemyCrds)
                seekHeatMap.coordPlusX(crd, 1);
        }
    }
}

/**
 * \brief: This function goes over all of the coords of the board,
 * and if a coord could possible be a top/left edge of an enemy
 * ship, then it increases the heat of all the coords of that ship
 * by 1.
 */
HeatMap Battleship_MAP_player::computeSeekHeat() const
{
    auto seekHeatMap = HeatMap(brd);
    for (int i = 1; i <= brd.getNumOfRows(); i++)
    {
        for (int j = 1; j <= brd.getNumOfCols(); j++)
        {
            //i,j is the top/left edge of an enemy ship
            if (obstacles(i,j) == 1)
                continue;
            seekHeat_addHeat_tryShipSizes(seekHeatMap, i, j);
        }
    }
    return seekHeatMap;
}

pair<int, int> Battleship_MAP_player::findTopLeftDestroySessionCoord() const
{
    // assuming that in destroy session all coords share same i or same j
    // so monotoneusly find the min.

    pair<int, int> min_pair(MAXINT16, MAXINT16);
    for (auto const& crd : destroySession)
    {
        if (crd.first + crd.second < min_pair.first + min_pair.second)
            min_pair = crd;
    }
    return min_pair;
}

void Battleship_MAP_player::computeDestroyHeatAllOptions(HeatMap& heat_map, bool isHoriz) const
{
    auto topleft = findTopLeftDestroySessionCoord();
    int i_adv = 1 - isHoriz;
    int j_adv = isHoriz;
    size_t len = destroySession.size();
    for (size_t shipSize = len + 1; shipSize <= Ship::MAXIMUM_SHIP_SIZE; shipSize++) 
    { //go over possible ship sizes
        for (int mv = 0; mv <= shipSize - len; mv++)
        { //go over possible positions
            auto startCrd = make_pair(topleft.first - mv * i_adv, topleft.second - mv * j_adv);
            auto currPositionCrdVect = vector<pair<int, int>>();
            bool legal = true;
            for (int crdIdx = 0; crdIdx < shipSize; crdIdx++)
            {
                auto currCrd = make_pair(startCrd.first + i_adv * crdIdx, startCrd.second + j_adv * crdIdx);
                if (!brd.isInBoard(currCrd) || obstacles(currCrd) == 1)
                {
                    legal = false;
                    break; 
                }
                currPositionCrdVect.push_back(currCrd);
            }
            if (legal)
            {
                for (auto& crd : currPositionCrdVect)
                    heat_map.coordPlusX(crd, 1);
            }
        }
    }
}

/**
 * \brief: this function must be called only if
 * the destroySession has more than 1 coord.
 */
bool Battleship_MAP_player::isDestroySessionHorizontal() const
{
    if (destroySession.size() < 2)
        throw new exception("isDestroySessionHorizontal must be called only if the destroySession has more than 1 coord!");
    auto crd1 = destroySession[0];
    auto crd2 = destroySession[1];
    if (crd1.first != crd2.first)
    { // if exists delta i then destroy session is vertical!
        return false;
    }
    return true;
}


HeatMap Battleship_MAP_player::computeDestroyHeat() const
{
    //TODO: write this implementation
    /*
     * this method calculates the heat of the destroySession
     * looking for the hottest coord to hit
     * 
     * keep in mind you have the uncompleteHitSpots to think about
     * maybe discover that an uncomplete hit helps?
     */
    
    auto destroyHeatMap = HeatMap(brd);
    if (destroySession.size() == 1)
    {
        //special case two orientations
        computeDestroyHeatAllOptions(destroyHeatMap, true);
        computeDestroyHeatAllOptions(destroyHeatMap, false);
    }
    else if (destroySession.size() > 1)
    {
        // detect the orientation then only one
        bool isHoriz = isDestroySessionHorizontal();
        computeDestroyHeatAllOptions(destroyHeatMap, isHoriz);
    }
    else
    {
        throw new exception("Not supposed to calculate destroy heat with no coordinates in the destroy session!");
    }
    for (auto const& crd : destroySession)
    {
        destroyHeatMap.setCoord(crd, 0);
    }
    return destroyHeatMap;
}

/************************
 *  HeatMap Functions   *
 ************************/

//non-default ctor
HeatMap::HeatMap(int rows, int columns) : 
    rows_(rows),
    columns_(columns),
    data_(new int[rows * columns])
{
    DEBUG("non-default HeatMap ctor activated - HeatMap with only zeros created");
    memset(data_, 0, rows*columns * sizeof(int));
}

HeatMap::HeatMap(const Board &brd) :
    HeatMap(brd.getNumOfRows(), brd.getNumOfCols())
{
}

HeatMap::HeatMap(const HeatMap &map) : 
    rows_(map.rows_),
    columns_(map.columns_),
    data_(new int[map.rows_ * map.columns_])
{
    memcpy(data_, map.data_, rows_ * columns_ * sizeof(int));
}

HeatMap& HeatMap::operator=(const HeatMap &other)
{
    DEBUG("operator= of HeatMap was called");
    if (this != &other)
    {
        if (columns_*rows_ != other.columns_*other.rows_) //bummer
        {
            delete[] data_;
            data_ = new int[other.columns_*other.rows_];
        }
        columns_ = other.columns_;
        rows_ = other.rows_;
        memcpy(data_, other.data_, rows_*columns_*sizeof(int));
    }
    //self assignment
    return *this;
}

/*In the game, indexing starts at 1*/
int HeatMap::operator()(int row, int column) const
{
    row -= 1;
    column -= 1;
    if (-1 < row && row < rows_ && -1 < column && column < columns_)
    {
        return data_[columns_*row + column];
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

/*In the game, indexing starts at 1*/
int HeatMap::operator()(pair<int, int> crd) const
{

    int row = crd.first - 1;
    int column = crd.second - 1;
    if (-1 < row && row < rows_ && -1 < column && column < columns_)
    {
        return data_[columns_*row + column];
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

void HeatMap::setCoord(int row, int col, int newVal)
{
    row -= 1;
    col -= 1;
    if (-1 < row && row < rows_ && -1 < col && col < columns_)
    {
        data_[columns_*row + col] = newVal;
        return;
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

void HeatMap::setCoord(pair<int, int> crd, int newVal)
{
    setCoord(crd.first, crd.second, newVal);
}

void HeatMap::coordPlusX(int row, int col, int X)
{
    row -= 1;
    col -= 1;
    if (-1 < row && row < rows_ && -1 < col && col < columns_)
    {
        data_[columns_*row + col] += X;
        return;
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

void HeatMap::coordPlusX(pair<int, int> crd, int X)
{
    coordPlusX(crd.first, crd.second, X);
}

int HeatMap::getNumRows() const
{
    return rows_;
}

int HeatMap::getNumCols() const
{
    return columns_;
}

//dtor
HeatMap::~HeatMap()
{
    DEBUG("HeatMap dtor activated");
    delete[] data_;
}

int HeatMap::getMaxHeat() const
{
    auto maxHeatCrd = getMaxHeatCoord();
    return (*this)(maxHeatCrd);
}

pair<int, int> HeatMap::getMaxHeatCoord() const
{
    int maxHeat = -1;
    auto maxHeatCrd = vector<pair<int, int>>();
    for (int i = 1; i <= rows_; i++)
    {
        for (int j = 1; j <= columns_; j++)
        {
            if ((*this)(i,j) == maxHeat)
            {
                maxHeatCrd.push_back(make_pair(i, j));
            }
            else if ((*this)(i,j) > maxHeat)
            {
                maxHeatCrd.clear();
                maxHeatCrd.push_back(make_pair(i, j));
                maxHeat = (*this)(i, j);
            }
        }
    }
    if (maxHeatCrd.empty())
    {
        throw new exception("Failed to find max heat coordinate!");
    }
    int randomIndex = rand() % maxHeatCrd.size();
    return maxHeatCrd[randomIndex];
}

pair<int, int> HeatMap::getMaxHeatCoord_destroy() const
{
    throw new exception("Unimplemented");
    return pair<int, int>();
}

/**
 * \brief: this is used on the uncompleteHitSpots
 * in order to reveal a group of adjacent hit spots
 * Note: removed them from uncompleteHitSpots altogether!
 */
void HeatMap::reveal(const pair<int, int>& crd, vector<pair<int, int>>& revealed)
{
    if ((*this)(crd) == 1)
    {
        setCoord(crd, 0);
        revealed.push_back(crd);
        int rows[2] = { crd.first + 1, crd.second - 1 };
        int cols[2] = { crd.second + 1, crd.second - 1 };
        for (int i = 0; i < 2; i++)
        {
            try
            {
                reveal(make_pair(crd.first, cols[i]), revealed);
            }
            catch (exception) {} //exceptions will stop recursion
            try
            {
                reveal(make_pair(rows[i], crd.second), revealed);
            }
            catch (exception) {}
        }
    }
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *algo = new Battleship_MAP_player();
	return algo;
}