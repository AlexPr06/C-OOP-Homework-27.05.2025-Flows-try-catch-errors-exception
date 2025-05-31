#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

using namespace std;


class FileManagingSystem
{
private:
	string fileName;
public:
	FileManagingSystem(string& fileName) : fileName(fileName) {}

	void writeDataToFile(const string& data) {
		ofstream file(fileName, ios::app);
		if (file.is_open()) {
			file << data << endl;
			file.close();
			cout << GREEN << "Data written to file successfully!" << RESET << endl;
		}
		else {
			cout << RED << "Failed to open file for writing." << RESET << endl;
		}
	}

	void clearFile() {
		ofstream file(fileName, ios::trunc);
		if (!file.is_open()) {
			cout << RED << "Failed to clear file." << RESET << endl;
			return;
		}
		file.close();
		cout << GREEN << "File cleared successfully!" << RESET << endl;
	}

	~FileManagingSystem()
	{
		cout << YELLOW << "FileManagingSystem destroyed." << RESET << endl;
	}
};

class CastSpelling : public FileManagingSystem
{
private:
	mutex mtx;
	condition_variable cv;
	bool mannaAccumulated = false;
	bool spellingRemembered = false;

public:

	CastSpelling(string& fileName) : FileManagingSystem(fileName) {
		cout << GREEN << "Welcome to the Cast Spelling Game!" << RESET << endl;
		cout << YELLOW << "Your goal is to cast the spell 'Avada Kedavra'." << RESET << endl;
		cout << YELLOW << "You need to accumulate manna and remember the spelling first." << RESET << endl;
		cout << endl << endl;
		clearFile();
		writeDataToFile("Game started: Cast Spelling Game initialized.");

	}

	void mannaAccumulation() {
		cout << YELLOW << "Manna accumulation started..." << RESET << endl;
		while (true) {
			unique_lock<mutex> lock(mtx);

			if (rand() % 5 == 0) {
				cout << GREEN << "Manna accumulated!" << RESET << endl;
				mannaAccumulated = true;
				cv.notify_all();
				writeDataToFile("Manna accumulated successfully!");
				break;
			}
			cout << BLUE << "Still accumulating..." << RESET << endl;
			this_thread::sleep_for(chrono::milliseconds(500)); 
		}
	}

	void spellingRemembering()
	{
		cout << YELLOW << "Spelling remembering..." << RESET << endl;
		while (true)
		{
			unique_lock<mutex> lockForSpellingRemembering(mtx);
			bool innerTimerPassed = false;
			cout << CYAN << "Can't remember..." << RESET << endl;
			if (rand() % 20 == 0)
			{
				cout << MAGENTA << "Spelling Remembered!" << RESET << endl;
				spellingRemembered = true;
				cv.notify_all(); 
				writeDataToFile("Spelling Remembered!");
				break;
			}

			lockForSpellingRemembering.unlock();
			this_thread::sleep_for(chrono::milliseconds(500));
		}
	}

	void avadaKedavra()
	{
		unique_lock<mutex> lockForSayAvadaKedavra(mtx);
		cv.wait(lockForSayAvadaKedavra, [this]()
			{
				return mannaAccumulated && spellingRemembered;
			}
		);
		cout << RED << "Avada Kedavra!" << RESET << endl;
		writeDataToFile("Avada Kedavra spell casted!");
		cout << GREEN << "The spell has been cast successfully!" << RESET << endl;
	}

	~CastSpelling() {
		cout << endl << endl;
		cout << YELLOW << "Thank you for playing the Cast Spelling Game!" << RESET << endl;
		cout << YELLOW << "Goodbye!" << RESET << endl;
		writeDataToFile("Game ended: Cast Spelling Game finished.");
	}
};


int main() {
	system("chcp 65001 > nul");
	string fileName = "game_data.txt"; 
	CastSpelling* game = new CastSpelling(fileName);
	thread t1(&CastSpelling::mannaAccumulation, game);
	thread t2(&CastSpelling::spellingRemembering, game);
	thread t3(&CastSpelling::avadaKedavra, game);

	t1.join();
	t2.join();
	t3.join();

	delete game;
	return 0;
}

