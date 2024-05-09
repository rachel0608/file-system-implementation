function main(argc, argv):
    // Constants
    DISK_SIZE_MB_DEFAULT = 1
    EXPECTED_ARGS = 2
    MAX_NAME = 8

    // Parse command-line arguments
    if argc < EXPECTED_ARGS:
        print("Usage: {EXECUTABLE} <filename> -s <num MB requested>")
        return EXIT_FAILURE

    // Initialize variables
    disk_size_mb = DISK_SIZE_MB_DEFAULT
    filename = argv[1]

    // Check for disk size flag
    if argc > EXPECTED_ARGS and argv[2] == "-s":
        disk_size_mb = atoi(argv[3])

    // Open disk image file for writing
    disk_image = open_file(filename, "wb")
    if disk_image is None:
        print("Failed to create disk image file")
        return EXIT_FAILURE

    // Initialize the disk image with file system structures
    initialize_disk_image(disk_image, disk_size_mb)
    close_file(disk_image)

    print("Disk image formatted successfully:", filename)
    return EXIT_SUCCESS

function initialize_disk_image(disk_image, disk_size_mb):
    // Initialize the FAT (File Allocation Table)
    initialize_fat(disk_image, disk_size_mb)

    // Initialize the Master File Table (MFT)
    initialize_mft(disk_image)

    // Initialize the root directory
    initialize_root_directory(disk_image)

    // Initialize the boot sector
    initialize_boot_sector(disk_image, disk_size_mb)

function initialize_fat(disk_image, disk_size_mb):
    // Calculate number of blocks (clusters) based on disk size
    num_blocks = disk_size_mb * 1024  // Assuming 1 block = 512 bytes

    // Allocate memory for FAT entries
    fat_entries = allocate_memory(num_blocks * sizeof(FATEntry))
    if fat_entries is None:
        print("Memory allocation error")
        exit(EXIT_FAILURE)

    // Initialize FAT entries
    for i in range(num_blocks):
        if i == 0 or i == 1:
            // Reserved blocks (boot block, superblock)
            fat_entries[i].value = 0xFFF  // Mark as reserved (end of file)
        else:
            // Free cluster
            fat_entries[i].value = 0x000  // Mark as free

    // Write FAT entries to disk image
    write_to_disk(disk_image, fat_entries, num_blocks * sizeof(FATEntry))

    // Free allocated memory
    free_memory(fat_entries)

function initialize_mft(disk_image):
    // Initialize the root directory entry in the MFT
    root_directory_entry = initialize_file_handle("ROOT", 0, FIRST_BLOCK_ROOT_DIR, DIR_ATTR)
    write_to_disk(disk_image, root_directory_entry, sizeof(FileHandle))

    // Write other MFT entries for system files (if needed)

function initialize_root_directory(disk_image):
    // Initialize the root directory entry
    root_entry = initialize_directory_entry(".", "", DIR_ATTR, FIRST_BLOCK_ROOT_DIR, 0)
    write_to_disk(disk_image, root_entry, sizeof(DirectoryEntry))

function initialize_boot_sector(disk_image, disk_size_mb):
    // Initialize the boot sector
    boot_sector = initialize_boot_sector_struct(disk_size_mb)
    write_to_disk(disk_image, boot_sector, sizeof(BootSector))

// Helper functions

function initialize_file_handle(filename, file_size, start_cluster, attributes):
    file_handle = create_new_file_handle()
    file_handle.filename = filename
    file_handle.file_size = file_size
    file_handle.start_cluster = start_cluster
    file_handle.attributes = attributes
    return file_handle

function initialize_directory_entry(filename, ext, attributes, start_cluster, file_size):
    directory_entry = create_new_directory_entry()
    directory_entry.filename = filename
    directory_entry.ext = ext
    directory_entry.attributes = attributes
    directory_entry.start_cluster = start_cluster
    directory_entry.file_size = file_size
    return directory_entry

function initialize_boot_sector_struct(disk_size_mb):
    boot_sector = create_new_boot_sector()
    boot_sector.bytes_per_sector = 512
    boot_sector.sectors_per_cluster = 1
    boot_sector.reserved_sector_count = 1
    boot_sector.num_fats = 2
    boot_sector.root_entry_count = 512
    boot_sector.total_sectors = disk_size_mb * 1024 / boot_sector.bytes_per_sector
    boot_sector.media_descriptor = 0xF8
    boot_sector.sectors_per_fat = 9  // Assuming 9 sectors per FAT for FAT-12
    boot_sector.sectors_per_track = 18
    boot_sector.num_heads = 2
    boot_sector.total_sectors_big = 0  // Not used for FAT-12
    return boot_sector
