#include "Convertor.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
using namespace std;

void Convertor::SetAngle(double & angle) {
	while (angle >= 360) {
		angle -= 360;
	}
	while (angle < 0) {
		angle += 360;
	}
}

bool Convertor::Open(string adofai_file_location) {
	Json::CharReaderBuilder reader;
	ifstream is(adofai_file_location, ifstream::binary);
	string errorMessage;
	auto bret = Json::parseFromStream(reader, is, &this->root, &errorMessage);

	if (bret == false) {
		cout << "Error to parse JSON file !!!" << " " << errorMessage << endl;
		return true;
	}


	if (this->root["angleData"].size() == 0) {
		string path = this->root["pathData"].asString();

		map<char, int> direction;
		direction.insert(pair<char, int>('R', 1));
		direction.insert(pair<char, int>('p', 2));
		direction.insert(pair<char, int>('J', 3));
		direction.insert(pair<char, int>('E', 4));
		direction.insert(pair<char, int>('T', 5));
		direction.insert(pair<char, int>('o', 6));
		direction.insert(pair<char, int>('U', 7));
		direction.insert(pair<char, int>('q', 8));
		direction.insert(pair<char, int>('G', 9));
		direction.insert(pair<char, int>('Q', 10));
		direction.insert(pair<char, int>('H', 11));
		direction.insert(pair<char, int>('W', 12));
		direction.insert(pair<char, int>('L', 13));
		direction.insert(pair<char, int>('x', 14));
		direction.insert(pair<char, int>('N', 15));
		direction.insert(pair<char, int>('Z', 16));
		direction.insert(pair<char, int>('F', 17));
		direction.insert(pair<char, int>('V', 18));
		direction.insert(pair<char, int>('D', 19));
		direction.insert(pair<char, int>('Y', 20));
		direction.insert(pair<char, int>('B', 21));
		direction.insert(pair<char, int>('C', 22));
		direction.insert(pair<char, int>('M', 23));
		direction.insert(pair<char, int>('A', 24));
		direction.insert(pair<char, int>('!', -1));

		for (int i = 0; i < path.size(); i++) {
			this->root["angleData"][i] = (direction[path[i]] - 1) * 15;
			if (direction[path[i]] == -1) {
				this->root["angleData"][i] = 999;
			}
		}
	}

	this->root.removeMember("pathData");

	Json::Value angleData = this->root["angleData"];

	this->angle_data.clear();
	for (int i = 0; i < angleData.size(); i++) {
		this->angle_data.push_back(angleData[i].asDouble());
	}

	this->twirl.resize(this->angle_data.size());
	for (int i = 0; i < this->angle_data.size(); i++) {
		this->twirl[i] = false;
	}
	for (int i = 0; i < this->root["actions"].size(); i++) {
		if (this->root["actions"][i]["eventType"].asString() == "Twirl") {
			this->twirl[this->root["actions"][i]["floor"].asInt()] = true;
		}
	}
	return false;
}

void gotoxy(){
	CONSOLE_SCREEN_BUFFER_INFO presentCur;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &presentCur);

	COORD Cur;
	Cur.X = 0;
	Cur.Y = presentCur.dwCursorPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

void Convertor::ChangeFile() {
	double angle = 0;
	double twirl = 1;

	cout << "Setting Tiles . . ." << endl;

	vector<double> new_angle;
	vector<int> tile_number;
	new_angle.push_back(0);
	new_angle.push_back(999);
	tile_number.push_back(0);
	for (int i = 1; i < this->angle_data.size(); i++) {
		if (this->angle_data[i] != 999) {
			if (this->twirl[i]) {
				twirl *= -1.0;
			}
			if (this->angle_data[i - 1] == 999) {
				angle -= (this->angle_data[i - 2] - this->angle_data[i]) * twirl;
			}
			else {
				angle -= (this->angle_data[i - 1] - this->angle_data[i]) * twirl;
				angle += 180;
			}
			this->SetAngle(angle);
			tile_number.push_back(new_angle.size());
			new_angle.push_back(angle);
			new_angle.push_back(999);
		}
		else {
			tile_number.push_back(tile_number.back());
		}
	}
	tile_number.push_back(tile_number.back() + 1);

	this->root["angleData"].clear();
	for (int i = 0; i < new_angle.size(); i++) {
		this->root["angleData"][i] = new_angle[i];
	}

	cout << "Setting Actions . . ." << endl;

	for (int i = 0; i < this->root["actions"].size(); i++) {
		if (this->root["actions"][i]["eventType"].asString() == "Twirl" ||
			this->root["actions"][i]["eventType"].asString() == "MoveTrack" ||
			this->root["actions"][i]["eventType"].asString() == "AnimateTrack") {
			this->root["actions"].removeIndex(i, nullptr);
			i--;
			continue;
		}

		if (this->root["actions"][i]["eventType"].asString() == "MoveCamera") {
			this->root["actions"][i]["position"][0] = 0;
			this->root["actions"][i]["position"][1] = 0;
			this->root["actions"][i]["relativeTo"] = "Player";
		}

		int floor = this->root["actions"][i]["floor"].asInt();
		this->root["actions"][i]["floor"] = tile_number[floor] - 1;
		gotoxy();
		cout << ceil((double)i / (double)this->root["actions"].size() * 100.0) << "%";
	}

	this->root["settings"]["relativeTo"] = "Player";
	this->root["settings"]["position"][0] = 0;
	this->root["settings"]["position"][1] = 0;

	this->root["settings"]["trackAnimation"] = "Fade";
	this->root["settings"]["beatsAhead"] = 1;
	this->root["settings"]["trackDisappearAnimation"] = "Fade";
	this->root["settings"]["beatsBehind"] = 0;
}

void Convertor::Save(string adofai_file_location) {
	FILE *f;
	fopen_s(&f, adofai_file_location.c_str(), "wb");
	fprintf_s(f, "%s", this->root.toStyledString().c_str());
	fclose(f);
}
