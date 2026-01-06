#include "utils/psk_generator.h"
#include "esp_random.h"
#include <string.h>

static const char psk_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

void generate_random_psk(char *output, size_t length) {
    if (output == NULL || length == 0) {
        return;
    }

    for (size_t i = 0; i < length; i++) {
        uint32_t random_val = esp_random();
        output[i] = psk_alphabet[random_val % 62];
    }
    output[length] = '\0';
}
