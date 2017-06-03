#pragma once


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

//#define DEBUG_BUILD

#ifdef DEBUG_BUILD
#define DEBUG(x) do { std::cerr << x << endl; } while (0)
#else
#define DEBUG(x) do {} while (0)
#endif

using namespace std;


//const string ERROR_FINDING_PATH = "error finding path";

class Utils
{
	public:		
		static void gotoxy(int x, int y);
		static void setcolor(WORD color);
		static void setForeGroundColor(int ForeGroundColor);
		static bool Utils::is_valid_dir_path(const char *pathname);
		static COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
		static void Utils::updateBoardPrint(int player_color, COORD hit_coord, char current, int hit_color);
		static string find_path(const string& path_expr_to_find, bool& find_file);
		static void set_quiet(bool is_quiet);
		static bool get_quiet();
		static void set_delay(int delay);
		static string Utils::find_file(const string& path_expr_to_find, int player_id, bool at_least_two, bool& file_found);
		static void Utils::ShowConsoleCursor(bool showFlag);
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
		//static const string  FILE_NOT_FOUND;

	private:
		Utils(){} //dissallow crating instances of this class
		static bool QUIET;
		static int MILISECONDS_PRINT_DELAY;
};
