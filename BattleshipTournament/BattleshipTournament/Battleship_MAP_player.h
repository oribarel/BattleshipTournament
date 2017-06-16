#pragma once

#include "BoardClass.h"
#include "IBattleshipGameAlgo.h"
#include "Utils.h"

using namespace std;

typedef Coordinate Crd;

class HeatMap
{
public:
    HeatMap() : HeatMap(Board::DEFAULT_BOARD_WIDTH, Board::DEFAULT_BOARD_WIDTH, Board::DEFAULT_BOARD_WIDTH) {}//empty constructor
    HeatMap(int rows, int columns, int depth); //non-default
    HeatMap(const BoardData&); //non-default 2
    HeatMap(const HeatMap& map); //copy ctor

    HeatMap& operator=(const HeatMap&);
    int operator()(Coordinate crd) const;
    void setCoord(Coordinate crd, int newVal);
    void coordPlusX(Coordinate crd, int X);


    int getNumRows() const;
    int getNumCols() const;
    int getNumOfDepthLayers() const;
    //friend std::ostream& operator<<(std::ostream &strm, const HeatMap &brd);
    ~HeatMap();
    int getMaxHeat() const;
    void getMaxHeatCoord(Crd& crd) const;
    void HeatMap::reveal(Coordinate crd, vector<Coordinate> &revealed);

private:
    int rows_;
    int columns_;
    int depth_;
    vector<int> data_;
};



/*This is a smart battleship player which makes choices considering maximal apostriori probability */
class Battleship_MAP_player : public IBattleshipGameAlgo
{
public:
    static const int PLAYER_A = 0;
    static const int PLAYER_B = 1;
	//static const int MANY_COORDS_POSSIBLE = 400;

    Battleship_MAP_player();

    ~Battleship_MAP_player() override;

    /****************
     *      API     *
     ****************/

    void setPlayer(int player) override;
    void setBoard(const BoardData& board) override;
    Coordinate attack() override;
    void notifyOnAttackResult(int player, Coordinate crd, AttackResult result) override; // notify on last move result

private:

    /*******************
    *    Queries       *
    *******************/

    bool myShipAt(Coordinate crd) const;
    bool crdHelpsMeInDestroyMode(Coordinate crd) const;
    void turn_sunk_ship_to_obstacle(vector<Coordinate> sunkShipCoords);

    /*******************
    *    Functions     *
    *******************/
    void notifyAttackMiss(        Coordinate crd);
    void notifyAttackHit(         Coordinate crd);
    void notifyAttackSink(        Coordinate crd);
    void notifyOpponentAttackHit( Coordinate crd);
    void notifyOpponentAttackSink(Coordinate crd);

    void initDestroyModeRightAfterSink();


    void constructBoardFromBoardData(const BoardData& board_data);
    void initObstacles();
	void seekHeat_addHeat_tryShipSizes(HeatMap& seekHeatMap, Coordinate crd);
    void computeSeekHeat();
    Coordinate findTopNorthWestrenDestroySessionCoord() const;
    void Battleship_MAP_player::computeDestroyHeatAllOptions(HeatMap& heat_map, Crd orientation) const;
    Crd Battleship_MAP_player::destroySessionOrientation() const;
    HeatMap computeDestroyHeat() const;
	void updateObstacleAt(Coordinate crd);
	void removeHeatAroundObstacle(Coordinate obst, Coordinate ori);
    int player_;
    Board brd;
    vector<Ship> ships;
	vector<int> enemy_ships_left;

	HeatMap heatmap;
    HeatMap obstacles; //in this heatmap 1 means obstacle and 0 means possible
    bool isSeek; // true == SeekMode, false == DestroyMode
    //HeatMap heatmap;
    vector<Coordinate> destroySession;
    HeatMap uncompleteHitSpots;
};


