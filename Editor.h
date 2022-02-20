#pragma once
#include <string>
#include "json/json.h"
using namespace std;
enum Mode { INSIDE, OUTSIDE }; //내각 or 외각
struct Range {
	int begin;
	int end;
};

class Editor {
private:
	vector<double> angle_data;
	Range range;
	double bpm;

	Json::Value root;

	void ClearEffects();

	void SetAngle(double *angle);

public:
	void Open(string adofai_file_location); //파일 열기
 	void SetRange(int start_tile, int end_tile); //변경 범위 설정
	void SetBPM(double bpm); //BPM 설정
	void ChangeFile(); //변형
	void Save(string adofai_file_location); //저장
};

