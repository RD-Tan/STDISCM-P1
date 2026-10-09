#include "readconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int is_full_line(const char* line) {
    return strchr(line, '\n') != NULL;
}

int readConfig(const char* filename, int* threadCount, unsigned long long* upperRange) {
    FILE* file;
    auto err = fopen_s(&file, filename, "r");
    if (err) {
        perror("Error opening config file");
        return 1;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (!is_full_line(line)) {
            perror("an input is longer than buffer.");
            fclose(file);
            return 1;
        }

        char buffer[250];

        if (sscanf_s(line, "threadcount=%s", buffer, (unsigned)_countof(buffer)) == 1) {
            char* endptr;
            errno = 0;
            *threadCount = (int)strtol(buffer, &endptr, 10);
            if (endptr == buffer || *endptr != '\0') {
                perror("value given for thread count not valid base 10 number.");
                fclose(file);
                return 1;
            }
            if (errno == ERANGE) {
                perror("value given for thread exceeds int.");
                fclose(file);
                return 1;
            }
            if (*threadCount < 1) {
                perror("thread count given is zero or negative. Must be positive.");
                fclose(file);
                return 1;
            }
            continue;
        }

        if (sscanf_s(line, "upperRange=%s", buffer, (unsigned)_countof(buffer)) == 1) {

            char* endptr;
            errno = 0;
            *upperRange = strtoull(buffer, &endptr, 10) + 1;
            if (endptr == buffer || *endptr != '\0') {
                perror("value given for upper range not valid base 10 number.");
                fclose(file);
                return 1;
            }
            if (errno == ERANGE) {
                perror("value given for upper range exceeds unsigned long long int.");
                fclose(file);
                return 1;
            }
            if (*upperRange < 1) {
                perror("upper range given is zero or negative. Must be positive.");
                fclose(file);
                return 1;
            }
            continue;
        }
    }

    fclose(file);
    return 0;
}