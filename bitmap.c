#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 32  // total number of blocks (small just for test)

// print the bitmap
void print_bitmap(unsigned char *bitmap, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", (bitmap[i] & (1 << j)) ? 1 : 0);
        }
    }
    printf("\n");
}

// init bitmap with 0 (free)
void initialize_bitmap(unsigned char *bitmap, int size) {
    memset(bitmap, 0, size);
    // set first bit as used (root dir)
    bitmap[0] |= 1 << 0; // doing OR operation on 0th bit and 1
}

// update the bitmap at a given block index to a specified value (0 or 1)
void update_bitmap(unsigned char *bitmap, int block_index, int value) {
    if (value == 0) {
        // set block as free
        bitmap[block_index / 8] &= ~(1 << (block_index % 8));
    } else {
        // set block as used
        bitmap[block_index / 8] |= 1 << (block_index % 8);
    }
}

int main() {
    // declare bitmap with alignment
    unsigned char bitmap[(BLOCK_SIZE + 7) / 8]; 

    // init bitmap
    initialize_bitmap(bitmap, sizeof(bitmap));
    printf("Initial Bitmap: ");
    print_bitmap(bitmap, sizeof(bitmap));

    // update bitmap to set block index 20 as used (1)
    update_bitmap(bitmap, 20, 1);
    printf("Bitmap after setting block 20 as used: ");
    print_bitmap(bitmap, sizeof(bitmap));

    // free block 20
    update_bitmap(bitmap, 20, 0);
    printf("Bitmap after setting block 20 as free: ");
    print_bitmap(bitmap, sizeof(bitmap));

    return 0;
}
