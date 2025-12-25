#ifndef BASE32_H
#define BASE32_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Generate a random base32 encoded string
 * 
 * @param output Buffer to store the base32 string (must be at least length+1 for null terminator)
 * @param length Desired length of the base32 string (in characters)
 */
void generate_random_psk(char *output, size_t length);

#endif // BASE32_H
