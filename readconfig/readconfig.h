#pragma once
#include <stdio.h>
#include <cstdlib>
#include <format>

int readConfig(const char* filename, int* threadCount, unsigned long long* upperRange);