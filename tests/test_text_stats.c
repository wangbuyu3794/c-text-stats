#include "text_stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_FILE_NAME "text_stats_test_input.tmp"

static int failures = 0;

static FILE *create_test_stream(void)
{
    return fopen(TEST_FILE_NAME, "w+b");
}

static void check_stats(const char *name,
                        const char *input,
                        size_t expected_lines,
                        size_t expected_words,
                        size_t expected_bytes)
{
    FILE *stream = create_test_stream();
    TextStats actual;

    if (stream == NULL) {
        fprintf(stderr, "FAIL %s: could not create test file\n", name);
        failures++;
        return;
    }

    if (fwrite(input, 1, strlen(input), stream) != strlen(input)
        || fflush(stream) == EOF
        || fseek(stream, 0, SEEK_SET) != 0) {
        fprintf(stderr, "FAIL %s: could not prepare test input\n", name);
        fclose(stream);
        remove(TEST_FILE_NAME);
        failures++;
        return;
    }

    if (text_stats_count(stream, &actual) != 0) {
        fprintf(stderr, "FAIL %s: counting failed\n", name);
        fclose(stream);
        remove(TEST_FILE_NAME);
        failures++;
        return;
    }

    fclose(stream);
    remove(TEST_FILE_NAME);

    if (actual.lines != expected_lines
        || actual.words != expected_words
        || actual.bytes != expected_bytes) {
        fprintf(stderr,
                "FAIL %s: expected {%zu, %zu, %zu}, got {%zu, %zu, %zu}\n",
                name,
                expected_lines,
                expected_words,
                expected_bytes,
                actual.lines,
                actual.words,
                actual.bytes);
        failures++;
        return;
    }

    printf("PASS %s\n", name);
}

static void check_invalid_arguments(void)
{
    TextStats stats;
    FILE *stream = create_test_stream();

    if (stream == NULL) {
        fprintf(stderr,
                "FAIL invalid arguments: could not create test file\n");
        failures++;
        return;
    }

    if (text_stats_count(NULL, &stats) == 0
        || text_stats_count(stream, NULL) == 0) {
        fprintf(stderr, "FAIL invalid arguments\n");
        failures++;
    } else {
        printf("PASS invalid arguments\n");
    }

    fclose(stream);
    remove(TEST_FILE_NAME);
}

int main(void)
{
    check_stats("empty", "", 0, 0, 0);
    check_stats("one word", "hello", 0, 1, 5);
    check_stats("one line", "hello world\n", 1, 2, 12);
    check_stats("mixed whitespace", "one\n\ntwo\tthree\n", 3, 3, 15);
    check_stats("crlf lines", "a\r\nb\r\n", 2, 2, 6);
    check_invalid_arguments();

    if (failures != 0) {
        fprintf(stderr, "%d test(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    printf("All tests passed\n");
    return EXIT_SUCCESS;
}
