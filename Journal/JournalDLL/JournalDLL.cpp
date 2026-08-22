#include "JournalDLL.h"
#include <string>
#include <fstream>
#include <time.h>
#include <iostream>
using namespace std;

Message::Message(string FileName, Levels level) {
	stream.open(FileName, ios::app); 
	DefaultLevel = level; 
}

Message::~Message() {
	if (stream.is_open()) {
		stream.close();
	}
}

string Message::LevelToString(Levels level) {
	if (level == Levels::LOW) { return "LOW"; }
	else if (level == Levels::MEDIUM) { return "MEDIUM"; }
	else if (level == Levels::HIGH) { return "HIGH"; }
	else if (level == Levels::NONE) { return "NONE"; }
	return "Unknown";
}

Levels Message::StringToLevel(string level) {
	if (level == "low") { return Levels::LOW; }
	else if (level == "medium") { return Levels::MEDIUM; }
	else if (level == "high") { return Levels::HIGH; }
	else if (level == "none") { return Levels::NONE; }
    throw invalid_argument("Invalid security level"); // вызов исключени, если пользователем введено неккоректное значение
	}

// Замена текущего уровня важности
void Message::ChangeLevel(Levels NewLevel) {
	DefaultLevel = NewLevel;
}

// Запись сообщения в файл
bool Message::SaveMessage(Levels level, string text) {

    if (level < DefaultLevel) {
        return false;
    }
    else {
        if (!stream.is_open()) {
            return false;
        }
        else { 
			time_t MessageTime = time(NULL);
			struct tm now;
			localtime_r(&MessageTime, &now);

			string Date = "Date: " + to_string(now.tm_mday) + "." + to_string(now.tm_mon + 1) + "." + to_string(now.tm_year + 1900) + "\n";
			string Time = "Time: " + to_string(now.tm_hour) + ":" + to_string(now.tm_min) + ":" + to_string(now.tm_sec);

			stream << "Importance level: " + LevelToString(level) << "\n" << "Message text: " << text << "\n" << Date << Time << "\n" << endl;

            if (stream.fail()) {
                return false;
            }
		}
	}
    return true;
}

