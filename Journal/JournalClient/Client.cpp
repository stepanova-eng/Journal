#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "JournalDLL.h"
using namespace std;

bool stopThread = false;
mutex mtx;
condition_variable cv;

struct Task {
	Levels level;
	string text;
	Task() : level(Levels::NONE), text("") {}
};

queue<Task> taskQueue;

// Функция для записи сообщения в файл, передающаяся в отдельный поток
// При открытии программы один раз создается отдельный фоновый поток для записи сообщений в файл
// Поток активируется в случае подачи сигнала из основной программы 
void SavingMessage(Message& obj) {

	while (true) {

		unique_lock<mutex> lck(mtx);
		cv.wait(lck, [] { return !taskQueue.empty() || stopThread; });

		// Завершение работы функции в случае отсутствия сообщений или при сигнале завершения 
		if (stopThread && taskQueue.empty()) {
			break;
		}

		Task currentTask = taskQueue.front();
		taskQueue.pop();
		lck.unlock();

		if (!obj.SaveMessage(currentTask.level, currentTask.text)) {
			cout << "Message was not saved" << endl;
		}
	}
}

// Приведение строки к нижнему регистру
string Lower(string text) {
	string newText = "";
	for (char cymb : text) {
		cymb = tolower(cymb);
		newText += cymb;
	}
	return newText;
}

int main() {
	string fileName, option, strDefaultLevel;
	Levels defaultLevel, level;

	try { // необходим в случае неудачи при выделении динамической памяти (возможно введено слишком длинное сообщение)
		cout << "Welcome to Journal!\nEnter your file name:" << endl;

		while (true) {
                      getline(cin, fileName);
                      if (fileName.empty()) {
                              cout << "Enter your file name: " << endl;
                      }
                      else { break; }
                }
		cout << "Enter default importance level: " << endl;

		while (true) {
			getline(cin, strDefaultLevel);
			strDefaultLevel = Lower(strDefaultLevel);

			// проверка правильности ввода уровня по умолчанию 
			try {
				defaultLevel = Message::StringToLevel(strDefaultLevel);
				break;
			}
			catch (const invalid_argument& e) {
				cout << "Invalid security level. Try again:" << endl;
			}
		}

		Message message(fileName, defaultLevel);
		thread savingMessageThread(SavingMessage, ref(message));

		do {
			cout << "Choose an option:\n1. Change default security level\n2. Add message\n3. Leave the program\n" << endl;

			getline(cin, option);

			if (option == "1") {
				string newLevel;
				cout << "Enter new security level:" << endl;

				while (true) {

					getline(cin, newLevel);
					newLevel = Lower(newLevel);

					try {
						level = Message::StringToLevel(newLevel);
						break;
					}
					catch (const invalid_argument& e) {
						cout << "Invalid security level. Try again:" << endl;
					}
				}

				message.ChangeLevel(level);
			}
			else if (option == "2") {
				string messageText;
				string messageSecurityLevel;
				Levels messageLevel;

				cout << "Enter security level of your message: " << endl;
				while (true) {

					getline(cin, messageSecurityLevel);
					if (messageSecurityLevel.empty()) { // обработка отсутствия уровня важности
						messageSecurityLevel = "none";
					}
					else {messageSecurityLevel = Lower(messageSecurityLevel);}

					try {
						messageLevel = Message::StringToLevel(messageSecurityLevel);
						break;
					}
					catch (const invalid_argument& e) {
						cout << "Invalid security level. Try again:" << endl;
					}
				}

				cout << "Enter your message: " << endl;
				getline(cin, messageText);

				Task newTask;
				newTask.level = messageLevel;
				newTask.text = messageText;

				unique_lock<mutex> lck(mtx);
				taskQueue.push(newTask);

				cv.notify_one(); // сигнал к пробуждению фонового потока
			}
			else if (option == "3") {

				unique_lock<mutex> lck(mtx);
				stopThread = true; 
				cout << "The program has been successfully completed" << endl;
			}
			else { cout << "Input error. Try again" << endl; }

		} while (option != "3");

		cv.notify_all();
		savingMessageThread.join();
		return 0;
	}
	catch(const bad_alloc &e) {
		cout << "Memory error" << endl;
	}
}
