// Embedded C implementation for 100x100 subframe background subtraction
#include <stdint.h>
#include <stdlib.h>

#define WIDTH 3124
#define HEIGHT 3030
#define SUBFRAME 100
#define MAX_SUBFRAMES_X ((WIDTH + SUBFRAME - 1) / SUBFRAME)
#define MAX_SUBFRAMES_Y ((HEIGHT + SUBFRAME - 1) / SUBFRAME)
#define MAX_SUBFRAMES (MAX_SUBFRAMES_X * MAX_SUBFRAMES_Y)

uint8_t image[HEIGHT][WIDTH];
int16_t processed_image[HEIGHT][WIDTH];
uint8_t medians[MAX_SUBFRAMES];

// Compare function for qsort
int compare(const void *a, const void *b) {
    return (*(uint8_t*)a - *(uint8_t*)b);
}

// Compute median of a block
uint8_t compute_median(uint8_t *block, int size) {
    qsort(block, size, sizeof(uint8_t), compare);
    if (size % 2 == 0)
        return (block[size/2 - 1] + block[size/2]) / 2;
    else
        return block[size/2];
}

void background_subtraction() {
    int median_index = 0;
    for (int row = 0; row < HEIGHT; row += SUBFRAME) {
        int row_end = (row + SUBFRAME < HEIGHT) ? row + SUBFRAME : HEIGHT;
        for (int col = 0; col < WIDTH; col += SUBFRAME) {
            int col_end = (col + SUBFRAME < WIDTH) ? col + SUBFRAME : WIDTH;
            int block_size = (row_end - row) * (col_end - col);
            uint8_t *block = (uint8_t *)malloc(block_size);
            int idx = 0;
            for (int i = row; i < row_end; i++) {
                for (int j = col; j < col_end; j++) {
                    block[idx++] = image[i][j];
                }
            }
            uint8_t median_val = compute_median(block, block_size);
            medians[median_index++] = median_val;
            idx = 0;
            for (int i = row; i < row_end; i++) {
                for (int j = col; j < col_end; j++) {
                    processed_image[i][j] = (int16_t)image[i][j] - median_val;
                }
            }
            free(block);
        }
    }
}
