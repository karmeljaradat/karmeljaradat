#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <climits>
using namespace std;

struct Process {
    string id;
    int arrival_time;
    int cpu_burst;
    int remaining_time;
    int finish_time = 0;
    int waiting_time = 0;
    int turnaround_time = 0;
};

// Function to calculate metrics
void calculateMetrics(vector<Process>& processes) {
    for (auto& process : processes) {
        process.turnaround_time = process.finish_time - process.arrival_time;
        process.waiting_time = process.turnaround_time - process.cpu_burst;
    }
}

// Function to display Gantt Chart
void displayGanttChart(const vector<pair<string, int>>& gantt) {
    cout << "\nGantt Chart:\n|";
    for (const auto& entry : gantt) {
        cout << " " << entry.first << " |";
    }
    cout << "\n";
    int time = 0;
    cout << "0" << " " << " ";
    for (const auto& entry : gantt) {
        time += entry.second;
        cout << " " << " " << setw(2) << time << " ";
    }
    cout << "\n";
}

// Function to display results
void displayResults(const vector<Process>& processes, double cpu_utilization) {
    cout << left << setw(10) << "Process"
        << setw(10) << "Arrival"
        << setw(10) << "Burst"
        << setw(10) << "Finish"
        << setw(15) << "Turnaround"
        << setw(10) << "Waiting" << "\n";

    for (const auto& process : processes) {
        cout << left << setw(10) << process.id
            << setw(10) << process.arrival_time
            << setw(10) << process.cpu_burst
            << setw(10) << process.finish_time
            << setw(15) << process.turnaround_time
            << setw(10) << process.waiting_time << "\n";
    }

    cout << "CPU Utilization: " << fixed << setprecision(2) << cpu_utilization << "%\n";
}

// First-Come-First-Served (FCFS)
void fcfs(vector<Process> processes) {
    cout << "\n--- FCFS Scheduling ---\n";
    vector<pair<string, int>> gantt;
    int time = 0, idle_time = 0;

    for (auto& process : processes) {
        if (time < process.arrival_time) {
            idle_time += (process.arrival_time - time);
            time = process.arrival_time;
        }
        gantt.push_back({ process.id, process.cpu_burst });
        time += process.cpu_burst;
        process.finish_time = time;
    }

    calculateMetrics(processes);
    double cpu_utilization = ((time - idle_time) / (double)time) * 100;
    displayGanttChart(gantt);
    displayResults(processes, cpu_utilization);
}

// Shortest Remaining Time (SRT)
void srt(vector<Process> processes) {
    cout << "\n--- SRT Scheduling ---\n";
    vector<pair<string, int>> gantt;
    int time = 0, completed = 0, n = processes.size(), idle_time = 0;

    while (completed < n) {
        int min_index = -1, min_remaining = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 &&
                processes[i].remaining_time < min_remaining) {
                min_remaining = processes[i].remaining_time;
                min_index = i;
            }
        }

        if (min_index == -1) {
            idle_time++;
            time++;
            continue;
        }

        if (!gantt.empty() && gantt.back().first == processes[min_index].id) {
            gantt.back().second++;
        }
        else {
            gantt.push_back({ processes[min_index].id, 1 });
        }

        processes[min_index].remaining_time--;
        if (processes[min_index].remaining_time == 0) {
            processes[min_index].finish_time = time + 1;
            completed++;
        }
        time++;
    }

    calculateMetrics(processes);
    double cpu_utilization = ((time - idle_time) / (double)time) * 100;
    displayGanttChart(gantt);
    displayResults(processes, cpu_utilization);
}

// Round-Robin (RR)
void rr(vector<Process> processes, int quantum) {
    cout << "\n--- RR Scheduling ---\n";
    vector<pair<string, int>> gantt;
    int time = 0, completed = 0, idle_time = 0;
    queue<int> q;
    vector<bool> in_queue(processes.size(), false);

    for (int i = 0; i < processes.size(); i++) {
        processes[i].remaining_time = processes[i].cpu_burst;
    }

    q.push(0);
    in_queue[0] = true;

    while (!q.empty() || completed < processes.size()) {
        if (q.empty()) {
            idle_time++;
            time++;
            continue;
        }

        int index = q.front();
        q.pop();
        in_queue[index] = false;

        int exec_time = min(quantum, processes[index].remaining_time);
        if (!gantt.empty() && gantt.back().first == processes[index].id) {
            gantt.back().second += exec_time;
        }
        else {
            gantt.push_back({ processes[index].id, exec_time });
        }

        time += exec_time;
        processes[index].remaining_time -= exec_time;

        if (processes[index].remaining_time == 0) {
            processes[index].finish_time = time;
            completed++;
        }
        else {
            q.push(index);
            in_queue[index] = true;
        }

        for (int i = 0; i < processes.size(); i++) {
            if (!in_queue[i] && processes[i].arrival_time <= time && processes[i].remaining_time > 0) {
                q.push(i);
                in_queue[i] = true;
            }
        }
    }

    calculateMetrics(processes);
    double cpu_utilization = ((time - idle_time) / (double)time) * 100;
    displayGanttChart(gantt);
    displayResults(processes, cpu_utilization);
}

int main() {
    vector<Process> processes;
    ifstream input("processes.txt");
    int quantum;
    if (input.is_open()) {
        int n;
        input >> n >> quantum;
        for (int i = 0; i < n; i++) {
            Process p;
            input >> p.id >> p.arrival_time >> p.cpu_burst;
            p.remaining_time = p.cpu_burst;
            processes.push_back(p);
        }
        input.close();
    }
    else {
        cout << "Error opening file!\n";
        return 1;
    }

    fcfs(processes);
    srt(processes);
    rr(processes, quantum);

    return 0;
}