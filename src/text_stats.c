#include "text_stats.h"

#include <ctype.h>

int text_stats_count(FILE *stream, TextStats *result)
{
    int ch;
    int in_word = 0;

    if (stream == NULL || result == NULL) {
        return -1;
    }

    result->lines = 0;
    result->words = 0;
    result->bytes = 0;

    while ((ch = fgetc(stream)) != EOF) {
        result->bytes++;

        if (ch == '\n') {
            result->lines++;
        }

        if (isspace((unsigned char)ch)) {
            in_word = 0;
        } else if (!in_word) {
            result->words++;
            in_word = 1;
        }
    }

    return ferror(stream) ? -1 : 0;
}
