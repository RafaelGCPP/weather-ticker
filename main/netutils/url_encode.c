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

void url_encode(const char *src, char *dst, size_t dst_size)
{
    if (src == NULL || dst == NULL || dst_size == 0)
        return;

    const char *hex = "0123456789ABCDEF";
    size_t pos = 0;
    size_t len = strlen(src);

    for (size_t i = 0; i < len && pos < dst_size - 1; i++)
    {
        if (is_url_safe(src[i]))
        {
            dst[pos++] = src[i];
        }
        else if (pos < dst_size - 3)  // Precisa de 3 chars (%XX)
        {
            dst[pos++] = '%';
            dst[pos++] = hex[(src[i] >> 4) & 0x0F];
            dst[pos++] = hex[src[i] & 0x0F];
        }
        else
        {
            break;  // Não cabe mais
        }
    }

    dst[pos] = '\0';
}