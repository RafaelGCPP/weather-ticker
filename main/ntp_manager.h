#pragma once
#include <stddef.h>
#include <stdbool.h>
#define NTP_SYNC_INTERVAL_MS (10 * 60 * 1000) // 10 minutes

/**
 * @brief Starts the NTP service and sets the Timezone
 * Can be safely called multiple times (checks if already running)
 */
void ntp_manager_init(void);

/**
 * @brief Forces an immediate resynchronization (optional)
 */
void ntp_manager_sync(void);

/**
 * @brief Gets the current time formatted as a string
 * 
 * @param local If true, returns local time; if false, returns UTC
 * @param buffer Buffer where the formatted string will be stored
 * @param max_len Maximum buffer size
 */
void get_time(bool local, char *buffer, size_t max_len);
