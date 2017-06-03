// BattleshipGame.cpp : Defines the entry point for the console application.
//
#include "BoardClass.h"
#include "IBattleshipGameAlgo.h"


#include "GameManager.h"


/*Main is for running games etc...*/
int main(int argc, char *argv[])
{

	GameManager manager;

	/* The following line does:
	 *  - finds files
	 *  - checks board validity
	 *  - manager finds the players' ships
	 *  - manager initializes players
	 */
	bool success = manager.initialize(argc, argv);
	if (!success)
		return -1;

	manager.setPlayersBoards();

	manager.mainLoop();

	return 0;
}

