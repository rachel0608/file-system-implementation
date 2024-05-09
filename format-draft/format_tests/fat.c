void initialize_fat(FILE *disk_image, superblock sb) {

    //each block of FAT has 256 entries
    int num_entries_per_block = BLOCKSIZE/BYTES_PER_ENTRY;
    int num_blocks = sb.file_size_blocks;

    // Allocate memory & initialize all FAT entries as free
    FATEntry *fat_table = (FATEntry *)malloc(num_blocks * sizeof(FATEntry));

    for (int i = 0; i < num_blocks; i++) {
        fat_table[i].block_number = FREEBLOCK;
    }
    fat_table[2000].block_number = 2020;
    fat_table[2020].block_number = 2040;
    fat_table[2040].block_number = 2047;
    fat_table[2047].block_number = 0;

    // Write FAT to disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fwrite(fat_table, (num_blocks * sizeof(FATEntry)), 1, disk_image);

    free(fat_table);

}

void test_readfat(FILE *disk_image, superblock sb) {

    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, sb.file_size_blocks * sizeof(FATEntry), 1, disk_image);

    for (int i = 0; i < sb.file_size_blocks; i++) {
        printf("fat cell %d: %d\n", i, fat_entries[i].block_number);
    }

}

void test_fat_end_of_chain(FILE *disk_image, superblock sb) {

    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, sb.file_size_blocks * sizeof(FATEntry), 1, disk_image);

    // Simulate file allocation (chain of clusters)
    int start_cluster = 2000;
    int current_cluster = start_cluster;

    while (fat_entries[current_cluster].block_number != END_OF_CHAIN) {
        current_cluster = fat_entries[current_cluster].block_number;
        printf("Next cluster in chain: %d\n", current_cluster);
    }

    // read back and verify FAT entries for the file's clusters
    test_readfat(disk_image, sb);

void test_cluster_allocation(FILE *disk_image, superblock sb) {
    // Assuming you have initialized FATEntry array and disk_image file pointer

    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk_image);

    // Simulate file allocation (chain of clusters)
    int start_cluster = 2000;
    int current_cluster = start_cluster;

    // Allocate clusters for a new file (chain them in FAT)
    while (current_cluster < sb.file_size_blocks) {
        if (fat_entries[current_cluster].block_number == FREEBLOCK) {
            fat_entries[current_cluster].block_number = END_OF_CHAIN;
            break;
        }
        current_cluster++;
    }

    // Write updated FAT entries back to disk
    //fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    //fwrite(fat_entries, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk_image);

    // read back and verify FAT entries for the allocated clusters
    test_readfat(disk_image, sb);
}

    
    // Verify that last cluster in chain points to end of chain
    printf("Last cluster in chain: %d\n", current_cluster);

}

