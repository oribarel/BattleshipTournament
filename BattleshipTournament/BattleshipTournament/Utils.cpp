#include "Utils.h"
#include "Boardclass.h"
#include <iso646.h>
#include <map>
#include "Logger.h"
#include <regex>
using namespace std;

//const string Utils::FILE_NOT_FOUND = "file not found";
bool Utils::QUIET = false;
int Utils::MILISECONDS_PRINT_DELAY = Utils::DEFAULT_PRINT_DELAY;;

void Utils::gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	return;
}

void Utils::setcolor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	return;
}

void Utils::setForeGroundColor(int ForeGroundColor)
{
	int color = ForeGroundColor;
	setcolor(color);
}

COORD Utils::GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		// The function failed. Call GetLastError() for details.
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

void Utils::updateBoardPrint(int player_color, Coordinate hit_coord, char current, int hit_color, int num_of_rows)
{
	if (!Utils::QUIET)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(MILISECONDS_PRINT_DELAY));
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD cursor_coord = GetConsoleCursorPosition(h);
		gotoxy(PRINT_INDENT + hit_coord.col * 2 - 2, hit_coord.row+(num_of_rows+1)*(hit_coord.depth-1));
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), player_color); cout << "@";
		gotoxy(PRINT_INDENT + hit_coord.col * 2 - 2, hit_coord.row+ (num_of_rows + 1)*(hit_coord.depth - 1));
		std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_START_ANIMATION_DELAY));
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), player_color); cout << " ";
		gotoxy(PRINT_INDENT + hit_coord.col * 2 - 2, hit_coord.row+ (num_of_rows + 1)*(hit_coord.depth - 1));
		std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_END_ANIMATION_DELAY));
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), hit_color); cout << current;
		gotoxy(cursor_coord.X, cursor_coord.Y);
		setcolor(WHITE_COLOR);
	}
}

void Utils::set_quiet(bool is_quiet)
{
	Utils::QUIET = is_quiet;
}
bool Utils::get_quiet()
{
	return Utils::QUIET;
}
void Utils::set_delay(int delay)
{
	Utils::MILISECONDS_PRINT_DELAY = delay;
}


pair<bool, string> Utils::parse_command_line_arguments(int argc, char *argv[], bool& is_working_dir, int& num_of_threads)
{
	string dir_path = ".";
	is_working_dir = (argc == 1);
	/* parsing command line arguments */
	for (int i = 1; i < argc; i++)
	{
		string argument = argv[i];
		if (argument == "-quiet")
		{
			Utils::set_quiet(true);
			if (i == 1)
				is_working_dir = true;
		}
		if (argument == "-delay")
		{
			Utils::set_delay(atoi(argv[i + 1]));
			if (i == 1)
				is_working_dir = true;
		}
		if (argument == "-threads")
		{
			num_of_threads = atoi(argv[i + 1]);
			if (i == 1)
				is_working_dir = true;
		}
	}
	if (!is_working_dir)
	{
		if (!Utils::is_valid_dir_path(argv[1]))
		{
			cout << "Wrong path: " << argv[1] << endl; //ReqPrnt
			return make_pair(false, dir_path);
		}
		dir_path = string(argv[1]);
	}
	return make_pair(true, dir_path);
}

/* returns true if could extract directory path, and false otherwise */
bool Utils::get_directory_and_cmd_line_args(int argc, char *argv[], string& dir_path, int& numOfThreads)
{
	bool is_working_dir = false;
	pair<bool, string> parser_ret_val = parse_command_line_arguments(argc, argv, is_working_dir, numOfThreads);
	char cCurrentPath[FILENAME_MAX];
	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		cout << "Error: can't extract current directory path" << endl;
		return false;

	}
	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	dir_path = is_working_dir ? string(cCurrentPath) : string(argv[1]);
	if (parser_ret_val.first == false)
		return false;
	return true;
}


bool Utils::find_all_board_files(const string& dir_path, vector<BoardFullData>& boards)
{
	/* find files */
	string path_expr_to_find = dir_path + "\\*.sboard";
	WIN32_FIND_DATAA fileData;
	HANDLE hFind;
	string retVal = "";
	bool board_found = true;
	hFind = FindFirstFileA(path_expr_to_find.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			board_found = false;
	}
	else
	{
		do
		{
			BoardFullData brd_full_data;
			if(brd_full_data.initialize_board(dir_path + "\\" + fileData.cFileName))
				boards.push_back(brd_full_data);
		} while (FindNextFileA(hFind, &fileData));
		FindClose(hFind);
	}	
	if (!board_found)
	{
		cout << "No board files (*.sboard) looking in path: " << dir_path << endl; //ReqPrnt
		LOGGER.log("No board files (*.sboard) looking in path: "s + string(dir_path));
		string dummy_str;
		/* even if dll is missing for both players an error message will be printed only once, intentionally*/
	}
	return board_found;
}



bool Utils::load_dll (string dir_path, DLLData& dll_data)
{
	// Load dynamic library
	char full_path[_MAX_PATH];
	string full_dll_path = dir_path + "\\" + dll_data.file_name;
	_fullpath(full_path, full_dll_path.c_str(), _MAX_PATH);
	HINSTANCE hDll = LoadLibraryA(full_path); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll)
	{
		LOGGER.log("ERROR: Cannot load dll: "s + string(full_path) + " this file will be skipped");
		return false;
	}

	/* create A instance*/
	// Get function pointer
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoFunc)();
	dll_data.algo_func = reinterpret_cast<GetAlgoFunc>(GetProcAddress(hDll, "GetAlgorithm"));
	if (!dll_data.algo_func)
	{
		LOGGER.log("ERROR: Cannot load dll: "s + string(full_path) + " this file will be skipped");
	}
	else
	{
		return true;
	}
	return false;
	
}


bool Utils::get_dlls (string dir_path, vector<DLLData>& players)
{
	string path = dir_path;
	if (dir_path.compare(".") == 0)
		path = "Working Directory";
	string s_dll = "\\*.dll"; // only .dll endings
	bool file_found = true;
	find_all_dll_files(dir_path + s_dll, file_found, players, dir_path);
	if (!file_found or players.size() <= 1)
	{
		cout << "Missing algorithm (dll) files looking in path: " << path << " (needs at least two)" << endl;
		LOGGER.log("Missing algorithm(dll) files looking in path : "s + path + " (needs at least two)"s);
		return false;
	}
	return true;
}


void Utils::find_all_dll_files(const string& path_expr_to_find,bool& file_found, vector<DLLData>& players, string dir_path)
{
	WIN32_FIND_DATAA fileData;
	string retVal = "";
	HANDLE hFind = FindFirstFileA(path_expr_to_find.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			file_found = false;
	}
	else
	{
		do
		{
			DLLData dll_data(fileData.cFileName);
			if(load_dll(dir_path, dll_data))
				players.push_back(dll_data);
		} while (FindNextFileA(hFind, &fileData));
		FindClose(hFind);
	}
}

string Utils::find_path(const string& path_expr_to_find, bool& find_file)
{
	WIN32_FIND_DATAA fileData;
	HANDLE hFind;
	string retVal = "";
	find_file = true;
	hFind = FindFirstFileA(path_expr_to_find.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			find_file = false;
		//else
	    //	retVal = ERROR_FINDING_PATH;
	}

	else
	{
		retVal = fileData.cFileName;
		FindClose(hFind);
	}
	return retVal;
}


string Utils::find_file(const string& path_expr_to_find, int player_id, bool at_least_two, bool& file_found)
{
	WIN32_FIND_DATAA fileData;
	HANDLE hFind;
	string first_file, second_file; // lexiogrphic order
	string retVal = "", tmp_str = "";
	file_found = true;
	hFind = FindFirstFileA(path_expr_to_find.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			file_found = false;
		//else
		//	retVal = ERROR_FINDING_PATH;
	}

	else
	{
		first_file = fileData.cFileName;
		second_file = fileData.cFileName;
		do
		{
			tmp_str = fileData.cFileName;
			if (first_file.compare(tmp_str) > 0)
			{
				second_file = first_file;
				first_file = tmp_str;
			}
			else if (second_file.compare(tmp_str) > 0 || first_file.compare(second_file) == 0)
			{
				second_file = tmp_str;
			}

		} while (FindNextFileA(hFind, &fileData));
		if (at_least_two && first_file.compare(second_file) == 0) // make sure there exist at least two different files
			file_found = false;
		if (player_id == PLAYER_A)
			retVal = first_file;
		else
			retVal = second_file;
		FindClose(hFind);
	}
	return retVal;
}

bool Utils::is_valid_dir_path(const char *pathname)
{
	struct stat info;

	if ((stat(pathname, &info) != 0) || !(info.st_mode & S_IFDIR))
		return false;
	return true;
}

void Utils::ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

bool Utils::doesCoordsEqual(Coordinate c1, Coordinate c2)
{
	return c1.row == c2.row and c1.col == c2.col and c1.depth == c2.depth;
}


bool Utils::ParseConfigFile(int& number_of_threads)
{
	const char* path = "config.xml";
	std::ifstream infile(path);
	std::string line;
	std::regex number_regex("[0-9]+");
	std::smatch number_match;
	if (infile.is_open())
	{
		// read dimenstions
		while (safeGetline(infile, line))
		{
			if (line.find("<threads>"s) != std::string::npos)
			{
				if (std::regex_search(line, number_match, number_regex))
				{
					number_of_threads = std::stoi(number_match[0]);
					return true;
				}

			}
		}
	}
	return false;
}

