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
}

void gameboy_print_tile(gameboy *gba, int tile) {
    for (int i = 0; i < TILE_SIZE; i++) {
        printf("%02X ", gba->tiles_ptr_array[tile][i]);
    }
    printf("\n");
}
