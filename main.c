#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf(
            "Arguments for bmp to gba converter: "
            "\"bmppath\" -stats -colormode\n");
        return 0;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror("bitmap file null\n");
        exit(1);
    }

    bitmap *bmp = bitmap_init(f);
    fclose(f);

    gameboy *gba = bitmap_convert_to_gba(bmp);

    bitmap_stats(bmp);
    printf("\n\n");
    gameboy_stats(gba);
    for (int i = 0; i < (bitmap_get_n_sprites(bmp)*64); i++) {
        gameboy_print_tile(gba, i);
    }

    f = fopen("output.bin", "wb");
    fclose(f);

    bitmap_free(bmp);
    gameboy_free(gba);

    return 0;
}