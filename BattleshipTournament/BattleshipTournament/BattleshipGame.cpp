// BattleshipGame.cpp : Defines the entry point for the console application.
//
#include "BoardClass.h"
#include "TournamentManager.h"
#include "GameManager.h"
#include "Logger.h"

///*Main is for running games etc...*/
int main(int argc, char *argv[])
{
	//-- number of threads
	auto numOfThreads = Utils::DEFAULT_NUMBER_OF_THREADS; //todo
	Utils::ParseConfigFile(numOfThreads);

	vector<BoardFullData> boards = vector<BoardFullData>();
	vector<DLLData> dlls = vector<DLLData>();
	string dir_path;
	if (!Utils::get_directory_and_cmd_line_args(argc, argv, dir_path, numOfThreads))
		return -1;

	// starting logger
	Logger::getLogger();
	LOGGER.init(dir_path);


	if (!Utils::find_all_board_files(dir_path, boards))
		return -1;
	if (!Utils::get_dlls(dir_path, dlls))
		return -1;

	cout << "Number of legal players: " << dlls.size() << endl;
	cout << "Number of legal boards: " << boards.size() << endl;


	//-- create tournament
	{
		TournamentManager tm(numOfThreads, dlls, boards);
		tm.runTournament();
	}
	return 0;
}

/* example for using ThreadPool*/
//void silly(int n)
//{
//    // A silly job for demonstration purposes
//    std::cerr << "Sleeping for " << n << " seconds" << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(n));
//}
//
//int main()
//{
//    // Create two threads
//    ThreadPool p(2);
//
//    // Assign them 4 jobs
//    p.doJob(std::bind(silly, 1));
//    p.doJob(std::bind(silly, 2));
//    p.doJob(std::bind(silly, 3));
//    p.doJob(std::bind(silly, 4));
//}


