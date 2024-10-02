#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int is_valid_date(int year, int month, int day) {
    // Check if the month is valid
    if (month < 1 || month > 12) {
        return 0; // Invalid month
    }

    // Check if the day is valid for the given month
    int days_in_month;
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            days_in_month = 31;
            break;
        case 4: case 6: case 9: case 11:
            days_in_month = 30;
            break;
        case 2:
            // Check for leap year
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                days_in_month = 29; // Leap year
            } else {
                days_in_month = 28; // Non-leap year
            }
            break;
        default:
            return 0; // Should not reach here
    }

    // Check if the day is valid
    if (day < 1 || day > days_in_month) {
        return 0; // Invalid day
    }

    return 1; // Valid date
}

int is_valid_time(int hour, int minute, int second) {
    // Validate hour, minute, and second
    if (hour < 0 || hour > 23) {
        return 0; // Invalid hour
    }
    if (minute < 0 || minute > 59) {
        return 0; // Invalid minute
    }
    if (second < 0 || second > 59) {
        return 0; // Invalid second
    }
    return 1; // Valid time
}

void execute_command_if_set() {
    char *command = getenv("ENDZEIT_EXEC_WHEN_DONE");
    if (command) {
        printf("\nExecuting command: %s\n", command);
        system(command); // Execute the command
    }
}

void countdown(int year, int month, int day, int hour, int minute, int second) {
    struct tm tm_time = {0};
    time_t target, now;

    tm_time.tm_year = year - 1900;
    tm_time.tm_mon = month - 1;
    tm_time.tm_mday = day;
    tm_time.tm_hour = hour;   // Set the hour
    tm_time.tm_min = minute;   // Set the minute
    tm_time.tm_sec = second;   // Set the second
    tm_time.tm_isdst = -1;     // Automatically determine whether to use DST

    // Calculate target time
    now = time(NULL);
    if (mktime(&tm_time) <= now) {
        // If the specified time is already in the past, set it to the next day
        tm_time.tm_mday += 1; // Move to the next day
    }

    target = mktime(&tm_time);

    while (1) {
        now = time(NULL);
        double seconds_left = difftime(target, now);

        if (seconds_left <= 0) {
            printf("\rendzeit!\n");
            execute_command_if_set(); // Execute command if set
            break;
        }

        int total_days = (int)(seconds_left / (3600 * 24)); // Total days until end time
        int hours = (int)(seconds_left / 3600) % 24; // Remaining hours within the last day
        int minutes = (int)(seconds_left / 60) % 60; // Remaining minutes
        int seconds = (int)seconds_left % 60; // Remaining seconds

        //int total_hours = (int)(seconds_left / 3600); // Total hours until end time
        //int total_seconds = (int)seconds_left; // Total seconds until end time

        printf("\rendzeit in %d days %02d:%02d:%02d", total_days, hours, minutes, seconds);

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

                // Validate the date
                if (!is_valid_date(year, month, day)) {
                    fprintf(stderr, "Invalid date. Please ensure the month is 1-12 and the day is valid for the month.\n");
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

                // Validate the time
                if (!is_valid_time(hour, minute, second)) {
                    fprintf(stderr, "Invalid time. Please ensure hour is 0-23 and minute/second are 0-59.\n");
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

