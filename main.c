#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

#define N_REQUIRED_ARGS 1

enum Flags {
    STATS,
    NORACER,
    N_FLAGS
};

int main(int argc, char* argv[]) {

    // no args
    if (argc == 1) {
        printf(
            "Arguments for bmp to gba converter: "
            "\"bmppath\" [-stats] [-noracer]\n");
        return 0;
    }

    // start with path
    if (argv[1][0] == '-') {
        printf("error: first argument must be a bitmap path\n");
        return 1;
    }

    // check all flags
    int flags[N_FLAGS] = {0};
    for (int i = N_REQUIRED_ARGS+1; i < argc; i++) {
        if (strcmp(argv[i], "-stats") == 0) {
            flags[STATS] = 1;
        }
        else if (strcmp(argv[i], "-noracer") == 0) {
            flags[NORACER] = 1;
        }
        else {
            printf("warning: unknown flag '%s'\n", argv[i]);
        }
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror("bitmap file null\n");
        exit(EXIT_FAILURE);
    }

    bitmap *bmp = bitmap_init(f);
    fclose(f);

    if (flags[STATS]) {
        bitmap_stats(bmp);
        printf("\n\n");
    }

    if (!flags[NORACER]) {
        bitmap_swap_tires(bmp);
    }

    gameboy *gba = bitmap_convert_to_gba(bmp);
    gameboy_write_to_file(gba);

    if (flags[STATS]) {
        gameboy_stats(gba);
    }

    bitmap_free(bmp);
    gameboy_free(gba);

    printf("Successfully converted!\n");

    return 0;
}
