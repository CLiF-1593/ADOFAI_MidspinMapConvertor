#include "Editor.h"
#include <iostream>
#include <fstream>
using namespace std;

void Editor::ClearEffects() {
	for (int i = 0; i < this->root["actions"].size(); i++) {
		int f = this->root["actions"][i]["floor"].asInt();
		string et = this->root["actions"][i]["eventType"].asString();
		if (f >= this->range.begin && f <= this->range.end) {
			if (et == "Twirl" || et == "SetSpeed") {
				this->root["actions"].removeIndex(i, NULL);
				i--;
			}
		}
	}
}

void Editor::SetAngle(double * angle) {
	while (*angle >= 360) {
		*angle -= 360;
	}
	while (*angle < 0) {
		*angle += 360;
	}
}

void Editor::Open(string adofai_file_location) {
	Json::CharReaderBuilder reader;
	ifstream is(adofai_file_location, ifstream::binary);
	string errorMessage;
	auto bret = Json::parseFromStream(reader, is, &this->root, &errorMessage);

	if (bret == false) {
		cout << "Error to parse JSON file !!!" << " " << errorMessage << endl;
		return;
	}


	if (this->root["angleData"].size()== 0) {
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
}

void Editor::SetRange(int start_tile, int end_tile) {
	this->range.begin = start_tile;
	this->range.end = end_tile;
}

void Editor::SetBPM(double bpm) {
	this->bpm = bpm;
}

Json::Value Twirl(int floor) {
	Json::Value twirl;
	twirl["floor"] = floor;
	twirl["eventType"] = "Twirl";
	return twirl;
}

Json::Value BPM(int floor, double bpm) {
	Json::Value bpm_change;
	bpm_change["floor"] = floor;
	bpm_change["eventType"] = "SetSpeed";
	bpm_change["speedType"] = "Bpm";
	bpm_change["beatsPerMinute"] = bpm;
	bpm_change["bpmMultiplier"] = 1;
	return bpm_change;
}


void Editor::ChangeFile() {
	this->ClearEffects();

	auto *actions = &this->root["actions"];
	
	bool twirl = false;

	int prev_angle = -1;

	for (int i = this->range.begin; i <= this->range.end; i++) {
		bool midspin = false;
		if (this->angle_data[i] == 999) {
			i++;
			midspin = true;
		}
		double angle = 180 - (this->angle_data[i] - this->angle_data[i - 1]);
		if (midspin) {
			angle = this->angle_data[i - 2] - this->angle_data[i];
		}
		this->SetAngle(&angle);

		if (angle > 180 && !twirl) {
			twirl = true;
			actions->append(Twirl(i));
		}
		else if (angle < 180 && twirl) {
			twirl = false;
			actions->append(Twirl(i));
		}

		if (angle > 180) {
			angle = 360 - angle;
		}

		if (angle == 0)
			angle = 360;

		if (prev_angle != angle) {
			actions->append(BPM(i, this->bpm * angle / 180.0));
			prev_angle = angle;
		}
	}
}

void Editor::Save(string adofai_file_location) {
	FILE *f;
	fopen_s(&f, adofai_file_location.c_str(), "wb");
	fprintf_s(f, "%s", this->root.toStyledString().c_str());
	fclose(f);
}
