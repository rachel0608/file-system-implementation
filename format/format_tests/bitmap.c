void initialize_bitmap(FILE *disk_image, superblock sb) {
    BitmapBlock bitmap_block;

    // Initialize the bitmap block with all bits set to 1 (indicating free)
    memset(bitmap_block.bitmap, 0xFF, BLOCKSIZE);

    // set first 10 blocks as used for test (change for testing)
    for (int i = 0; i < 10; i++) {
        bitmap_block.bitmap[i / 8] &= ~(1 << (i % 8)); // Set corresponding bit to 0 (used)
    }

    // Write the bitmap block to the disk image
    fseek(disk_image, (BLOCKSIZE * sb.FREEMAP_offset), SEEK_SET); // Move to the position after the boot sector
    fwrite(&bitmap_block, sizeof(BitmapBlock), 1, disk_image);
}

//tests bitmap with first 10 blocks used
void test_bitmap(FILE *disk_image, superblock sb) {
    BitmapBlock bitmap_block;

    // Read the bitmap block from the disk image
    fseek(disk_image, BLOCKSIZE * sb.FREEMAP_offset, SEEK_SET); // Move to the position after the boot sector
    fread(&bitmap_block, sizeof(BitmapBlock), 1, disk_image);

    // Validate the bitmap contents
    int num_used_blocks = 0;
    for (int i = 0; i < sb.file_size_blocks; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        int is_used = !(bitmap_block.bitmap[byte_index] & (1 << bit_index));

        if (is_used) {
            printf("Block %d is used\n", i);
            num_used_blocks++;
        }
    }

    if (num_used_blocks == 10) {
        printf("Bitmap initialization test passed: Expected 10 used blocks\n");
    } else {
        printf("Bitmap initialization test failed: Expected 10 used blocks, found %d\n", num_used_blocks);
    }
}
