#ifndef BITMAP_H
#define BITMAP_H

#include "gba.h"

typedef struct {
    unsigned size_x;
    unsigned size_y;
    size_t n_sprites;
    unsigned char colors[16][3];
    size_t pixel_data_size;
    unsigned char *pixel_data;
} bitmap;

/*  Initializes the bitmap and allocate it to memory
    
    Returns a pointer to the bitmap struct or returns 
    NULL if there was an error
*/
bitmap *bitmap_init(FILE *f);

/*  Frees the given bitmap */
void bitmap_free(bitmap *bmp);

/*  Write stats of the bitmap */
void bitmap_stats(const bitmap *bmp);

/*  Prints the image data */
void bitmap_print_pixel_data(const bitmap *bmp);

/*  Checks if both sizes are multiples of 8

    Returns 1 if true, 0 if it fails
*/
int bitmap_size_correct(const bitmap *bmp);

/*  Gets the horizontal size of the bitmap 

    Returns the x lenght of the bitmap
*/
unsigned bitmap_get_x_length(const bitmap *bmp);

/*  Gets the vertical size of the bitmap 

    Returns the y lenght of the bitmap
*/
unsigned bitmap_get_y_length(const bitmap *bmp);

/*  Sets the number of sprites based on bitmap dimensions */
void bitmap_set_n_sprites(bitmap *bmp);

/*  Gets the number of sprites in the bitmap 

    Return the amount of sprites
*/
size_t bitmap_get_n_sprites(const bitmap *bmp);

/*  Gets the size of the pixel data of the bitmap 

    Returns the size of the image data
*/
size_t bitmap_get_pixel_data_size(const bitmap *bmp);

/*  Reads a number of bytes from a certain offset in a file with
    an endianness of little.
    
    Returns a size_t number or 0 if file is NULL
*/
size_t file_read_le(FILE *f, int n_bytes, long offset);

/*  Sets the colors from the file to the bitmap struct */
void bitmap_set_colors(FILE *f, bitmap *bmp);

/*  Swaps the index of two colors and changes the pixel data too, so there
    are no actual differences
*/
void bitmap_swap_entries(bitmap *bmp, unsigned char a, unsigned char b);

/*  Checks if tire colors are present and puts them in the right index */
void bitmap_swap_tires(bitmap *bmp);

/*  Gets the first color index using individual R, G and B values 

    Returns the index
*/
int bitmap_get_color_index(bitmap *bmp, int r, int g, int b);

/*  Checks to see if a color exists using individual R, G and B values 

    Returns the amount of times the color is present
*/
int bitmap_contains_color(bitmap *bmp, int r, int g, int b);

/*  Gets the first color index using individual R, G and B values

    Returns a short with gba BGR555 color values
*/
unsigned short bitmap_color_to_gba(bitmap *bmp, int idx);

/*  Gets the swapped pixel data of two pixels on the bitmap, given an offset 

    Returns a single 4bit pixel
*/
unsigned char bitmap_get_pixel(const bitmap *bmp, const long offset);

/*  Reads all the pixels in a tile and stores that

    Returns a pointer to a whole tile stored on the heap
*/
unsigned char *bitmap_read_tile(const bitmap *bmp, long offset);

/*  Calculates the starting offset of a tile

    Returns a pointer to a whole tile stores on the heap
*/
unsigned char *bitmap_get_tile(const bitmap *bmp, const unsigned sprite_id, const unsigned tile);

/*  Converts a bitmap type to a gba type
    
    Returns a pointer to a gba type on the heap
*/
gameboy *bitmap_convert_to_gba(const bitmap *bmp);

#endif
