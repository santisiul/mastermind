#ifndef FILE_H
#define FILE_H 


#include <fstream>
#include <vector>
#include <sstream>


using namespace std;

class File
{

private:
	char* nameFile;
	std::vector<string> gamers;

public:
	File();
	~File();

	bool readFile();
	bool writeFile(string cadena);
	void addGamer(string cadena);
	void save();

	int getGamers();
	string getGamer(int pos);

};

#endif

