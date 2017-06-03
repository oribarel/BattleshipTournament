#pragma once

#include <string> // Actually not required here - but here due to students' request

/*Notice:
 * Usually it is bad practice to have several classes/structs in the same header.
 * Here we use this practice only for simplicity considerations,
 * since we only want to have 1 common file in this excersice.
 * You should avoid from doing so yourself!
 */

enum class AttackResult
{
	Miss,
	Hit,
	Sink
};

struct Coordinate
{
	int row, col, depth;
	Coordinate(int row_, int col_, int depth_) : row(row_), col(col_), depth(depth_) {}
};

/*This is a wrapper to the Board's Data
 * You should derive from it, make sure you set all protected members in the derived class.
 */
class BoardData
{
public:
	virtual ~BoardData() = default;
	virtual char charAt(Coordinate c) const = 0; //returns only selected players' chars

	int rows()  const { return _rows; }
	int cols()  const { return _cols; }
	int depth() const { return _depth; }
protected:
	int _rows = 0; // make sure you set all protected members in the derived class.
	int _cols = 0; // make sure you set all protected members in the derived class.
	int _depth = 0; // make sure you set all protected members in the derived class.
};

class IBattleshipGameAlgo
{
public:
	virtual ~IBattleshipGameAlgo() = default;
	virtual void setPlayer(int player) = 0;				// called every time the player changes his order
	virtual void setBoard(const BoardData& board) = 0;	// called once at the beginning of each new game
	virtual Coordinate attack() = 0;					// ask player for his move
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result) = 0; // last move result
};

#ifdef ALGO_EXPORTS										// A flag defined in this project's Preprocessor's Definitions
#define ALGO_API extern "C" __declspec(dllexport)		// If we build - export
#else
#define ALGO_API extern "C" __declspec(dllimport)		// If someone else includes this .h - import
#endif

/* Please Notice:
 * The derived class of IBattleshipGameAlgo allocates a new instance, returns a raw pointer, and IS NOT respoinsible for deleting it
 * The Game Manager MUST take this pointer, and put in in an std::unique_ptr<IBattleshipGameAlgo>.
 * The reason that we don't return a unique_ptr instead, is that it is bad practice to export cpp methods due to their name mangling.
 * When working with shared objects (dlls), the interface must be a C interface.
 */
ALGO_API IBattleshipGameAlgo* GetAlgorithm(); // This method must be implemented in each player(algorithm) .cpp file
