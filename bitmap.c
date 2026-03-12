#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"
#include "macros.h"

#define SPRITE_DIMENSION 64
#define BMP_HEADER_X_SIZE_OFFSET 0x12
#define BMP_HEADER_Y_SIZE_OFFSET 0x16
#define BMP_HEADER_COLOR_OFFSET(f) (file_read_le(f, 4, 0xE) + 0xE)

enum RGB {
    BLUE,
    GREEN,
    RED
};

bitmap *bitmap_init(FILE *f) {
    
    bitmap *bmp = malloc(sizeof(bitmap));
    if (!bmp) {
        fprintf(stderr, "error: bmp null init\n");
        exit(EXIT_FAILURE);
    }

    // sizes
    bmp->size_x = (unsigned)file_read_le(f, 4, BMP_HEADER_X_SIZE_OFFSET);
    bmp->size_y = (unsigned)file_read_le(f, 4, BMP_HEADER_Y_SIZE_OFFSET);
    if (!bitmap_size_correct(bmp)) {
        fprintf(stderr, "error: bitmap size not good\n");
        bitmap_free(bmp);
        exit(EXIT_FAILURE);
    }

    // amount of sprites based on total image dimensions
    bitmap_set_n_sprites(bmp);

    // load colors (uses the first 16 it sees)
    bitmap_set_colors(f, bmp);
    
    // allocate pixel data based on certain sizes in file
    size_t total_size = file_read_le(f, 4, 0x2);
    size_t pixel_offset = file_read_le(f, 4, 0xA);
    bmp->pixel_data_size = total_size - pixel_offset;
    bmp->pixel_data = malloc(bmp->pixel_data_size);
    if (!bmp->pixel_data) {
        fprintf(stderr, "error: bmp pixel data init failed\n");
        exit(EXIT_FAILURE);
    }

    if (fseek(f, (long)pixel_offset, SEEK_SET) != 0) {
        fprintf(stderr, "error: fseek to pixel data failed\n");
        bitmap_free(bmp);
        exit(EXIT_FAILURE);
    }

    size_t data_read = fread(bmp->pixel_data, 1, bmp->pixel_data_size, f);
    if (data_read != bmp->pixel_data_size) {
        fprintf(stderr, "error: fread pixel_data failed\n");
        bitmap_free(bmp);
        exit(EXIT_FAILURE);
    }

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
        fprintf(stderr, "error: bmp or gba null\n");
        exit(EXIT_FAILURE);
    }
    printf("Bitmap 4BPP:\n");
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
}

void bitmap_print_pixel_data(const bitmap *bmp) {
    if (bmp == NULL || bmp->pixel_data == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < bmp->pixel_data_size; i++) {
        printf("%02X ", bmp->pixel_data[i]);
    }
    printf("\n");
}

int bitmap_size_correct(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    if (bitmap_get_x_length(bmp) % 8 != 0 || bitmap_get_y_length(bmp) % 8 != 0) {
        return 0;
    }
    return 1;
}

unsigned bitmap_get_x_length(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }
    return bmp->size_x;
}

unsigned bitmap_get_y_length(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }
    return bmp->size_y;
}

void bitmap_set_n_sprites(bitmap *bmp) {
    bmp->n_sprites = (bitmap_get_x_length(bmp) / SPRITE_DIMENSION) * (bitmap_get_y_length(bmp) / SPRITE_DIMENSION);
}

size_t bitmap_get_n_sprites(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }
    return bmp->n_sprites;
}

size_t bitmap_get_pixel_data_size(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }
    return bmp->pixel_data_size;
}

size_t file_read_le(FILE *f, int n_bytes, long offset) {
    if (f == NULL) {
        return 0;
    }

    size_t val = 0;
    fseek(f, offset, SEEK_SET);

    for (int i = 0; i < n_bytes; i++) {
        int c = fgetc(f);
        if (c == EOF) {
            fprintf(stderr, "error: end of file character read\n");
            exit(EXIT_FAILURE);
        }
        // bitshift right for even x
        // AND upper bits
        val |= ((size_t)c) << (8 * i);
    }
    return val;
}

void bitmap_swap_entries(bitmap *bmp, unsigned char a, unsigned char b) {
    if (bmp == NULL || bmp->pixel_data == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    if (a >= N_COLORS || b >= N_COLORS) {
        fprintf(stderr, "error: can't swap entries that dont exist");
        exit(EXIT_FAILURE);
    }

    a &= 0x0F;
    b &= 0x0F;

    // build remap table with identity, but swap a and b
    unsigned char remap[16];
    for (int i = 0; i < 16; i++) {
        remap[i] = i;
        remap[a] = b;
        remap[b] = a;
    
        // swap palette color entries
        for (int j = 0; j < 3; j++) {
            unsigned char temp = bmp->colors[a][j];
            bmp->colors[a][j] = bmp->colors[b][j];
            bmp->colors[b][j] = temp;
        }
    }

    // remap every nibble in pixel data
    for (size_t i = 0; i < bmp->pixel_data_size; i++) {
        unsigned char lo = remap[bmp->pixel_data[i] & 0x0F];
        unsigned char hi = remap[(bmp->pixel_data[i] >> 4) & 0x0F];
        bmp->pixel_data[i] = (unsigned char)((hi << 4) | lo);
    }
}

void bitmap_swap_tires(bitmap *bmp) {

}

void bitmap_set_colors(FILE *f, bitmap *bmp) {
    for (int i = 0; i < N_COLORS; i++) {
        for (int j = 0; j <= 2; j++) {
            bmp->colors[i][j] = (unsigned char)file_read_le(f, 1, BMP_HEADER_COLOR_OFFSET(f)+i*4+j);
        }
    }
}

unsigned short bitmap_color_to_gba(bitmap *bmp, int idx)
{
    unsigned short r = bmp->colors[idx][RED];
    unsigned short g = bmp->colors[idx][GREEN];
    unsigned short b = bmp->colors[idx][BLUE];

    unsigned short color =
          ((r >> 3) & 31)
        | (((g >> 3) & 31) << 5)
        | (((b >> 3) & 31) << 10);
    
    // set highest unused bit, not perfect but mksc is very inconsistent with it
    color |= ((r >= 128 && g >= 128 && b >= 128) << 15);

    return color;
}

unsigned char bitmap_get_pixel(const bitmap *bmp, const long offset) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    if (bmp->pixel_data == NULL) {
        fprintf(stderr, "error: bmp pixel_data null\n");
        exit(EXIT_FAILURE);
    }

    if (offset < 0 || (size_t)offset >= bitmap_get_pixel_data_size(bmp)) {
        fprintf(stderr, "error: pixel offset out of bounds\n");
        exit(EXIT_FAILURE);
    }

    // swap the lower and higher 4 bits
    unsigned char p = bmp->pixel_data[offset];
    return (unsigned char)(((p & 0x0F) << 4) | ((p & 0xF0) >> 4));
}

unsigned char *bitmap_read_tile(const bitmap *bmp, long offset) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    unsigned char *data = malloc(TILE_SIZE);
    if (data == NULL) {
        fprintf(stderr, "error: failed allocating tile data\n");
        exit(EXIT_FAILURE);
    }

    long row_width = (long)(bitmap_get_x_length(bmp) / 2);

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 4; col++) {
            data[row * 4 + col] = bitmap_get_pixel(bmp, offset + col);
        }
        offset -= row_width;
    }

    return data;
}

unsigned char *bitmap_get_tile(const bitmap *bmp, const unsigned sprite_id, const unsigned tile) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    if (sprite_id >= bitmap_get_n_sprites(bmp)) {
        fprintf(stderr, "error: sprite id is not in bounds\n");
        exit(EXIT_FAILURE);
    }

    unsigned sprites_per_row = bitmap_get_x_length(bmp) / SPRITE_DIMENSION;
    long row_width = (long)(bitmap_get_x_length(bmp) / 2);

    unsigned tile_row = tile / 8;
    unsigned tile_col = tile % 8;

    unsigned sprite_row = sprite_id / sprites_per_row;
    unsigned sprite_col = sprite_id % sprites_per_row;

    // bottom-left of the sprite sheet is offset 0 in BMP
    // calculate the bottom row of this specific tile
    long tile_x_byte = (long)((sprite_col * SPRITE_DIMENSION + tile_col * 8) / 2);
    long tile_y_row = (long)((sprite_row * SPRITE_DIMENSION + tile_row * 8));

    // BMP starts at bottom, so invert y
    long tile_offset = (long)(bitmap_get_pixel_data_size(bmp)) - (tile_y_row + 1) * row_width + tile_x_byte;

    return bitmap_read_tile(bmp, tile_offset);
}

gameboy *bitmap_convert_to_gba(const bitmap *bmp) {
    if (bmp == NULL) {
        fprintf(stderr, "error: bmp null\n");
        exit(EXIT_FAILURE);
    }

    gameboy *gba = malloc(sizeof(gameboy));
    if (gba == NULL) {
        fprintf(stderr, "error: failed allocating gba\n");
        exit(EXIT_FAILURE);
    }

    size_t n_tiles = bitmap_get_n_sprites(bmp) * TILES_IN_SPRITE;
    gba->n_tiles = n_tiles;

    gba->tiles_ptr_array = malloc(n_tiles * sizeof(unsigned char *));
    if (gba->tiles_ptr_array == NULL) {
        fprintf(stderr, "error: failed allocating ptr array\n");
        exit(EXIT_FAILURE);
    }

    // sprites
    for (int sprite = 0; sprite < bitmap_get_n_sprites(bmp); sprite++) {
        // tiles
        for (int tile = 0; tile < TILES_IN_SPRITE; tile++) {
            unsigned char *tile_ptr = bitmap_get_tile(bmp, sprite, tile);
            gba->tiles_ptr_array[sprite * TILES_IN_SPRITE + tile] = tile_ptr;
        }
    }

    // colors
    gba->n_colors = 0;
    for (int i = 0; i < N_COLORS; i++) {
        // printf("%u, %u, %u\n", bmp->colors[i][RED], bmp->colors[i][GREEN], bmp->colors[i][BLUE]);
        gba->colors[i] = bitmap_color_to_gba(bmp, i);
        gba->n_colors++;
        // printf("%04X\n", gba->colors[i]);
    }

    return gba;
}
