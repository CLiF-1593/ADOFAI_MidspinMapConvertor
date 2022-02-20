#include <iostream>
#include "Convertor.h"
using namespace std;
int main() {
	Convertor convertor;
	cout << "[ADOFAI MidSpin Map Convertor]" << endl << endl;
	cout << "맵의 모든 타일을 미드스핀으로 변경합니다" << endl;
	cout << "Developer : CLiF" << endl;
	cout << "Version : 1.0" << endl << endl;
	cout << "ADOFAI File Path : ";
	string path;
	cin >> path;
	if (convertor.Open(path)) {
		return 1;
	}
	convertor.ChangeFile();
	convertor.Save(path);
	cout << endl;
	cout << "Completed!!";
	system("pause");
	return 0;
}