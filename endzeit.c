#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void countdown(int year, int month, int day, int hour, int minute, int second) {
    struct tm tm_time = {0};
    time_t target, now;

    tm_time.tm_year = year - 1900;
    tm_time.tm_mon = month - 1;
    tm_time.tm_mday = day;
    tm_time.tm_hour = hour;
    tm_time.tm_min = minute;
    tm_time.tm_sec = second;

    target = mktime(&tm_time);

    while (1) {
        now = time(NULL);
        double seconds_left = difftime(target, now);

        if (seconds_left <= 0) {
            printf("\rThe time is up!\n");
            break;
        }

        int total_days = (int)(seconds_left / (3600 * 24)); // Total days until end time
        int hours = (int)(seconds_left / 3600) % 24; // Remaining hours within the last day
        int minutes = (int)(seconds_left / 60) % 60; // Remaining minutes
        int seconds = (int)seconds_left % 60; // Remaining seconds

        int total_hours = (int)(seconds_left / 3600); // Total hours until end time
        int total_seconds = (int)seconds_left; // Total seconds until end time

        printf("\rendzeit in: %d days %02d:%02d:%02d (%d hours : %d seconds) | Total seconds: %d", 
               total_days, hours, minutes, seconds, total_hours, seconds, total_seconds);
        fflush(stdout);  
        
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    int year, month, day;
    int hour = 24, minute = 0, second = 0;  // Default time is 24:00:00
    int date_set = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-date") == 0) {
            if (i + 1 < argc) {
                if (sscanf(argv[i + 1], "%d-%d-%d", &year, &month, &day) != 3) {
                    fprintf(stderr, "Invalid date format. Please use yyyy-mm-dd.\n");
                    return EXIT_FAILURE;
                }
                date_set = 1;
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Missing argument for -date.\n");
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-time") == 0) {
            if (i + 1 < argc) {
                if (sscanf(argv[i + 1], "%d:%d:%d", &hour, &minute, &second) != 3) {
                    fprintf(stderr, "Invalid time format. Please use hh:mm:ss.\n");
                    return EXIT_FAILURE;
                }
                i++;  // Skip the next argument
            } else {
                fprintf(stderr, "Missing argument for -time.\n");
                return EXIT_FAILURE;
            }
        }
    }

    if (!date_set) {
        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);
        year = local_time->tm_year + 1900;
        month = local_time->tm_mon + 1;
        day = local_time->tm_mday;
    }

    countdown(year, month, day, hour, minute, second);
    return EXIT_SUCCESS;
}

