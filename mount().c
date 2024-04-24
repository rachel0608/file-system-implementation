//to be called before the mainloop
void fs_mount(char *diskname) {
    //extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func
    
    FILE *disk = fopen(diskname, "rb");
    //read + define superblock
    fseek(disk, 0, SEEK_SET);
    fread(sb, sizeof(superblock), 1, disk);

    //read + define FAT
    fseek(disk, BLOCKSIZE * sb.FAT_offset, SEEK_SET);
    fread(fat, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk);

    //read + define bitmap
    fseek(disk, BLOCKSIZE * sb.FREEMAP_offset, SEEK_SET);
    fread(bitmap, sizeof(BitmapBlock), 1, disk);

    //read + define rootdirentry
    fseek(disk, BLOCKSIZE * sb.ROOTDIR_offset, SEEK_SET);
    fread(root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 32 bytes

    //read + define datablock section
    fseek(disk, BLOCKSIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
    fread(data_section, BLOCKSIZE * sb.file_size_blocks, 1, disk); //Directory is size: 512 bytes

    fclose(disk);
}
