// OnTimeMeasure.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Task.cpp"
#include <list>
#include <cctype>
#include <vector>
#include<time.h>
#include <fstream>
#include <sstream>
#include <map>  
#include <utility>  
#include<algorithm>
using namespace std;

struct  exec_time_sort
{
    bool operator()(Task const task1, Task const task2) {
        
        return task1.exe_time > task2.exe_time;
    }
};
string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

struct Config {
    int mla;
    int binSize;
    int noTasks;
    int conflictRatio;
};

void loadConfig(Config& config) {
    ifstream fin("config.txt");
    string line;
    while (getline(fin, line)) {
        istringstream sin(line.substr(line.find("=") + 1));
        if (line.find("mla") != -1)
            sin >> config.mla;
        else if (line.find("binSize") != -1)
            sin >> config.binSize;
        else if (line.find("noTasks") != -1)
            sin >> config.noTasks;
        else if (line.find("conflictRatio") != -1)
            sin >> config.conflictRatio;
    }
}

int round_robin(list<Task> tasks, int bin_size) {
    list<Task> ::iterator it;
    int start_time = 0;
    int end_time = 0;
    list<Task> temp_tasks = tasks;
    for (it = temp_tasks.begin(); it != temp_tasks.end(); ++it) {
        int execution_time = (it->exe_time);

        if (execution_time <= bin_size) {           
            end_time = start_time + execution_time;
            cout << "Start time : " << start_time << " End time : " << end_time << " for task " << it->task_no<<"\n";
            start_time = start_time + execution_time;
        }
        else {
            end_time = start_time + bin_size;
            execution_time = execution_time - bin_size;
            Task task(it->task_no,it->source,it->destination,it->tool,(execution_time));
            cout << "Start time : " << start_time << " End time : " << end_time << " for task " << it->task_no<<"\n";
            temp_tasks.push_back(task);

            start_time = start_time + bin_size;
        }
        
    }


    return end_time;
}

bool checkKeyConflict(int task_no, map<int, list<int>> conflict_map) {
    if (conflict_map.find(task_no) == conflict_map.end()) {
        return false;
     }
    return true;
}

bool check_key_value_conflict(int key,int value, map<int, list<int>> conflict_map) {
    
    list<int> temp = conflict_map.at(key);
    if (checkKeyConflict(value, conflict_map)) {
        list<int> temp1 = conflict_map.at(value);
        if (find(temp.begin(), temp.end(), value) != temp.end() || find(temp1.begin(), temp1.end(), key) != temp1.end()) {
            return false;
        }
        return true;
    }
    else {
        if (find(temp.begin(), temp.end(), value) != temp.end()) {
            return false;
        }
        return true;

    }
    
    
    return true;
}

int heuristic_algo(list<Task> tasks, int MLA, map<int,list<int>> conflict_map,int bin_size) {
    int max_time = 0;
    list<Task> ::iterator it;
    list<Task> ::iterator it1;
    list<Task> temp_tasks = tasks;
    for (it = temp_tasks.begin(); it != temp_tasks.end(); ++it) {
        if (it->done_execution != true) {
            int mla_cnt = 1;
            if (it->exe_time <= bin_size) {
                it->done_execution = true;
                max_time = max_time + it->exe_time;
            }
            else {
                int execution_time = it->exe_time - bin_size;
                Task task(it->task_no, it->source, it->destination, it->tool, (execution_time));
                temp_tasks.push_back(task);
                max_time = max_time + bin_size;
            }
            for (it1 = next(it, 1); it1 != temp_tasks.end(); ++it1) {
                if(mla_cnt != MLA){
                    if (it1->done_execution != true) {
                        if (checkKeyConflict(it->task_no, conflict_map)) {
                            if (check_key_value_conflict(it->task_no, it1->task_no, conflict_map)) {
                                if (it1->exe_time <= bin_size) {
                                    it1->done_execution = true;
                                }
                                else {
                                    int execution_time = it1->exe_time - bin_size;
                                    Task task(it1->task_no, it1->source, it1->destination, it1->tool, (execution_time));
                                    temp_tasks.push_back(task);
                                }
                                mla_cnt = mla_cnt + 1;
                            }
                        }
                        else {
                            if (it1->exe_time <= bin_size) {
                                it1->done_execution = true;
                            }
                            else {
                                int execution_time = it1->exe_time - bin_size;
                                Task task(it1->task_no, it1->source, it1->destination, it1->tool, (execution_time));
                                temp_tasks.push_back(task);
                            }
                            mla_cnt = mla_cnt + 1;
                        }
                    }
                }
            }
               /* for (int j = i + 1; j < tasks1.size(); j++) {
                    if (mla_cnt != MLA) {
                        bool src_found = (find(sources.begin(), sources.end(), tasks1[j].source) != sources.end());
                        bool src_found1 = (find(destinations.begin(), destinations.end(), tasks1[j].source) != destinations.end());
                        
                        bool dst_found1 = (find(sources.begin(), sources.end(), tasks1[j].destination) != sources.end());
                        bool dst_found = (find(destinations.begin(), destinations.end(), tasks1[j].destination) != destinations.end());

                        //bool tool_found = (find(tools.begin(), tools.end(), tasks1[j].tool) != tools.end());

                        if (!src_found && !dst_found &&  !src_found1 && !dst_found1) {
                            sources.push_back(tasks1[j].source);
                            destinations.push_back(tasks1[j].destination);
                            //tools.push_back(tasks1[j].tool);
                            tasks1[j].done_execution = true;
                            mla_cnt += 1;
                        }
                    }
                }
            
            tasks1[i].done_execution = true;
            max_time = max_time + (tasks1[i].exe_time);*/
        }
        
    }
    return max_time;
}


map<int,list<int>> conflicts(list<Task> tasks, int conflict_ratio, int no_of_tasks) {
    map<int, list<int>> conflict_map;
    list<Task> ::iterator it;
    int total_edges = (no_of_tasks * (no_of_tasks - 1)) / 2;
    int conflicts = round(conflict_ratio * total_edges);
    int key;
    for (int i = 1; i <= conflicts; i++) {
        if (i < no_of_tasks) {
            conflict_map.insert({ i, { i + 1 } });
        }
        else if (i == no_of_tasks) {
            conflict_map.insert({ i, { 1 } });
        }
        else {
            key = i % no_of_tasks;
            if (key != 0) {
                list<int> temp = conflict_map.at(key);
                int last_element = temp.back();
                if (last_element < no_of_tasks) {
                    conflict_map.at(key).push_back(last_element + 1);
                }
                else {
                    if (last_element == no_of_tasks) {
                        conflict_map.at(key).push_back(1);
                    }
                }
            }
            else {
                list<int> temp = conflict_map.at(no_of_tasks);
                int last_element = temp.back();
                if (last_element < no_of_tasks) {
                    conflict_map.at(no_of_tasks).push_back(last_element + 1);
                }
                else {
                    if (last_element == no_of_tasks) {
                        conflict_map.at(no_of_tasks).push_back(1);
                    }
                }
            }
        }
    }
    return conflict_map;
}



int main()
{    
    Config config;
    loadConfig(config);

    int MLA = config.mla;
    int bin_size = config.binSize;
    int total_no_of_tasks = config.noTasks;
    int conflict_ratio = config.conflictRatio;

    srand(time(0));

    list<Task> tasks ;
    list <Task> :: iterator it;
    map<int, list<int>> :: iterator it1;
    list<int> :: iterator it2;
    for (int i=1;i<=total_no_of_tasks;i++)
    {
        
        string src = gen_random(4);
        string dst = gen_random(5);
        string tool = gen_random(6);
        int rnd = bin_size + 20;
        int exe_time = 1 + (rand() % rnd);
        Task obj(i,src, dst, tool, exe_time);
        tasks.push_back(obj);       
        cout << '\n';       
    }
    
    map<int,list<int>> conflict_map = conflicts(tasks, conflict_ratio,total_no_of_tasks);
    /*for (it1 = conflict_map.begin(); it1 != conflict_map.end(); ++it1) {
        cout << it1->first << "elements " << endl;;
        for (it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            cout << *it2 << " ";
        }
        cout << endl;
    }*/
    
    tasks.sort(exec_time_sort());
    /*for (it = tasks.begin(); it != tasks.end(); ++it) {
        cout << it->exe_time << " "<<it->task_no<<endl;
    }*/
    
    if (MLA == 1) {
        int total_time = round_robin(tasks,bin_size);
        cout << "Total time taken to execute the tasks is " << total_time;

    }
    else {
        int total_time = heuristic_algo(tasks, MLA,conflict_map,bin_size);
        cout << "Total time taken to execute the tasks is " << total_time;
    }
    
    
    
    
}

