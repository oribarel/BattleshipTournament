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
    destroySession(vector<Crd>()),
    uncompleteHitSpots(HeatMap())
{
	DEBUG("@@@@@@@@@@@@@@@@@@@@@@@@@@@ MAP player");
    srand(static_cast<int>(time(nullptr)));
}

Battleship_MAP_player::~Battleship_MAP_player()
{
}


// TODO
void Battleship_MAP_player::setPlayer(int player)
{
    //-- TODO (symbolizes a rest - so reset thyself!)
    player_ = player;
}

// adapted
void Battleship_MAP_player::setBoard(const BoardData& board)
{
    constructBoardFromBoardData(board);
    this->brd.findShips(PLAYER_A, ships);
    if (ships.empty())
        this->brd.findShips(PLAYER_B, ships);

    initObstacles();
}

// adapted
void Battleship_MAP_player::constructBoardFromBoardData(const BoardData& board_data)
{
    for (int i = 1; i <= board_data.rows(); i++)
    {
        for (int j = 1; j <= board_data.cols(); j++)
        {
            for (int k = 1; k <= board_data.depth(); k++)
            {
                auto crd = Crd(i, j, k);
                brd.setSlot(crd, board_data.charAt(crd));
            }
        }
    }
}

// adapted
Coordinate Battleship_MAP_player::attack()
{
    HeatMap heatmap;
    Crd attackCrd(-1,-1,-1);
    if (isSeek)
    {
        heatmap = computeSeekHeat();
        heatmap.getMaxHeatCoord(attackCrd);
    }
    else
    {
        heatmap = computeDestroyHeat();
        heatmap.getMaxHeatCoord(attackCrd);
    }
    return attackCrd;
}

// adapted
bool Battleship_MAP_player::myShipAt(Coordinate crd) const
{
    return brd(crd) != Board::SEA;
}


/************************
 *      Notifications   *
 ************************/

 // adapted
void Battleship_MAP_player::notifyAttackMiss(Coordinate crd)
{
    obstacles.setCoord(crd, 1);
}


// adapted
/**
 * \brief: This is called to notify me only 
 * when ***I*** hit the enemy.
 * (Assuming I will never shoot myself - 
 * which I don't intend to do)
 */
void Battleship_MAP_player::notifyAttackHit(Coordinate crd)
{
    isSeek = false; //going to "Destroy" Mode (or already in this mode)

    // If not first hit then the hit must be adjacent to 
    // a previous hit - meaning same ship.

    // maybe this hit connected other incomplete hits to the destroy session?
    uncompleteHitSpots.setCoord(crd, 1); //set it as hit
    auto revealed = vector<Crd>();
    uncompleteHitSpots.reveal(crd, revealed);

    for (auto const& icrd : revealed)
    {
        destroySession.push_back(icrd);
    }
}

// adapted
void Battleship_MAP_player::initDestroyModeRightAfterSink()
{
    Crd new_destroy_core(-1, -1, -1);
    uncompleteHitSpots.getMaxHeatCoord(new_destroy_core); //selects random uncompleted hit
    
    auto revealed = vector<Crd>();
    uncompleteHitSpots.reveal(new_destroy_core, revealed); //removes from uncompleteHitSpots!
    for (auto const& crd: revealed)
    {
        destroySession.push_back(crd);
    }
}


// adapted
/**
 * \brief: This is called to notify me only
 * when ***I*** sunk an enemy ship.
 * (Assuming I will never shoot myself - 
 * which I don't intend to do)
 */
void Battleship_MAP_player::notifyAttackSink(Coordinate crd)
{
    destroySession.push_back(crd);
    turn_sunk_ship_to_obstacle(destroySession);
    destroySession.clear();

    // if uncompleteHitSpots is empty then just seek
    // else - I know where to find enemy ships!
    isSeek = (uncompleteHitSpots.getMaxHeat() == 0); //checks if empty
    if (!isSeek)
        initDestroyModeRightAfterSink();
}

// adapted
void Battleship_MAP_player::notifyOpponentAttackHit(Coordinate opponentHit)
{
    if (!myShipAt(opponentHit)) // nice! the opponent hit itself and I discovered its location!
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

// adapted
bool Battleship_MAP_player::crdHelpsMeInDestroyMode(Coordinate crd) const
{
    bool helps_me_in_destroy_mode = false;
    for (auto const& icrd : destroySession)
    {
        if (Ship::isAdjacentCoordinates(icrd, crd))
        {//helps me in my destroy mode:
            helps_me_in_destroy_mode = true;
            break;
        }
    }
    return helps_me_in_destroy_mode;
}

// adapted
void Battleship_MAP_player::turn_sunk_ship_to_obstacle(vector<Crd> sunkShipCoords)
{
    for (auto const& crd : sunkShipCoords)
    {
        obstacles.setCoord(crd, 1);
        Crd moves[6] =
        {
            Crd(1, 0, 0),
            Crd(-1, 0, 0),
            Crd(0, 1, 0),
            Crd(0, -1, 0),
            Crd(0, 0, 1),
            Crd(0, 0, -1)
        };
        auto addCoords = [](Crd a, Crd b) { return Crd(a.row + b.row, a.col + b.col, a.depth + b.depth); };

        for (auto const& move : moves)
        { 
            auto currCrd = addCoords(move, crd);
            if (brd.isInBoard(currCrd))
                obstacles.setCoord(currCrd, 1); //set adjacent coords as obstacle
        }
    }
}

// adapted
void Battleship_MAP_player::notifyOpponentAttackSink(Coordinate sinkCrd)
{
    if (!myShipAt(sinkCrd)) // nice! the opponent sink itself!
    {
        if (crdHelpsMeInDestroyMode(sinkCrd))
        { //sets all destroy session coords to obstacles, turns on seek mode (or destroy),
          //and clears destroySession.
            notifyAttackSink(sinkCrd);
        }
        else
        { // either I can find adjacents in uncompleteHitSpots or it was a 1 coord ship.
            uncompleteHitSpots.setCoord(sinkCrd, 1); //set it as hit
            auto revealed = vector<Coordinate>();
            uncompleteHitSpots.reveal(sinkCrd, revealed);
            turn_sunk_ship_to_obstacle(revealed);
        }
    }
    //else opponent sunk my ship :(
}

// adapted
void Battleship_MAP_player::notifyOnAttackResult(int player, Coordinate crd, AttackResult result)
{
    /* Whoever made the shot, if missed, I'm interested.*/
    if (result == AttackResult::Miss)
    {
        notifyAttackMiss(crd);
        return;
    }
    if (player == player_)
    {
        // tell me what happened with my attack:
        if (result == AttackResult::Hit)
            notifyAttackHit(crd);
        else
            notifyAttackSink(crd);
    }
    else
    {
        // tell me what happened with opponent's attack:
        if (result == AttackResult::Hit)
            notifyOpponentAttackHit(crd);
        else
            notifyOpponentAttackSink(crd);
    }
}

// adapted
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

// adapted
void Battleship_MAP_player::seekHeat_addHeat_tryShipSizes(HeatMap& seekHeatMap, Coordinate crd) const
{
    seekHeatMap.coordPlusX(crd, 1); //adding for 1 slot ship since has no orientation
    
    Crd orientations[] = {
        Crd(1,0,0),
        Crd(0,1,0),
        Crd(0,0,1)
    };

    for (auto const& orient : orientations)
    {
        auto base_plus_aXadv = [](Crd base, int a, Crd adv)
        {
            return Crd(base.row + a * adv.row,
                base.col + a * adv.col,
                base.depth + a * adv.depth);
        };

        auto enemyCrds = vector<Coordinate>();
        enemyCrds.push_back(crd);
        for (int shipSize = 2; shipSize <= Ship::MAXIMUM_SHIP_SIZE; shipSize++)
        { //starting from shipSize 2 because ship of one slot has no orientation.

            // check all coords are not in obstacles:
            auto longer = base_plus_aXadv(crd, shipSize - 1, orient);
            if (!brd.isInBoard(longer) || obstacles(longer) == 1)
                break;
            
            enemyCrds.push_back(longer);
            // if here means no obstacles:
            for (const auto& icrd : enemyCrds)
                seekHeatMap.coordPlusX(icrd, 1);
        }
    }
}

// adapted
/**
 * \brief: This function goes over all of the coords of the board,
 * and if a coord could possible be a top/north/west edge of an enemy
 * ship, then it increases the heat of all the coords of that ship
 * by 1.
 */
HeatMap Battleship_MAP_player::computeSeekHeat() const
{
    auto seekHeatMap = HeatMap(brd);
    for (int i = 1; i <= brd.rows(); i++)
    {
        for (int j = 1; j <= brd.cols(); j++)
        {
            for (int k = 1; k <= brd.depth(); k++)
            {
                //i,j is the top/left edge of an enemy ship
                auto crd = Coordinate(i, j, k);
                if (obstacles(crd) == 1)
                    continue;
                seekHeat_addHeat_tryShipSizes(seekHeatMap, crd);
            }
        }
    }
    return seekHeatMap;
}

// adapted
Coordinate Battleship_MAP_player::findTopNorthWestrenDestroySessionCoord() const
{
    // assuming that in destroy session all coords share same i or same j
    // so monotoneusly find the min.

    Coordinate min_pair(MAXINT16, MAXINT16, MAXINT16);
    for (auto const& crd : destroySession)
    {
        if (crd.row + crd.col + crd.depth < min_pair.row + min_pair.col + min_pair.depth)
            min_pair = crd;
    }
    return min_pair;
}

// adapted
void Battleship_MAP_player::computeDestroyHeatAllOptions(HeatMap& heat_map, Crd orientation) const
{
    auto topNorthWest = findTopNorthWestrenDestroySessionCoord();

    auto base_plus_a_times_adv = [](Crd base, int a, Crd adv)
    {
        return Crd(base.row + a * adv.row,
            base.col + a * adv.col,
            base.depth + a * adv.depth);
    };

    size_t len = destroySession.size();
    for (size_t shipSize = len + 1; shipSize <= Ship::MAXIMUM_SHIP_SIZE; shipSize++) 
    { //go over possible ship sizes
        for (int mv = 0; mv <= shipSize - len; mv++)
        { //go over possible positions
            auto startCrd = base_plus_a_times_adv(topNorthWest, -mv, orientation);
            auto currPositionCrdVect = vector<Crd>();
            bool legal = true;
            for (int crdIdx = 0; crdIdx < shipSize; crdIdx++)
            {
                auto currCrd = base_plus_a_times_adv(startCrd, crdIdx, orientation);
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

// adapted
/**
 * \brief: this function must be called only if
 * the destroySession has more than 1 coord.
 */
Crd Battleship_MAP_player::destroySessionOrientation() const
{
    auto crd1 = destroySession[0];
    auto crd2 = destroySession[1];
    if (crd1.row == crd2.row) // horiz or depth
    { 
        if (crd1.col == crd2.col) //depth
        {
            return Crd(0, 0, 1);
        }
        return Crd(0, 1, 0);
    }
    //vert
    return Crd(0, 1, 0);
}

// adapted
HeatMap Battleship_MAP_player::computeDestroyHeat() const
{
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
        //special case 3 orientations
        computeDestroyHeatAllOptions(destroyHeatMap, Crd(1, 0, 0));
        computeDestroyHeatAllOptions(destroyHeatMap, Crd(0, 1, 0));
        computeDestroyHeatAllOptions(destroyHeatMap, Crd(0, 0, 1));
    }
    else if (destroySession.size() > 1)
    {
        // detect the orientation then only one
        Crd orientation = destroySessionOrientation();
        computeDestroyHeatAllOptions(destroyHeatMap, orientation);
    }
    else
    {
        throw exception("Not supposed to calculate destroy heat with no coordinates in the destroy session!");
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
HeatMap::HeatMap(int rows, int columns, int depth) : 
    rows_(rows),
    columns_(columns),
    depth_(depth),
    data_(vector<int>(rows * columns * depth ,0))
{
    DEBUG("non-default HeatMap ctor activated - HeatMap with only zeros created");
}

HeatMap::HeatMap(const BoardData &brd_data) :
    HeatMap(brd_data.rows(), brd_data.cols(), brd_data.depth())
{
}

HeatMap::HeatMap(const HeatMap &map) : 
    rows_(map.rows_),
    columns_(map.columns_),
    depth_(map.depth_),
    data_(vector<int>(map.data_))
{}


//dtor
HeatMap::~HeatMap()
{
    DEBUG("HeatMap dtor activated");
}


HeatMap& HeatMap::operator=(const HeatMap &other)
{
    DEBUG("operator= of HeatMap was called");
    if (this != &other)
    {
        rows_ = other.rows_;
        columns_ = other.columns_;
        depth_ = other.depth_;
        data_ = vector<int>(other.data_);
    }
    //self assignment
    return *this;
}

/*In the game, indexing starts at 1*/
int HeatMap::operator()(Coordinate crd) const
{
    crd.row -= 1;
    crd.col -= 1;
    crd.depth -= 1;

    if (-1 < crd.row   && crd.row < rows_    &&
        -1 < crd.col   && crd.col < columns_ &&
        -1 < crd.depth && crd.depth < depth_)
    {
        return data_[crd.depth * (rows_ * columns_)
                    + crd.row * columns_
                    + crd.col];
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

void HeatMap::setCoord(Coordinate crd, int newVal)
{
    crd.row -= 1;
    crd.col -= 1;
    crd.depth -= 1;

    if (-1 < crd.row   && crd.row < rows_    &&
        -1 < crd.col   && crd.col < columns_ &&
        -1 < crd.depth && crd.depth < depth_)
    {
        data_[crd.depth * (rows_ * columns_)
            + crd.row * columns_
            + crd.col] = newVal;
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}


void HeatMap::coordPlusX(Coordinate crd, int X)
{
    crd.row -= 1;
    crd.col -= 1;
    crd.depth -= 1;

    if (-1 < crd.row   && crd.row < rows_    &&
        -1 < crd.col   && crd.col < columns_ &&
        -1 < crd.depth && crd.depth < depth_)
    {
        data_[crd.depth * (rows_ * columns_)
            + crd.row * columns_
            + crd.col] += X;
    }
    throw std::out_of_range("Trying to access invalid index in the HeatMap.");
}

int HeatMap::getNumRows() const
{
    return rows_;
}

int HeatMap::getNumCols() const
{
    return columns_;
}

int HeatMap::getNumOfDepthLayers() const
{
    return depth_;
}


int HeatMap::getMaxHeat() const
{
    Crd maxHeatCrd(-1, -1, -1);
    getMaxHeatCoord(maxHeatCrd);
    return (*this)(maxHeatCrd);
}

void HeatMap::getMaxHeatCoord(Crd& crd) const
{
    int maxHeat = -1;
    auto maxHeatCrd = vector<Coordinate>();
    for (int i = 1; i <= rows_; i++)
    {
        for (int j = 1; j <= columns_; j++)
        {
            for (int k = 1; k <= depth_; k++)
            {
                auto currCrd = Coordinate(i, j, k);
                if ((*this)(currCrd) == maxHeat)
                {
                    maxHeatCrd.push_back(currCrd);
                }
                else if ((*this)(currCrd) > maxHeat)
                {
                    maxHeatCrd.clear();
                    maxHeatCrd.push_back(currCrd);
                    maxHeat = (*this)(currCrd);
                }
            }
        }
    }
    if (maxHeatCrd.empty())
    {
        printf("Failed to find max heat coordinate!");
        return;
    }
    int randomIndex = rand() % maxHeatCrd.size();
    crd = maxHeatCrd[randomIndex];
}

/**
 * \brief: this is used on the uncompleteHitSpots
 * in order to reveal a group of adjacent hit spots
 * Note: removed them from uncompleteHitSpots altogether!
 */
void HeatMap::reveal(Coordinate crd, vector<Coordinate> &revealed)
{
    if ((*this)(crd) == 1)
    {
        setCoord(crd, 0);
        revealed.push_back(crd);
        int rows[2] = { crd.row + 1, crd.row - 1 };
        int cols[2] = { crd.col + 1, crd.col - 1 };
        int depths[2] = { crd.depth + 1, crd.depth - 1 };
        for (int i = 0; i < 2; i++)
        {
            try
            {
                reveal(Crd(rows[i], crd.col, crd.depth), revealed);
            }
            catch(exception) {}//exceptions will stop recursion
            try
            {
                reveal(Crd(crd.row, cols[i], crd.depth), revealed);
            }
            catch (exception) {}//exceptions will stop recursion
            try
            {
                reveal(Crd(crd.row, crd.col, depths[i]), revealed);
            }
            catch (exception) {}//exceptions will stop recursion
        }
    }
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *algo = new Battleship_MAP_player();
	return algo;
}