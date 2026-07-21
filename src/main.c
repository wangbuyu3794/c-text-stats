#include "text_stats.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int show_lines;
    int show_words;
    int show_bytes;
    const char *file_name;
} Options;

static void print_usage(FILE *stream, const char *program_name)
{
    fprintf(stream,
            "Usage: %s [OPTION]... FILE\n"
            "Count lines, words, and bytes in FILE.\n\n"
            "  -l    print the newline count\n"
            "  -w    print the word count\n"
            "  -c    print the byte count\n"
            "  -h    display this help and exit\n"
            "  -     read from standard input\n",
            program_name);
}

static int enable_option(Options *options, char option)
{
    switch (option) {
    case 'l':
        options->show_lines = 1;
        return 0;
    case 'w':
        options->show_words = 1;
        return 0;
    case 'c':
        options->show_bytes = 1;
        return 0;
    default:
        return -1;
    }
}

static int parse_options(int argc, char **argv, Options *options)
{
    int options_ended = 0;
    int index;

    memset(options, 0, sizeof(*options));

    for (index = 1; index < argc; index++) {
        const char *argument = argv[index];

        if (!options_ended && strcmp(argument, "--") == 0) {
            options_ended = 1;
            continue;
        }

        if (!options_ended
            && (strcmp(argument, "-h") == 0
                || strcmp(argument, "--help") == 0)) {
            return 1;
        }

        if (!options_ended && argument[0] == '-' && argument[1] != '\0') {
            size_t option_index;

            for (option_index = 1; argument[option_index] != '\0';
                 option_index++) {
                if (enable_option(options, argument[option_index]) != 0) {
                    fprintf(stderr,
                            "%s: invalid option -- '%c'\n",
                            argv[0],
                            argument[option_index]);
                    return -1;
                }
            }
            continue;
        }

        if (options->file_name != NULL) {
            fprintf(stderr, "%s: only one input file is supported\n", argv[0]);
            return -1;
        }

        options->file_name = argument;
    }

    if (options->file_name == NULL) {
        fprintf(stderr, "%s: missing input file\n", argv[0]);
        return -1;
    }

    if (!options->show_lines && !options->show_words
        && !options->show_bytes) {
        options->show_lines = 1;
        options->show_words = 1;
        options->show_bytes = 1;
    }

    return 0;
}

static void print_stats(const TextStats *stats, const Options *options)
{
    if (options->show_lines) {
        printf("%zu ", stats->lines);
    }
    if (options->show_words) {
        printf("%zu ", stats->words);
    }
    if (options->show_bytes) {
        printf("%zu ", stats->bytes);
    }

    printf("%s\n", options->file_name);
}

int main(int argc, char **argv)
{
    Options options;
    TextStats stats;
    FILE *stream;
    int parse_result = parse_options(argc, argv, &options);

    if (parse_result > 0) {
        print_usage(stdout, argv[0]);
        return 0;
    }

    if (parse_result < 0) {
        print_usage(stderr, argv[0]);
        return 2;
    }

    if (strcmp(options.file_name, "-") == 0) {
        stream = stdin;
    } else {
        stream = fopen(options.file_name, "rb");
        if (stream == NULL) {
            fprintf(stderr,
                    "%s: cannot open '%s': %s\n",
                    argv[0],
                    options.file_name,
                    strerror(errno));
            return 1;
        }
    }

    if (text_stats_count(stream, &stats) != 0) {
        fprintf(stderr,
                "%s: cannot read '%s'\n",
                argv[0],
                options.file_name);
        if (stream != stdin) {
            fclose(stream);
        }
        return 1;
    }

    if (stream != stdin && fclose(stream) == EOF) {
        fprintf(stderr,
                "%s: cannot close '%s': %s\n",
                argv[0],
                options.file_name,
                strerror(errno));
        return 1;
    }

    print_stats(&stats, &options);
    return 0;
}
