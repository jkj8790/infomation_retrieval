#include "util.h"

#include <list>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <stack>

using namespace std;
using namespace std::chrono;

stack<high_resolution_clock::time_point> startTimeStack;

namespace util {

	void startTimer() {
		startTimeStack.push(high_resolution_clock::now());
	}

	void endTimerAndPrint(string with) {
		auto duration = duration_cast<milliseconds>( high_resolution_clock::now() - startTimeStack.top() ).count();
		cout << with << endl;
		cout << durationToString(duration) << endl;
		startTimeStack.pop();
	}

	string durationToString(long duration) {
		string result = "";
		int deciSecond = (duration / 100) % 10;
		int second = (duration / 1000) % 60;
		int minute = (duration / (1000 * 60)) % 60;
		int hour = (duration / (1000 * 60 * 60)) % 24;

		if(hour > 0)
			result += to_string(hour) + "시간 ";
		if(minute > 0)
			result += to_string(minute) + "분 ";
		result += to_string(second) + "." + to_string(deciSecond) + "초 걸렸습니다.";

		return result;
	}

	void logList(list<string> strings) {
		list<string>::iterator iter = strings.begin();
		while( iter != strings.end()) {
			cout << *iter << endl;
			iter++;
		}
	}
}