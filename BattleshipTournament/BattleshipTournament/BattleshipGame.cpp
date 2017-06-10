// BattleshipGame.cpp : Defines the entry point for the console application.
//
#include "BoardClass.h"
#include "IBattleshipGameAlgo.h"


#include "GameManager.h"


/*Main is for running games etc...*/
int main(int argc, char *argv[])
{
	vector<BoardFullData> boards = vector<BoardFullData>();
	vector<DLLData> dlls = vector<DLLData>();
	string dir_path;
	if (!Utils::get_directory_and_cmd_line_args(argc, argv, dir_path))
		return -1;
	if (!Utils::find_all_board_files(dir_path, boards))
		return -1;
	if (!Utils::get_dlls(dir_path, dlls))
		return -1;
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


	manager.mainLoop();

	return 0;
}

