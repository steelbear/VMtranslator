#pragma once
class CodeWriter
{
public:
	CodeWriter(string filepath);
	~CodeWriter();
	void setFileName(string filename);
	void writerInit();
	void makeSysInit();
	void writeArithmetic(string command);
	void writePushPop(COMMAND_TYPE command, string segment, int index);
	void writeLabel(string label);
	void writeGoto(string label);
	void writeIf(string label);
	void writeCall(string functionName, int numArgs);
	void writeReturn();
	void writeFunction(string functionName, int numLocals);
	void makeEndLoop();

private:
	ofstream asmfile;
	string filename;
	static map<string, string> const labelTable;
	int index;

	static const string pushD;
	static const string popD;
	
	void writeLoadMemory(string segment, int index);
};

