typedef struct {
    unsigned size_x;
    unsigned size_y;
    size_t n_sprites;
    unsigned char colors[16][3];
    size_t pixel_data_size;
    unsigned char *pixel_data;
} bitmap;

typedef struct {
    size_t n_tiles;
    unsigned char **tiles_ptr_array;
} gameboy;

/*  Initializes the bitmap and allocate it to memory
    
    Returns a pointer to the bitmap struct or returns 
    NULL if there was an error
*/
bitmap *bitmap_init(FILE *f);

/*  Write stats of the bitmap and gba */
void print_stats(const bitmap *bmp, const gameboy *gba);

void bitmap_free(bitmap *bmp);

/*  Checks if both sizes are multiples of 8

    Returns 1 if true, 0 if it fails
*/
int bitmap_size_correct(const bitmap *bmp);

/*  Gets the horizontal size of the bitmap */
unsigned bitmap_get_x_length(const bitmap *bmp);

/*  Gets the vertical size of the bitmap */
unsigned bitmap_get_y_length(const bitmap *bmp);

/*  Sets the number of sprites based on bitmap dimensions 

    Side effects: sets the number of sprites in the given bitmap
*/
void bitmap_set_n_sprites(bitmap *bmp);

/*  Gets the number of sprites in the bitmap */
size_t bitmap_get_n_sprites(const bitmap *bmp);

/*  Gets the size of the pixel data of the bitmap */
size_t bitmap_get_pixel_data_size(const bitmap *bmp);

/*  Gets the swapped pixel data of two pixels on the bitmap, given an offset */
unsigned char bitmap_get_pixel(const bitmap *bmp, const long offset);

/*  Reads a number of bytes from a certain offset in a file with
    an endianness of little.
    
    Returns a size_t number or 0 if file is NULL
*/
size_t file_read_le(FILE *f, int n_bytes, long offset);

/*  Converts a bitmap type to a gba type
    
    Returns a pointer to a gba type on the heap
*/
gameboy *bitmap_convert_to_gba(const bitmap *bmp);

void gameboy_free(gameboy *gba);