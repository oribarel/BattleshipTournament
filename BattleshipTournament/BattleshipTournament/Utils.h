#pragma once

#include "IBattleshipGameAlgo.h"
#include <windows.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <vector>


/*

Color      Background    Foreground
---------------------------------------------
Black            0           0
Blue             1           1
Green            2           2
Cyan             3           3
Red              4           4
Magenta          5           5
Brown            6           6
White            7           7
Gray             -           8
Intense Blue     -           9
Intense Green    -           10
Intense Cyan     -           11
Intense Red      -           12
Intense Magenta  -           13
Yellow           -           14
Intense White    -           15



*/


// adding 2D print support for debugging purposes
//#define _2D_

//#define DEBUG_BUILD

#ifdef DEBUG_BUILD
#define DEBUG(x) do { std::cerr << x << endl; } while (0)
#else
#define DEBUG(x) do {} while (0)
#endif

struct BoardFullData;
struct Coordinate;
using namespace std;

typedef IBattleshipGameAlgo *(*GetAlgoFunc)();
struct DLLData
{
	string file_name;
	GetAlgoFunc algo_func;
	DLLData(string file_name_) : file_name(file_name_), algo_func(nullptr) {}
};

//const string ERROR_FINDING_PATH = "error finding path";

class Utils
{
	public:		
		static void gotoxy(int x, int y);
		static void setcolor(WORD color);
		static void setForeGroundColor(int ForeGroundColor);
		static bool Utils::is_valid_dir_path(const char *pathname);
		static COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
		static void Utils::updateBoardPrint(int player_color, Coordinate hit_coord, char current, int hit_color, int num_of_rows);
		static bool Utils::find_all_board_files(const string& dir_path, vector<BoardFullData>& boards);
		static string find_path(const string& path_expr_to_find, bool& find_file);
		static void set_quiet(bool is_quiet);
		static bool get_quiet();
		static void set_delay(int delay);
		static string Utils::find_file(const string& path_expr_to_find, int player_id, bool at_least_two, bool& file_found);
		static void Utils::ShowConsoleCursor(bool showFlag);
		static bool Utils::doesCoordsEqual(Coordinate c1, Coordinate c2);		
		static bool Utils::load_dll(string dir_path, DLLData& dll_data);
		static bool Utils::get_dlls(string dir_path, vector<DLLData>& players);
		static void Utils::find_all_dll_files(const string& path_expr_to_find, bool& file_found, vector<DLLData>& players, string dir_path);
		static bool Utils::get_directory_and_cmd_line_args(int argc, char *argv[], string& dir_path, int& numOfThreads);
		static pair<bool, string> Utils::parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir, int& num_of_threads);
		static bool Utils::ParseConfigFile(int& number_of_threads);
		static const int DEFAULT_PRINT_DELAY = 2000;
		static const int DEFAULT_START_ANIMATION_DELAY = 350;
		static const int DEFAULT_END_ANIMATION_DELAY = 500;
		static const int WHITE_COLOR = 7;
		static const int MAGNETA_COLOR = 5;
		static const int RED_COLOR = 4;
		static const int GREEN_COLOR = 2;
		static const int PRINT_INDENT = 4;
		static const char HIT_SIGN = 'X';
		static const int PLAYER_A = 0;
		static const int PLAYER_B = 1;
		static const int NUMBER_OF_SHIP_TYPES = 4;
		static const int DEFAULT_NUMBER_OF_THREADS = 4;
		//static const string  FILE_NOT_FOUND;

	private:
		Utils(){} //dissallow crating instances of this class
		static bool QUIET;
		static int MILISECONDS_PRINT_DELAY;
};
