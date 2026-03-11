#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "macros.h"

void gameboy_free(gameboy *gba) {
    if (!gba) {
        return;
    }

    for (unsigned i = 0; i < gba->n_tiles; i++) {
        free(gba->tiles_ptr_array[i]);
    }
    free(gba->tiles_ptr_array);
    free(gba);
}

void gameboy_stats(gameboy *gba) {
    printf("GBA 4BPP:\n");
    printf("Amount of tiles: %zu\n", gba->n_tiles);
    printf("Colors: \n");
    for (int i = 0; i < N_COLORS; i++) {
        printf("%04X ", gba->colors[i]);
    }
    printf("\n");
}

void gameboy_print_tile(gameboy *gba, int tile) {
    for (int i = 0; i < TILE_SIZE; i++) {
        printf("%02X ", gba->tiles_ptr_array[tile][i]);
    }
    printf("\n");
}

int gameboy_write_to_file(gameboy *gba) {
    FILE *f = fopen("output.gba", "wb");
    if (f == NULL) {
        fprintf(stderr, "error: output file ptr null\n");
        return 0;
    }

    for (size_t i = 0; i < gba->n_tiles; i++) {
        int n = fwrite(gba->tiles_ptr_array[i], sizeof(unsigned char), TILE_SIZE, f);

        if (!(n == TILE_SIZE)) {
            printf("Number of objects written is incorrect\n");
            fclose(f);
            return 0;
        }
    }
    
    fclose(f);
    return 1;
}