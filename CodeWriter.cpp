#include "stdafx.h"
#include "CodeWriter.h"

const string CodeWriter::pushD = "@SP\n"
                                 "A=M\n"
                                 "M=D\n"
                                 "@SP\n"
                                 "M=M+1\n";

const string CodeWriter::popD = "@SP\n"
                                "M=M-1\n"
                                "A=M\n"
                                "D=M\n";

map<string, string> const CodeWriter::labelTable = {
    {"local", "LCL"},
    {"argument", "ARG"},
    {"this", "THIS"},
    {"that", "THAT"},
};

CodeWriter::CodeWriter(string filepath)
{
    asmfile = ofstream(filepath, fstream::out);
    index = 0;
}

CodeWriter::~CodeWriter()
{
    if (asmfile.is_open())
    {
        asmfile.close();
    }
}

void CodeWriter::setFileName(string newFilename)
{
    if (newFilename.find(".vm") != string::npos)
    {
        newFilename.erase(newFilename.end() - 3, newFilename.end());
    }

    newFilename.erase(newFilename.begin(), find_if(newFilename.rbegin(), newFilename.rend(), [](char c) {
        return c == '\\' || c == '/';
    }).base());

    filename = newFilename;
}

void CodeWriter::writerInit()
{
    asmfile <<
        "@256\n" // SP = 256
        "D=A\n"
        "@SP\n"
        "M=D\n";
    writeCall("Sys.init", 0);
    asmfile <<
        "(COMEBACK)\n" // gt, eq, lt Ã³¸®
        "@R13\n"
        "A=M\n"
        "0;JMP\n"
        "(EQ)\n"
        "@TRUE\n"
        "D;JEQ\n"
        "@FALSE\n"
        "0;JMP\n"
        "(GT)\n"
        "@TRUE\n"
        "D;JGT\n"
        "@FALSE\n"
        "0;JMP\n"
        "(LT)\n"
        "@TRUE\n"
        "D;JLT\n"
        "@FALSE\n"
        "0;JMP\n"
        "(TRUE)\n"
        "D=-1\n"
        "@COMEBACK\n"
        "0;JMP\n"
        "(FALSE)\n"
        "D=0\n"
        "@COMEBACK\n"
        "0;JMP" << endl;
}

void CodeWriter::makeSysInit()
{
    writeFunction("Sys.init", 0);
}

void CodeWriter::writeArithmetic(string command)
{
    if (command == "add")
    {
        asmfile <<
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M" << endl;
    }
    else if (command == "sub")
    {
        asmfile <<
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D" << endl;
    }
    else if (command == "neg")
    {
        asmfile <<
            "@SP\n"
            "A=M-1\n"
            "M=-M" << endl;
    }
    else if (command == "eq")
    {
        asmfile <<
            "@EQ" + to_string(index) + "\n"
            "D=A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@EQ\n"
            "0;JMP\n"
            "(EQ" + to_string(index) + ")\n"
            "@SP\n"
            "A=M-1\n"
            "M=D" << endl;
        index++;
    }
    else if (command == "gt")
    {
        asmfile <<
            "@GT" + to_string(index) + "\n"
            "D=A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@GT\n"
            "0;JMP\n"
            "(GT" + to_string(index) + ")\n"
            "@SP\n"
            "A=M-1\n"
            "M=D" << endl;
        index++;
    }
    else if (command == "lt")
    {
        asmfile <<
            "@LT" + to_string(index) + "\n"
            "D=A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@LT\n"
            "0;JMP\n"
            "(LT" + to_string(index) + ")\n"
            "@SP\n"
            "A=M-1\n"
            "M=D" << endl;
        index++;
    }
    else if (command == "and")
    {
        asmfile <<
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M" << endl;
    }
    else if (command == "or")
    {
        asmfile <<
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M" << endl;
    }
    else if (command == "not")
    {
        asmfile <<
            "@SP\n"
            "A=M-1\n"
            "M=!M" << endl;
    }
    else
    {
        throw "writeArithmetic - invaild operator : " + command + "\n";
    }
}

void CodeWriter::writePushPop(COMMAND_TYPE command, string segment, int index)
{
    if (command == COMMAND_TYPE::C_PUSH)
    {
        writeLoadMemory(segment, index);
        asmfile <<
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1" << endl;
    }
    else if (command == COMMAND_TYPE::C_POP)
    {
        writeLoadMemory(segment, index);
        asmfile <<
            "D=A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@R13\n"
            "A=M\n"
            "M=D" << endl;
    }
    else
    {
        throw "writePushPop - invaild parameter : command\nmust use C_PUSH or C_POP\n";
    }
}

void CodeWriter::writeLabel(string label)
{
    asmfile <<
        "(" + filename + "." + label + ")" << endl;
}

void CodeWriter::writeGoto(string label)
{
    asmfile <<
        "@" + filename + "." + label + "\n"
        "0;JMP" << endl;
}

void CodeWriter::writeIf(string label)
{
    asmfile <<
        "@SP\n"
        "M=M-1\n"
        "A=M\n"
        "D=M\n"
        "@" + filename + "." + label + "\n"
        "D;JNE" << endl;
}

void CodeWriter::writeCall(string functionName, int numArgs)
{
    asmfile <<
        "@FUNCTIONBACK" + to_string(index) + "\n"
        "D=A\n"
        + pushD +
        "@LCL\n"
        "D=M\n"
        + pushD +
        "@ARG\n"
        "D=M\n"
        + pushD +
        "@THIS\n"
        "D=M\n"
        + pushD +
        "@THAT\n"
        "D=M\n"
        + pushD +
        "@SP\n"
        "D=M\n"
        "@" + to_string(numArgs) + "\n"
        "D=D-A\n"
        "@5\n"
        "D=D-A\n"
        "@ARG\n"
        "M=D\n"
        "@SP\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n"
        "@" + functionName + "\n"
        "0;JMP\n"
        "(FUNCTIONBACK" + to_string(index) + ")" << endl;
    index++;
}

void CodeWriter::writeReturn()
{
    asmfile <<
        "@LCL\n"
        "D=M\n"
        "@R13\n"
        "M=D\n"
        "@5\n"
        "A=D-A\n"
        "D=M\n"
        "@R14\n"
        "M=D\n"
        + popD +
        "@ARG\n"
        "A=M\n"
        "M=D\n"
        "@ARG\n"
        "D=M+1\n"
        "@SP\n"
        "M=D\n"
        "@R13\n"
        "M=M-1\n"
        "A=M\n"
        "D=M\n"
        "@THAT\n"
        "M=D\n"
        "@R13\n"
        "M=M-1\n"
        "A=M\n"
        "D=M\n"
        "@THIS\n"
        "M=D\n"
        "@R13\n"
        "M=M-1\n"
        "A=M\n"
        "D=M\n"
        "@ARG\n"
        "M=D\n"
        "@R13\n"
        "M=M-1\n"
        "A=M\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n"
        "@R14\n"
        "A=M\n"
        "0;JMP" << endl;
}

void CodeWriter::writeFunction(string functionName, int numLocals)
{
    asmfile <<
        "(" + functionName + ")" << endl;
    for (int i = 0; i < numLocals; i++)
    {
        asmfile <<
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "M=M+1" << endl;
    }
}

void CodeWriter::makeEndLoop()
{
    asmfile <<
        "(ENDINFILOOP)\n"
        "@ENDINFILOOP\n"
        "0;JMP" << endl;
}

void CodeWriter::writeLoadMemory(string segment, int index)
{
    if (segment == "pointer")
    {
        asmfile <<
            "@" + to_string(3 + index) + "\n"
            "D=M" << endl;
    }
    else if (segment == "temp")
    {
        asmfile <<
            "@" + to_string(5 + index) + "\n"
            "D=M" << endl;
    }
    else if (segment == "constant")
    {
        asmfile <<
            "@" + to_string(index) + "\n"
            "D=A" << endl;
    }
    else if (segment == "static")
    {
        asmfile <<
            "@" + filename + "." + to_string(index) + "\n"
            "D=M" << endl;
    }
    else
    {
        asmfile <<
            "@" + labelTable.at(segment) + "\n"
            "D=M\n"
            "@" + to_string(index) + "\n"
            "A=D+A\n"
            "D=M" << endl;
    }
}