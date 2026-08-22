#pragma once
#include <string>
#include <fstream>
using namespace std;

#ifdef _WIN32

#ifdef JOURNALDLL_EXPORTS
#define JOURNALDLL_API __declspec(dllexport) 
#else
#define JOURNALDLL_API __declspec(dllimport) 
#endif
#else
#define JOURNALDLL_API
#endif

enum class JOURNALDLL_API Levels {
	LOW,       
	MEDIUM,    
	HIGH,      
	NONE   
};

class JOURNALDLL_API Message {
private:
	ofstream stream; 
	Levels DefaultLevel; 
public:
	Message(string FileName, Levels level); 
	bool SaveMessage(Levels level, string text);
	void ChangeLevel(Levels NewLevel);
	static string LevelToString(Levels level);
	static Levels StringToLevel(string level);
	~Message(); 
};

