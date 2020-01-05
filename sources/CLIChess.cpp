#include <iostream>
#include <Windows.h>
#include "GameManager.h"
#include "CLIChessExceptions.h"

enum class CLICommand {NewGame, Quit, Save, Load, Move, TakeBack, UNK};

CLICommand getCommand(const std::string& cmd);
bool promptForYesNo(std::string promptMsg);

void printBoardFrame();
void printStartInfo();
void printMainMenu();
void printGameStartMenu();
void printQuitInfo();

void clearScreen(char fill = ' ');

int main()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleColor);
	SetConsoleCP(437);
	SetConsoleOutputCP(437);
	GameManager gm;
	bool quitGame = false;
	bool gameOngoing = false;

	std::string userInput;

	printStartInfo();
	printMainMenu();
	std::cout << std::endl;
	printGameStartMenu();
	std::cout << std::endl;

	while (!quitGame) {
		// 1. Print the proper input prompt:
		if (gameOngoing) {
			gm.printBoard();
			std::cout << gm.inTurnPlayer() + " to move: ";
		}
		else
			std::cout << "[CLIChess] >> ";

		// 2. Handle the user input: 
		std::getline(std::cin, userInput);
		// The clearScreen -call ensures a nice, consistent view of the board and no text flooding:
		clearScreen();

		switch (getCommand(userInput)) {

		case (CLICommand::NewGame):
			gm.restart();
			if (gameOngoing == false) {
				gameOngoing = true;
			}
			printBoardFrame();
			break;

		case (CLICommand::Quit):
			quitGame = true;
			break;

		case (CLICommand::Save):
			if (gameOngoing) {
				if(gm.save(userInput.substr(2, std::string::npos)))
					std::cout << "Save successful" << std::endl;
				else {
					std::cout << "An unexpected system error occured:" << std::endl;
					std::cout << gm.getMsg() << std::endl;
					std::cout << "The operation was not performed." << std::endl;
				}
			}
			else
				std::cout << "No ongoing game. Saving not possible." << std::endl;
			break;

		case (CLICommand::Load):
			gameOngoing = true;
			if (gm.load(userInput.substr(2, std::string::npos))) {
				clearScreen();
				std::cout << "Finished loading." << std::endl;
			}
			else {
				std::cout << "An unexpected system error occured:" << std::endl;
				std::cout << gm.getMsg() << std::endl;
				std::cout << "The operation was not performed completely." << std::endl;
			}
			break;

		case (CLICommand::Move):
			if (gameOngoing) {
				if (!gm.makeMove(userInput)) {
					std::cout << "The move could not be made:" << std::endl;
					std::cout << gm.getMsg() << std::endl;
					std::cout << std::endl;
				}
				else printBoardFrame();

				if (gm.isCheckmate() || gm.isStalemate()) {
					gm.printBoard();
					std::cout << "The game ended.";
					std::cout << gm.getMsg() << std::endl;
					gameOngoing = false;

					if (promptForYesNo("Would you like to save the game notation in a readable format? [y/n]: ")) {
						bool success = false;

						while (!success){
							std::cout << "Enter the save file name: ";
							std::getline(std::cin, userInput);
							if (gm.save(userInput)) {
								std::cout << "Game notation saved." << std::endl;
								success = true;
							}
							else {
								std::cout << gm.getMsg() << std::endl;
								std::cout << "Unable to save the game." << std::endl;
								success = !promptForYesNo("Would you like to try again? [y/n]: ");
							}
						}
					}
				}
			}
			break;

		case (CLICommand::TakeBack):
			if (gameOngoing) {
				try {
					if (gm.takeBack(std::stoi(userInput.substr(2, std::string::npos)))) {
						std::cout << "Takeback successful";
						printBoardFrame();
					}
					else {
						std::cout << "An error occured:" << std::endl;
						std::cout << gm.getMsg() << std::endl;
						std::cout << "The operation was not performed." << std::endl;
					}
				}
				catch (std::logic_error e) {
					std::cout << "PARSE ERROR" << std::endl;
					std::cout << "[ " + userInput + "]:" << std::endl;
					std::cout << "Could not parse the given number of takebacks." << std::endl;
				}
			}
			else
				std::cout << "No ongoing game. Taking back not possible." << std::endl;
			break;

		case (CLICommand::UNK):
			std::cout << "Unknown command." << std::endl;
			break;
		}
	}

	printQuitInfo();
}

// getCommand: string -> CLICommand
// Returns the type of the user inputted command.
//
CLICommand getCommand(const std::string& cmd) {
	int len = cmd.size();
	if (len == 0)
		return CLICommand::UNK;

	switch (cmd[0]) {
		
		case ('n'):
			if (len == 1)
				return CLICommand::NewGame;
			else
				return CLICommand::UNK;
		
		case ('q'):
			if (len == 1)
				return CLICommand::Quit;
			else
				return CLICommand::UNK;
		
		case ('s'):
			if (len > 2 && cmd[1] == ' ')
				return CLICommand::Save;
			else
				return CLICommand::UNK;

		case ('t'):
			if (len > 2 && cmd[1] == ' ')
				return CLICommand::TakeBack;
			else
				return CLICommand::UNK;
		
		case ('l'):
			if (len > 2 && cmd[1] == ' ')
				return CLICommand::Load;
			else
				return CLICommand::UNK;
		
		default:
			return CLICommand::Move;
	}
}

// promptForYesNo: prompt message -> bool
// Prompts the user with the given prompt message
// until the user enters either 'y' or 'n'.
// returns true if user entered 'y'.
// returns false if the user entered 'n'.
bool promptForYesNo(std::string promptMsg) {
	std::string userInput;

	while (true) {
		std::cout << promptMsg;
		std::getline(std::cin, userInput);
		if (!userInput.compare("y")) {
			return true;
		}
		else if (!userInput.compare("n")) {
			return false;
		}
		else std::cout << "Invalid input. Please, try again." << std::endl;
	}
}

void printBoardFrame() {
	std::cout << std::endl << std::endl << std::endl;
}

void printStartInfo() {
	std::cout << "Welcome to CLIChess!" << std::endl;
	std::cout << "CLIChess is a two player chess program." << std::endl;
	std::cout << "The players give move orders in a slightly simplified algebraic chess notation form:" << std::endl;
	std::cout << "The players do not have to give explicit check, checkmate, En Passant or promotion orders." << std::endl;
	std::cout << "Apart from these minor details, the used notation is a standard algebraic chess notation." << std::endl;
	std::cout << std::endl;
	std::cout << "Happy gaming!" << std::endl;
}

void printMainMenu() {
	std::cout << std::endl;
	std::cout << "Available commands:" << std::endl;
	std::cout << "n : starts a new game" << std::endl;
	std::cout << "q : quits CLIChess" << std::endl;
	std::cout << "l file: loads a game from the given file" << std::endl;
	std::cout << "example: \"l my_file01.clc\" loads the previously saved game from my_file01.clc" << std::endl << std::endl;
}

void printGameStartMenu() {
	std::cout << "During a chess game you can also save the current game by giving the command \"s file\"" << std::endl;
	std::cout << "For example: \"s mygame_1.chs\"" << std::endl;
	std::cout << "\"t n\" during a game takes back n moves. Example: \"t 5\" takes back 5 moves." << std::endl;
}

void printQuitInfo() {
	std::cout << "Terminating CLIChess." << std::endl;
}

// clearScreen: char -> void
// fills the entire console screen with the given char.
// If no character is given, simply clears the screen.
//
// NOTE: this function renders the program unportable,
// due to the reliance of windows-specific console commands.
//
// Shamelessly ripped off of StackOverflow:
// https://stackoverflow.com/questions/5866529/how-do-we-clear-the-console-in-assembly/5866648#5866648
void clearScreen(char fill) {
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}