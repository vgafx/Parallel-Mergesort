#ifndef SCOPEDTIMER_H
#define SCOPEDTIMER_H
#include <iostream>
#include <chrono>
#include <string_view>

using namespace std::literals::chrono_literals;

class ScopedTimer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<double> duration;
public:
	ScopedTimer(std::string_view);
	~ScopedTimer();
};

#endif
