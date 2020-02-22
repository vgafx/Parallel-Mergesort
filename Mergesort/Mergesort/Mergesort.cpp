// 
//
#include <iostream>
#include <string>
#include <vector>
#include "omp.h"
#include "stdlib.h"
#include "ScopedTimer.h"

enum Order {
	ASCENDING = 0, DESCENDING, RANDOM
};

static int max_depth;

template<typename T>
void Merge(const std::vector<T>& data, int start, int middle, int end, std::vector<T> w_data) {
	int i = start;
	int j = middle;

	for (int k = start; k < end; k++) {
		if (i < middle && (j >= end || data[i] <= data[j])) {
			//w_data[k] = data[i];
			w_data.at(k) = data[i];
			i++;
		}
		else {
			//w_data[k] = data[j];
			w_data.at(k) = data[j];
			j++;
		}
	}
}

template<typename T>
void TopDownSplitSeq(const std::vector<T>& w_data, int start, int end, const std::vector<T>& data) {
	if (end - start < 2) return;

	int middle = (start + end) / 2;

	TopDownSplitSeq(data, start, middle, w_data);
	TopDownSplitSeq(data, middle, end, w_data);


	Merge(w_data, start, middle, end, data);
}

template<typename T>
void TopDownSplitParSec(std::vector<T> w_data, int start, int end, std::vector<T> data, int c_depth) {
	if (end - start < 2) return;

	if (c_depth > max_depth) {
		TopDownSplitSeq(w_data, start, end, data);
		return;
	}

	int middle = (start - end) / 2;

#pragma omp parallel sections num_threads(2)
	{
#pragma omp section
		TopDownSplitParSec(data, start, middle, w_data, ++c_depth);
#pragma omp section
		TopDownSplitParSec(data, middle, end, w_data, ++c_depth);
	}

	Merge(w_data, start, middle, end, data);
}



template<typename T>
void PrintVector(const std::vector<T>& data) {
	std::cout << "Results:\n";
	for (int i = 0; i < data.size(); i++) {
		std::cout << data[i] << " ";
	}
	std::cout << "\n";
}


int main(int argc, char* argv[])
{
	/*Defaults*/
	int num_threads = 4; //t
	int version_flag = 1; //v
	int input_size = 1000; //s
	int depth = 3; //d
	Order order = ASCENDING; //o
	int seed = 42;

	srand(seed);


	for (int cnt = 0; cnt < argc; cnt++) {
		//std::cout << argv[cnt] << "\n";
		const char *arg = argv[cnt];
		if (*argv[cnt] == 't') {
			int val = atoi(argv[cnt + 1]);
			if (val > 0 && val < 128) {
				num_threads = val;
			}
		}

		if (*argv[cnt] == 'v') {
			int val = atoi(argv[cnt + 1]);
			if (val == 0 || val == 1) {
				version_flag = val;
			}
		}

		if (*argv[cnt] == 's') {
			int val = atoi(argv[cnt + 1]);
			if (val > 0 && val < 1000000000) {
				input_size = val;
			}
		}

		if (*argv[cnt] == 'd') {
			int val = atoi(argv[cnt + 1]);
			if (val > 0 && val < 7) {
				depth = val;
			}
		}

		if (*argv[cnt] == 'o') {
			int val = atoi(argv[cnt + 1]);
			if (val >= 0 && val < 3) {
				switch (val) {
				case 0:
					order = ASCENDING;
					break;
				case 1:
					order = DESCENDING;
					break;
				case 2:
					order = RANDOM;
					break;
				}
			}
		}

	}


	std::vector<int> data;
	data.reserve(input_size);
	std::vector<int> w_data;

	/* Fill vector. */
	switch (order) {
	case ASCENDING:
		for (int i = 0; i < input_size; i++) {
			data.push_back(i);
		}
		break;
	case DESCENDING:
		for (int i = 0; i < input_size; i++) {
			data.push_back(input_size - i);
		}
		break;
	case RANDOM:
		for (long i = 0; i < input_size; i++) {
			data.push_back(rand());
		}
		break;
	}

	/*Copy to the 'working' vector*/
	w_data = data;

	const char* version_str = (version_flag) ? "Parallel  " : "Sequential";
	max_depth = depth;
	PrintVector(data);
	PrintVector(w_data);
	std::cout << "++++\n";

	if (version_flag) { //Parallel
		std::string_view version(version_str, 10);
		ScopedTimer timer(version);
		TopDownSplitParSec(w_data, 0, input_size, data, 0);
	}
	else { //Seq
		std::string_view version(version_str, 10);
		ScopedTimer timer(version);
		TopDownSplitSeq(w_data, 0, input_size, data);
	}


	PrintVector(data);
	PrintVector(w_data);

	std::cin.get();
}
