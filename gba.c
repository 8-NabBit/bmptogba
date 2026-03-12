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
    printf("Number of colors: %zu\n", gba->n_colors);
    printf("Colors: ");
    for (int i = 0; i < N_COLORS; i++) {
        // swap hi lo
        unsigned short color = (gba->colors[i] << 8) | (gba->colors[i] >> 8);
        printf("%04X ", color);
    }
    printf("\n");
}

void gameboy_print_tile(gameboy *gba, int tile) {
    for (int i = 0; i < TILE_SIZE; i++) {
        printf("%02X ", gba->tiles_ptr_array[tile][i]);
    }
    printf("\n");
}

void gameboy_write_to_file(gameboy *gba) {
    FILE *f = fopen("tiles.gba", "wb");
    if (f == NULL) {
        fprintf(stderr, "error: tiles file ptr null\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < gba->n_tiles; i++) {
        int n = fwrite(gba->tiles_ptr_array[i], sizeof(unsigned char), TILE_SIZE, f);

        if (n != TILE_SIZE) {
            fprintf(stderr, "error: number of objects written is incorrect tiles\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    
    fclose(f);

    FILE *g = fopen("palette.bin", "wb");
    if (g == NULL) {
        fprintf(stderr, "error: palette file ptr null\n");
        exit(EXIT_FAILURE);
    }

    int n = fwrite(gba->colors, sizeof(unsigned short), gba->n_colors, g);
    if (n != gba->n_colors) {
        fprintf(stderr, "error: number of objects written is incorrect color");
        fclose(g);
        exit(EXIT_FAILURE);
    }
}