#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf(
            "Arguments for bmp to gba converter:\n"
            "\"bmppath\" -stats -colormode");
        return 0;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror("bitmap file null\n");
        exit(1);
    }

    bitmap *bmp = bitmap_init(f);
    fclose(f);

    bitmap_stats(bmp);

    gba *gameboy = bitmap_convert_to_gba(bmp);
    f = fopen("output.bin", "wb");
        
    
    fclose(f);

    return 0;
}