#include "Wordle.hpp"

std::string toUpper(std::string str)
{
	std::string cpy = str;
	for (int i = 0; cpy[i]; i++)
		cpy[i] = toupper(cpy[i]);
	return (cpy);
}

bool	checkYellow(std::string word, std::string answer, UINT index)
{
	int	count = 0;

	for (UINT i = 0; i < STR_LEN; i++)
	{
		if (answer[i] == word[index] && word[i] != answer[i])
			count++;
	}
	for (UINT i = 0; i < index; i++)
	{
		if (word[i] == word[index] && word[i] != answer[i])
			count--;
	}
	return count > 0;
}

void	sendError(std::string debugMessage, std::string boxTitle, std::string boxContent)
{
	if (DEBUG)
	{
		std::cout << debugMessage;
	}
	else
	{
		MessageBoxA(0, boxContent.c_str(), boxTitle.c_str(), MB_OK);
	}
}

bool	validWord(std::string word, std::string* word_list, uint32_t listLenght)
{
	if (word.length() != STR_LEN)
	{
		sendError("ERROR: Word should be 5 letters long\n", "Invalid input", "Word should only be 5 letters long");
		return false;
	}
	for (UINT i = 0; i < STR_LEN; i++)
	{
		if (!isalpha(word[i]))
		{
			sendError("ERROR: Word should only consist of letters\n", "Invalid input", "Words should only consist of lettes");
			return false;
		}
	}
	for (UINT i = 0; i < listLenght; i++)
	{
		if (word == word_list[i])
			return true;
	}
	sendError("ERROR: Word not found in word list\n", "Invalid input", "Word not found in word list");
	return false;
}

bool	handleWord(gameInfo* info, uint8_t y)
{
	for (int x = 0; x < STR_LEN; x++)
	{
		if (info->answer[x] == info->grid[y].str[x])
			info->grid[y].state[x] = GREEN;
		else if (checkYellow(info->grid[y].str, info->answer, x))
			info->grid[y].state[x] = YELLOW;
		else 
			info->grid[y].state[x] = GRAY;
	}
	return (info->grid[y].str == info->answer);
}
