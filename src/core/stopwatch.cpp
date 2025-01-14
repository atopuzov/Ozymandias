#include "stopwatch.h"
#include <algorithm>
#include <SDL.h>
#include <cinttypes>
#include <cmath>

#define CLAMP(x, upper, lower) (std::min<int>(upper, std::max<int>(x, lower)))

int stopwatch::STAMPS() {
    return stamps.size() - 1;
}
void stopwatch::START() {
    stamps.clear();
    repeats = 0;
    index = 0;
    RECORD("<none>");
}
void stopwatch::RECORD(std::string str) {
    if (index >= stamps.size()) {
        std::vector<std::chrono::time_point<std::chrono::system_clock>> nv;
        stamps.push_back(nv);
        if (index > 0)
            names.push_back(str);
    }
    auto array = &stamps.at(index);
    int a = array->size();
    array->push_back(std::chrono::high_resolution_clock::now());
    int b = array->size();
    index++;
}
uint64_t stopwatch::GET(int a, int b) {
    if (stamps.size() == 0)
        return 0;
    a = CLAMP(a, stamps.size() - 1, 0);
    b = CLAMP(b, stamps.size() - 1, 0);
    if (a == b)
        return 0;

    uint64_t ms = 0;
    auto arr_a = &stamps.at(a);
    auto arr_b = &stamps.at(b);
    auto arr_a_size = arr_a->size();
    auto arr_b_size = arr_b->size();

    int repeats_proper = repeats + 1;
    repeats_proper = std::min<int>(repeats_proper, std::min<int>(arr_a_size, arr_b_size));

    for (int i = 0; i < repeats_proper; ++i) {
        auto s_a = arr_a->at(i);
        auto s_b = arr_b->at(i);
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(s_b - s_a);
        ms += dur.count();
    }

    return ms / repeats_proper;
}
uint64_t stopwatch::GET(int a) {
    return GET(a, a + 1);
}
uint64_t stopwatch::STOP(std::string str) {
    RECORD(str);
    return GET(stamps.size() - 2);
}
void stopwatch::REPEAT() {
    if (index > 1) {
        index = 0;
        repeats++;
        RECORD("<none>");
    }
}
void stopwatch::LOG() {
    for (int i = 0; i < STAMPS(); ++i) {
        auto ms = GET(i);
        char bar[200] = "";
        for (int j = 0; j < std::min<int>(2.5 * log(ms + 1), 199); ++j)
            strcat(bar, "!");
        strcat(bar, "\0");
        SDL_Log("%02i %20s : %4" PRIu64 " %s\n",
                i, names.at(i).c_str(), ms, bar);
    }
}