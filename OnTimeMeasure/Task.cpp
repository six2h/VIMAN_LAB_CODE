#include <iostream>
#include <string>
using namespace std;

class Task {
	
public:
	int task_no;
	string source;
	string destination;
	string tool;
	int exe_time;
	bool done_execution;
	Task(int task_no,string src, string dst, string tool, int exe_time) {
		this->task_no = task_no;
		this->source = src;
		this->destination = dst;
		this->tool = tool;
		this->exe_time = exe_time;
	}
};