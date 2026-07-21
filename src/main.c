#include "text_stats.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int show_lines;
    int show_words;
    int show_bytes;
    int first_file_index;
} Options;

static void print_usage(FILE *stream, const char *program_name)
{
    fprintf(stream,
            "Usage: %s [OPTION]... FILE...\n"
            "Count lines, words, and bytes in each FILE.\n\n"
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
    int index;

    memset(options, 0, sizeof(*options));
    options->first_file_index = argc;

    for (index = 1; index < argc; index++) {
        const char *argument = argv[index];

        if (strcmp(argument, "--") == 0) {
            options->first_file_index = index + 1;
            break;
        }

        if (strcmp(argument, "-h") == 0
            || strcmp(argument, "--help") == 0) {
            return 1;
        }

        if (argument[0] == '-' && argument[1] != '\0') {
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

        options->first_file_index = index;
        break;
    }

    if (options->first_file_index >= argc) {
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

static void print_stats(const TextStats *stats,
                        const Options *options,
                        const char *label)
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

    printf("%s\n", label);
}

static int count_file(const char *program_name,
                      const char *file_name,
                      TextStats *stats)
{
    FILE *stream;

    if (strcmp(file_name, "-") == 0) {
        stream = stdin;
    } else {
        stream = fopen(file_name, "rb");
        if (stream == NULL) {
            fprintf(stderr,
                    "%s: cannot open '%s': %s\n",
                    program_name,
                    file_name,
                    strerror(errno));
            return 1;
        }
    }

    if (text_stats_count(stream, stats) != 0) {
        fprintf(stderr,
                "%s: cannot read '%s'\n",
                program_name,
                file_name);
        if (stream != stdin) {
            fclose(stream);
        }
        return 1;
    }

    if (stream != stdin && fclose(stream) == EOF) {
        fprintf(stderr,
                "%s: cannot close '%s': %s\n",
                program_name,
                file_name,
                strerror(errno));
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    Options options;
    TextStats total = {0, 0, 0};
    size_t successful_files = 0;
    size_t file_count;
    int has_error = 0;
    int index;
    int parse_result = parse_options(argc, argv, &options);

    if (parse_result > 0) {
        print_usage(stdout, argv[0]);
        return 0;
    }

    if (parse_result < 0) {
        print_usage(stderr, argv[0]);
        return 2;
    }

    file_count = (size_t)(argc - options.first_file_index);

    for (index = options.first_file_index; index < argc; index++) {
        TextStats stats;

        if (count_file(argv[0], argv[index], &stats) != 0) {
            has_error = 1;
            continue;
        }

        print_stats(&stats, &options, argv[index]);
        total.lines += stats.lines;
        total.words += stats.words;
        total.bytes += stats.bytes;
        successful_files++;
    }

    if (file_count > 1 && successful_files > 0) {
        print_stats(&total, &options, "total");
    }

    return has_error ? 1 : 0;
}
