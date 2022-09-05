#ifndef UTIL_HH
#define UTIL_HH

#include <cstdlib>
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>

void* read(const char* path) {
    void* buffer;
    auto file = fopen(path, "rb");
    auto length = 0;
    if (!file) return nullptr; 

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(length);
    if(!buffer) return nullptr; 
    fread(buffer, 1, length, file);
    fclose(file);
    return buffer;
}


float random_between(float min, float max) {
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));

}

uint64_t time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

float length(float x, float y) {
    return sqrt((x*x)+(y*y));
}


#endif UTIL_HH