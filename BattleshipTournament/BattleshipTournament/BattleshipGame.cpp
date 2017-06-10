// BattleshipGame.cpp : Defines the entry point for the console application.
//
#include "BoardClass.h"
#include "TournamentManager.h"
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
	
	//-- number of threads
	auto numOfThreads = 4; //todo

	//-- create tournament
	TournamentManager tm(numOfThreads, dlls, boards);
	tm.runTournament();
	return 0;
}

