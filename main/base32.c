#include "base32.h"
#include "esp_random.h"
#include <string.h>

// Base32 alphabet (RFC 4648)
static const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

void base32_generate_random(char *output, size_t length) {
    if (output == NULL || length == 0) {
        return;
    }

    for (size_t i = 0; i < length; i++) {
        uint32_t random_val = esp_random();
        output[i] = base32_alphabet[random_val % 32];
    }
    output[length] = '\0';
}
