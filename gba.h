#ifndef GBA_H
#define GBA_H

#include <stdlib.h>

typedef struct {
    size_t n_tiles;
    unsigned char **tiles_ptr_array;
} gameboy;

/*  Free all components of the gameboy struct */
void gameboy_free(gameboy *gba);

/*  Write stats of the gameboy */
void gameboy_stats(gameboy *gba);

/*  Prints all the contents of a tile */
void gameboy_print_tile(gameboy *gba, int tile);

#endif