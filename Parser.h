#pragma once

#define BUFFER_MAX_LENGTH 100

class Parser
{
public:
    Parser(string filename);
    ~Parser();
    bool hasMoreCommands();
    void advance();
    COMMAND_TYPE commandType();
    string arg1();
    int arg2();
    
    bool isNotCommand();
    void trim();

    void printLine();

private:
    ifstream vmfile;
    string currentLine;
    char buf[BUFFER_MAX_LENGTH];
};

