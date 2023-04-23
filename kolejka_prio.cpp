#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <format>
#include "windows.h"
#include "psapi.h"

using namespace std;

struct task {
    int r{};
    int p{};
    int q{};
};

class comparator_r {
public:
    bool operator ()(task const& t1, task const& t2) const {
        if (t1.r > t2.r) return true;
        if (t1.r < t2.r) return false;
        if (t1.q > t2.q) return false;
        if (t1.q < t2.q) return true;
        return false;
    }
};

class comparator_q {
public:
    bool operator ()(task const& t1, task const& t2) const {
        if (t1.q > t2.q) return false;
        if (t1.q < t2.q) return true;
        return false;
    }
};

int main() {
    clock_t start;
    clock_t end;

    start = clock();

    priority_queue <task, vector<task>, comparator_r> kp;
    priority_queue <task, vector<task>, comparator_q> kg;

    task e;
    int n = 0;

    string sciezka = "dane/dane100000.txt";
   

    if (ifstream data(sciezka); data.is_open()) {
        data >> n;
        for (int i = 0; i < n; i++) {
            data >> e.r;
            data >> e.p;
            data >> e.q;
            kp.push(e);
        } data.close();
    }
    else {
        cerr << "Nie udało się otworzyć pliku." << endl;
        return 1;
    }

    int t = 0;
    int c_max = 0;

    while (!kp.empty() || !kg.empty()) {
        while ((!kp.empty()) && (kp.top().r <= t)) {
            e = kp.top();
            kp.pop();
            kg.push(e);
        }
        if (kg.empty()) t = kp.top().r;
        else {
            e = kg.top();
            kg.pop();
            t = t + e.p;
            c_max = max(c_max, t + e.q);
        }
    }
    end = clock();

    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

    //cout << format("virtualMemUsedByMe: {}KB\n", virtualMemUsedByMe / 1024);
    //cout << format("physMemUsedByMe: {}KB\n", physMemUsedByMe / 1024);

    cout << n << " " << time_taken << " " << virtualMemUsedByMe / 1024 << " " << physMemUsedByMe / 1024;

    return 0;
}
