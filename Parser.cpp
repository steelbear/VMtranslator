#include "stdafx.h"
#include "Parser.h"

Parser::Parser(string filename)
{
	if (filename.find(".vm") == filename.npos)
	{
		filename += ".vm";
	}

	vmfile = ifstream(filename, fstream::in);
}

Parser::~Parser()
{
	vmfile.close();
}

bool Parser::hasMoreCommands()
{
	return !vmfile.eof();
}


void Parser::advance()
{
	do {
		vmfile.getline(buf, BUFFER_MAX_LENGTH);
		currentLine = string(buf);
		trim();
	} while (!vmfile.eof() && isNotCommand());
}


COMMAND_TYPE Parser::commandType()
{
	string command(currentLine);
	command.erase(find_if(command.begin(), command.end(), [](char c) {
		return c == ' ';
	}), command.end());

	if (command == "push")
	{
		return COMMAND_TYPE::C_PUSH;
	}
	else if (command == "pop")
	{
		return COMMAND_TYPE::C_POP;
	}
	else if (command == "label")
	{
		return COMMAND_TYPE::C_LABEL;
	}
	else if(command == "goto")
	{
		return COMMAND_TYPE::C_GOTO;
	}
	else if(command == "if-goto")
	{
		return COMMAND_TYPE::C_IF;
	}
	else if(command == "function")
	{
		return COMMAND_TYPE::C_FUNCTION;
	}
	else if(command == "return")
	{
		return COMMAND_TYPE::C_RETURN;
	}
	else if(command == "call")
	{
		return COMMAND_TYPE::C_CALL;
	}
	else
	{
		return COMMAND_TYPE::C_ARITHMETIC;
	}
}


string Parser::arg1()
{
	COMMAND_TYPE ctype = commandType();
	string command(currentLine);

	if (ctype == COMMAND_TYPE::C_ARITHMETIC)
	{
		command.erase(find_if(command.begin(), command.end(), [](char c) {
			return c == ' ';
		}), command.end());
		
		return command;
	}
	else if (ctype == COMMAND_TYPE::C_RETURN)
	{
		throw "arg1 - Return can\'t use arg1\n";
	}
	else
	{
		command.erase(command.begin(), find_if(command.begin(), command.end(), [](char c) {
			return isspace(c);
		}) + 1);

		auto trimRight = find_if(command.rbegin(), command.rend(), [](char c) {
			return isspace(c);
		}).base();

		if (trimRight != command.begin()) {
			command.erase(trimRight - 1, command.end());
		}

		return command;
	}
}


int Parser::arg2()
{
	COMMAND_TYPE ctype = commandType();
	string command(currentLine);

	if (ctype == COMMAND_TYPE::C_PUSH || ctype == COMMAND_TYPE::C_POP
		|| ctype == COMMAND_TYPE::C_FUNCTION || ctype == COMMAND_TYPE::C_CALL)
	{
		command.erase(command.begin(), find_if(command.rbegin(), command.rend(), [](char c) {
			return c == ' ';
		}).base());

		return stoi(command);
	}
	else
	{
		throw "arg2 - only Push and Pop operator have second argument\n";
	}
}


void Parser::printLine()
{
	switch (commandType())
	{
	case COMMAND_TYPE::C_ARITHMETIC:
		cout << "arithmetic : " << arg1() << endl;
		break;
	case COMMAND_TYPE::C_PUSH:
		cout << "push : " << arg1() << "[" << arg2() << "]" << endl;
		break;
	case COMMAND_TYPE::C_POP:
		cout << "pop : " << arg1() << "[" << arg2() << "]" << endl;
		break;
	case COMMAND_TYPE::C_GOTO:
		cout << "goto : " << arg1() << endl;
		break;
	case COMMAND_TYPE::C_IF:
		cout << "if : " << arg1();
		break;
	case COMMAND_TYPE::C_LABEL:
		cout << "label (" << arg1() << ")" << endl;
		break;
	case COMMAND_TYPE::C_FUNCTION:
		cout << "function : " << arg1() << "(" << arg2() << ")" << endl;
		break;
	case COMMAND_TYPE::C_CALL:
		cout << "call : " << arg1() << "(" << arg2() << ")" << endl;
		break;
	case COMMAND_TYPE::C_RETURN:
		cout << "return function" << endl;
		break;
	}
}

bool Parser::isNotCommand()
{
	return currentLine.empty();
}

void Parser::trim()
{
	currentLine.erase(currentLine.begin(), find_if(currentLine.begin(), currentLine.end(), [](char c) {
		return !isspace(c);
	}));

	currentLine.erase(find_if(currentLine.begin(), currentLine.end(), [](char c) {
		return c == '/';
	}), currentLine.end());

	currentLine.erase(find_if(currentLine.rbegin(), currentLine.rend(), [](char c) {
		return !isspace(c);
	}).base(), currentLine.end());
}