#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

#define N_COLORS 16
#define TILE_SIZE 64
#define BMP_HEADER_X_SIZE_OFFSET 0x12
#define BMP_HEADER_Y_SIZE_OFFSET 0x16
#define BMP_HEADER_COLOR_OFFSET 0x36

bitmap *bitmap_init(FILE *f) {
    
    bitmap *bmp = malloc(sizeof(bitmap));

    // sizes
    bmp->size_x = (unsigned)read_le(f, 4, BMP_HEADER_X_SIZE_OFFSET);
    bmp->size_y = (unsigned)read_le(f, 4, BMP_HEADER_Y_SIZE_OFFSET);
    if (!bitmap_size_correct(bmp)) {
        perror("bitmap size not good\n");
        bitmap_free(bmp);
        exit(1);
    }

    // amount of sprites based on total image dimensions
    unsigned x_length = (unsigned)bitmap_get_x_length(bmp);
    unsigned y_length = (unsigned)bitmap_get_y_length(bmp);
    if (x_length > 64) {
        bmp->n_sprites = x_length / y_length;
    } else if (y_length > 64) {
        bmp->n_sprites = y_length / x_length;
    } else if (x_length == 64 || y_length == 64) {
        bmp->n_sprites = 1;
    }

    // load colors (uses the first 16 it sees)
    for (int i = 0; i < N_COLORS; i++) {
        for (int j = 0; j <= 2; j++) {
            bmp->colors[i][j] = (unsigned char)read_le(f, 1, BMP_HEADER_COLOR_OFFSET+i*4+j);
        }
    }
    
    // allocate pixel data based on certain sizes in file
    size_t total_size = read_le(f, 4, 0x2);
    size_t pixel_offset = read_le(f, 4, 0xA);
    bmp->pixel_data_size = total_size - pixel_offset;
    bmp->pixel_data = malloc(bmp->pixel_data_size);

    return bmp;
}

void bitmap_free(bitmap *bmp) {
    if (!bmp) {
        return;
    }
    free(bmp->pixel_data);
    free(bmp);
}

void bitmap_stats(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }
    printf("Image size: %ux%u\n", bitmap_get_x_length(bmp), bitmap_get_y_length(bmp));
    printf("Number of sprites: 0x%zu\n", bitmap_get_n_sprites(bmp));
    printf("Pixel data size: 0x%06lX\n", bitmap_get_pixel_data_size(bmp));

    printf("Colors: ");
    for (int i = 0; i < N_COLORS; i++) {
        for (int j = 0; j <= 2; j++) {
            printf("%02X", bmp->colors[i][j]);
        }
        printf(" ");
    }
    printf("\n");
}

int bitmap_size_correct(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }

    if (bitmap_get_x_length(bmp) % 8 != 0 || bitmap_get_y_length(bmp) % 8 != 0) {
        return 0;
    }
    return 1;
}

unsigned bitmap_get_x_length(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }
    return bmp->size_x;
}

unsigned bitmap_get_y_length(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }
    return bmp->size_y;
}

size_t bitmap_get_n_sprites(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }
    return bmp->n_sprites;
}

size_t bitmap_get_pixel_data_size(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }
    return bmp->pixel_data_size;
}

size_t read_le(FILE *f, int n_bytes, long offset) {
    if (f == NULL) {
        return 0;
    }

    size_t val = 0;
    fseek(f, offset, SEEK_SET);

    for (int i = 0; i < n_bytes; i++) {
        int c = fgetc(f);
        if (c == EOF) {
            perror("end of file character read\n");
            exit(1);
        }
        // bitshift right for even x
        // AND upper bits
        val |= ((size_t)c) << (8 * i);
    }
    return val;
}

unsigned char bitmap_get_pixel(const bitmap *bmp,
                               const unsigned sprite_id,
                               const unsigned x,
                               const unsigned y)
{
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }

    if (x > bitmap_get_x_length(bmp) || y > bitmap_get_y_length(bmp)) {
        perror("out of bounds");
        bitmap_free(bmp);
        exit(1);
    }

    unsigned row = (unsigned)bitmap_get_pixel_data_size(bmp) - bitmap_get_y_length(bmp)*y;
    unsigned col = sprite_id*64 + x;

    unsigned char p = bmp->pixel_data[row + col];

    if (x % 2 == 0) {
        return p;
    }
    return p;
}

unsigned char *bitmap_get_tile(const bitmap *bmp,
                               const unsigned sprite_id,
                               const unsigned tile)
{
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }

    unsigned x = bitmap_get_x_length(bmp) / bitmap_get_n_sprites(bmp);
    unsigned y = bitmap_get_y_length(bmp);
    unsigned pos = x * y;

    printf("%d, %d, %d\n\n", x, y, pos);

    return;
}

gba *bitmap_convert_to_gba(const bitmap *bmp) {
    if (bmp == NULL) {
        perror("bmp null");
        exit(1);
    }

    gba *g = malloc(bitmap_get_pixel_data_size(bmp));
    if (g == NULL) {
        perror("failed allocating gba");
        bitmap_free(bmp);
        exit(1);
    }

    // sprites
    for (int sprite = 0; sprite < bitmap_get_n_sprites(bmp); sprite++) {
        // only horizontal right now
        unsigned n_tiles = (bitmap_get_x_length(bmp) / bitmap_get_n_sprites(bmp) / 8) * 
                           (bitmap_get_y_length(bmp) / 8);
        unsigned char **tiles_ptr_array = malloc(n_tiles * sizeof(unsigned char *));

        // tiles
        for (int tile = 0; tile < n_tiles; tile++) {
            unsigned char *tile_ptr = bitmap_get_tile(bmp, sprite, tile);
            tiles_ptr_array[tile] = tile_ptr;
        }

    }

    return g;
}