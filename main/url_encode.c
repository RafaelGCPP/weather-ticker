#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Verifica se o char é seguro (não precisa de encoding)
// RFC 3986 section 2.3 Unreserved Characters (alpha, digit, -, ., _, ~)
int is_url_safe(char ch)
{
    if (isalnum((unsigned char)ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~')
    {
        return 1;
    }
    return 0;
}

void url_encode(char *str)
{
    if (str == NULL)
        return;

    size_t len = strlen(str);

    char *encoded = (char *)malloc(len * 3 + 1);
    if (encoded == NULL)
        return;

    const char *hex = "0123456789ABCDEF";
    size_t pos = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (is_url_safe(str[i]))
        {
            encoded[pos++] = str[i];
        }
        else
        {
            encoded[pos++] = '%';
            encoded[pos++] = hex[(str[i] >> 4) & 0x0F];
            encoded[pos++] = hex[str[i] & 0x0F];
        }
    }

    encoded[pos] = '\0';
    strcpy(str, encoded); // Copia de volta para str
    free(encoded);
}