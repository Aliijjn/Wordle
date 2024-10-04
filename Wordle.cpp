#include "Wordle.hpp"

UINT tRand(int modulo)
{
	UINT x = __rdtsc();
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x % modulo;
}

std::string* initWordList(uint32_t* wordListLength)
{
	std::string*	wordList;
	std::ifstream	file("word_list.txt");
	std::string		temp;
	int				i;

	if (!file.is_open())
	{
		MessageBoxA(0, "Are \"answer_list\" and \word_list\" in the same directory as the executable?", "Error opening word list", MB_OK);
		exit(1);
	}
	//get answer_list.txt size and extract all words
	getline(file, temp);
	for (i = 0; temp[i] && !isdigit(temp[i]); i++) {}
	try
	{
		*wordListLength = std::stoi(&temp[i]);
	}
	catch (...)
	{
		file.close();
		MessageBoxA(0, "List length (the first row) of \"word_list\" is either not a valid number or too large", "Error reading word list", MB_OK);
		exit(1);
	}
	if (DEBUG)
		std::cout << "Size of word_list:" << *wordListLength << "\n";
	try
	{
		wordList = new std::string[*wordListLength];
	}
	catch (std::bad_alloc)
	{
		file.close();
		std::cerr << "Memory allocation error :(\n";
		exit(1);
	}
	for (uint32_t i = 0; i < *wordListLength; i++)
	{
		std::getline(file, wordList[i]);
		wordList[i] = toUpper(wordList[i]);
	}
	file.close();
	//get random number and use it as index for answer_list.txt
	return wordList;
}

std::string		getAnswer()
{
	std::ifstream	file("answer_list.txt");
	std::string		temp;
	std::string		answer;
	uint32_t		randomNbr;
	uint32_t		i;

	getline(file, temp);
	for (i = 0; temp[i] && !isdigit(temp[i]); i++) {}
	try
	{
		randomNbr = tRand(std::stoi(&temp[i]));
	}
	catch (...)
	{
		file.close();
		MessageBoxA(0, "List length (the first row) of \"answer_list\" is either not a valid number or too large", "Error reading word list", MB_OK);
		exit(1);
	}
	if (DEBUG)
	{
		std::cout << "Random nbr:" << randomNbr << "\n";
	}
	for (i = 0; i <= randomNbr; i++)
	{
		std::getline(file, answer);
	}
	file.close();
	return toUpper(answer);
}

block* initGrid()
{
	block* result = new block[MAX_TRY]();
	for (int i = 0; i < MAX_TRY; i++)
	{
		memset(result[i].state, WHITE, STR_LEN);
		result[i].str = "\0\0\0\0\0";
	}
	return result;
}

int		main()
{
	gameInfo	info;
	HWND		hwndConsole = GetConsoleWindow();

	info.wordList = initWordList(&info.listLength);
	info.textures = initTextures();

	initRender(VRAM_X, VRAM_Y, SCREEN_X, SCREEN_Y);
	if (hwndConsole && DEBUG == 0) {
		PostMessage(hwndConsole, WM_CLOSE, 0, 0);
	}
	while (1)
	{
		info.grid = initGrid();
		info.answer = getAnswer();
		info.finished = false;

		if (DEBUG)
			std::cout << "ANSWER IS: " << info.answer << "\n";
		renderLoop(&info);
	}
	return 0;
}
