#include "stdafx.h"

#include "Parser.h"
#include "CodeWriter.h"

void listVMFiles(string path, vector<string> &files);
string dirpath(string path);
string setAssemblyFilename(string path);

int main(int argc, char** argv)
{
	string path(argv[1]);
	vector<string> filepaths;
	int filepathIndex = 0;

	CodeWriter codeWriter(setAssemblyFilename(path));
	COMMAND_TYPE cmdType;

	int lineno;

	listVMFiles(path, filepaths);

	try {
		codeWriter.writerInit();

		if (find(filepaths.begin(), filepaths.end(), "Sys") == filepaths.end())
		{
			codeWriter.makeSysInit();
		}

		for (auto filepath : filepaths) {
			Parser parser(filepath);
			codeWriter.setFileName(filepath);

			cout << "Parsing " + filepath + "..." << endl;

			lineno = 0;
			parser.advance();
			while (parser.hasMoreCommands())
			{
				cmdType = parser.commandType();

				switch (cmdType)
				{
				case COMMAND_TYPE::C_PUSH:
				case COMMAND_TYPE::C_POP:
					codeWriter.writePushPop(cmdType, parser.arg1(), parser.arg2());
					break;
				case COMMAND_TYPE::C_ARITHMETIC:
					codeWriter.writeArithmetic(parser.arg1());
					break;
				case COMMAND_TYPE::C_LABEL:
					codeWriter.writeLabel(parser.arg1());
					break;
				case COMMAND_TYPE::C_GOTO:
					codeWriter.writeGoto(parser.arg1());
					break;
				case COMMAND_TYPE::C_IF:
					codeWriter.writeIf(parser.arg1());
					break;
				case COMMAND_TYPE::C_CALL:
					codeWriter.writeCall(parser.arg1(), parser.arg2());
					break;
				case COMMAND_TYPE::C_RETURN:
					codeWriter.writeReturn();
					break;
				case COMMAND_TYPE::C_FUNCTION:
					codeWriter.writeFunction(parser.arg1(), parser.arg2());
					break;
				default:
					parser.printLine();
				}
				lineno++;
				parser.advance();
			}

			filepathIndex++;
		}

		if (find(filepaths.begin(), filepaths.end(), "Sys") == filepaths.end())
		{
			codeWriter.makeEndLoop();
		}
	}
	catch (string errmsg)
	{
		cerr << "In " + filepaths[filepathIndex] + " line " + to_string(lineno) + "\n";
		cerr << errmsg << endl;
	}

	cout << "Done!" << endl;

	return 0;
}

void listVMFiles(string path, vector<string> &files)
{
	if (path.find(".vm") != string::npos) {
		files.push_back(path);
	}
	else
	{
		for (const auto& filepath : fs::directory_iterator(path))
		{
			string filename = filepath.path().string();

			if (filename.find(".vm") != string::npos)
			{
				files.push_back(filename);
			}
		}
	}
}

string dirpath(string path)
{
	if (path.find(".vm") != string::npos)
	{
		path.erase(find_if(path.rbegin(), path.rend(), [](char c) {
			return c == '\\' || c == '/';
			}).base() - 1, path.end());
	}

	return path;
}

string setAssemblyFilename(string arg)
{
	if (arg.find(".vm") == string::npos)
	{
		string path(dirpath(arg));
		string filename(path);
		filename.erase(filename.begin(), find_if(filename.rbegin(), filename.rend(), [](char c) {
			return c == '\\' || c == '/';
			}).base());
		filename += ".asm";

		return path + "/" + filename;
	}
	else
	{
		return arg.substr(0, arg.length() - 3) + ".asm";
	}
}