#ifndef TEXT_STATS_H
#define TEXT_STATS_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    size_t lines;
    size_t words;
    size_t bytes;
} TextStats;

int text_stats_count(FILE *stream, TextStats *result);

#endif
