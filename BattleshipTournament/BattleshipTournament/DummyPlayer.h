#pragma once
#include "IBattleshipGameAlgo.h"

class DummyPlayer : public IBattleshipGameAlgo
{
public:
	void setPlayer(int player) override {}		// called every time the player changes his order
	void setBoard(const BoardData& board) override {}	// called once at the beginning of each new game
	Coordinate attack() override { return Coordinate(-1, -1, -1); }					// ask player for his move
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override {} // last move result
};
