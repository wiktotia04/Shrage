#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "windows.h"
#include "psapi.h"
#include <format>
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

class heap {
public:
	heap() = default;

	void push_back(const task& job) {
		kopiec.push_back(job);
	}

	bool empty() const {
		return kopiec.empty();
	}

	auto begin() {
		return kopiec.begin();
	}

	auto end() {
		return kopiec.end();
	}

	auto front() const {
		return kopiec[0];
	}

	void pop_back()
	{
		kopiec.pop_back();
	}

	template<random_access_iterator RandomAccessIterator, typename Compare>
	void shift_down(RandomAccessIterator first, RandomAccessIterator last, size_t parent_index, Compare comp) const {
		auto max_child_index = 2 * parent_index + 1;
		auto parent_value = std::move(first[parent_index]);
		while (max_child_index < static_cast<size_t>(last - first)) {
			if (max_child_index + 1 < static_cast<size_t>(last - first) && comp(first[max_child_index], first[max_child_index + 1]))
				++max_child_index;
			if (!comp(parent_value, first[max_child_index])) 
				break;
			first[parent_index] = std::move(first[max_child_index]);
			parent_index = max_child_index;
			max_child_index = 2 * parent_index + 1;
		}
		first[parent_index] = std::move(parent_value);
	}

	template<random_access_iterator RandomAccessIterator, typename Compare>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		const auto n = static_cast<size_t>(last - first);
		for (auto parent_index = n / 2; parent_index > 0; --parent_index) {
			shift_down(first, last, parent_index - 1, comp);
		}
	}

	template<forward_iterator RandomAccessIterator, typename Compare>
	void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) const {
		if (first == last) return;

		iter_swap(first, --last);
		ptrdiff_t size = distance(first, last);

		ptrdiff_t parent_idx = 0;
		ptrdiff_t child_idx = 1;

		while (child_idx < size) {
			if (child_idx + 1 < size && comp(*(first + child_idx), *(first + child_idx + 1))) 
				child_idx++;
			if (comp(*(first + parent_idx), *(first + child_idx))) {
				iter_swap(first + parent_idx, first + child_idx);
				parent_idx = child_idx;
				child_idx = 2 * parent_idx + 1;
			}
			else break;
		}
	}

	template<forward_iterator RandomAccessIterator, typename Compare>
	void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) const {
		if (first == last) return;

		auto child = last - 1;
		while (child > first) {
			auto parent = (child - first - 1) / 2;
			if (comp(*(first + parent), *child)) {
				swap(*(first + parent), *child);
				child = first + parent;
			}
			else break;
		}
	}

private:
	vector <task> kopiec;
};

int main() {
	clock_t start;
	clock_t end;

	start = clock();
	task e;

	heap kp;
	heap kg;

	
	string sciezka = "dane/dane1260000.txt";
	int n = 0;
	if (ifstream data(sciezka); data.is_open()) {
		
		data >> n;
		for (int i = 0; i < n; i++) {
			data >> e.r;
			data >> e.p;
			data >> e.q;
			kp.push_back(e);
		} data.close();
	}
	else {
		cerr << "Nie udało się otworzyć pliku." << endl;
		return 1;
	}

	kp.make_heap(kp.begin(), kp.end(), comparator_r());
	kg.make_heap(kg.begin(), kg.end(), comparator_q());

	int t = 0;
	int c_max = 0;

	while (!kp.empty() || !kg.empty()) {
		while ((!kp.empty()) && (kp.front().r <= t)) {
			e = kp.front();
			kp.pop_heap(kp.begin(), kp.end(), comparator_r());
			kp.pop_back();
			kg.push_back(e);
			kg.push_heap(kg.begin(), kg.end(), comparator_q());
		}
		if (kg.empty()) t = kp.front().r;
		else {
			e = kg.front();
			kg.pop_heap(kg.begin(), kg.end(), comparator_q());
			kg.pop_back();
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
