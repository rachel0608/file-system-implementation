void test_sb(superblock sb) {
    printf("Block Size = %d\n", sb.block_size);
    printf("File Size in MB = %d\n", sb.file_size_mb);
    printf("File Size in Blocks = %d\n", sb.file_size_blocks);
    printf("FAT Offset = %d\n", sb.FAT_offset);
    printf("FREEMAP Offset = %d\n", sb.FREEMAP_offset);
    printf("ROOTDIR Offset = %d\n", sb.ROOTDIR_offset);
    printf("DATA Offset = %d\n\n", sb.DATA_offset);
}

void test_readsb(FILE *disk_image) {

    superblock sb;
    fseek(disk_image, 0, SEEK_SET);
    fread(&sb, sizeof(superblock), 1, disk_image);
    test_sb(sb);

}
