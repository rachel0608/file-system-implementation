include filesystem.h

function main():
    // Check for existence of "DISK" file in current directory
    if !file_exists("DISK"):
        print("File system disk image not found. Please run 'format' command to create one.")
        return

    // Mount the file system from the "DISK" file
    fs_mount("DISK")
    
    // Initialize users and their permissions
    initialize_users()
    login_prompt()

    // Login_prompt() will call start_shell_for_user() if username + password are correct


function file_exists(filename):
    // Check if the given filename exists in the current directory
    return 1 if filename exists, 0 otherwise

function fs_mount(disk_image_path):
    // Open disk image file to read from
    disk_image = open_file(disk_image_path, "rb")
    if disk_image is None:
        print("Failed to open disk image")
        return -1
    
    // Read the boot sector to obtain file system metadata
    boot_sector = read_boot_sector(disk_image)
    
    // Validate the boot sector and determine file system type (FAT12)
    if not validate_boot_sector(boot_sector):
        print("Invalid file system on disk image")
        close_file(disk_image)
        return -1
    
    // Initialize file system metadata from boot sector
    metadata = initialize_metadata(boot_sector)
    
    // Read the File Allocation Table (FAT) from disk image into memory
    fat_table = read_fat_table(disk_image, boot_sector)
    
    // Read the root directory from the disk image
    root_directory = read_root_directory(disk_image, boot_sector, metadata)
    
    // Close the disk image file
    close_file(disk_image)
    
    // Set up file system operations for accessing the mounted file system
    initialize_file_system_operations()
    
    // Store mounted file system metadata and structures for future use
    store_mounted_file_system(metadata, fat_table, root_directory)
    
    return 0  // Mounting successful

// HelperFunc: read boot sector from disk image
function read_boot_sector(disk_image):
    boot_sector = read_bytes(disk_image, 0, size_of(BootSector))
    return boot_sector

// HF: validate boot sector and determine file system type
function validate_boot_sector(boot_sector):
    // Perform validation checks based on boot sector contents
    if boot_sector.bytes_per_sector != 512 or boot_sector.num_fats != 2:
        return false
    // Additional checks for specific file system type (e.g., FAT12)
    return true

// HF: initialize file system metadata from boot sector
function initialize_metadata(boot_sector):
    metadata.bytes_per_sector = boot_sector.bytes_per_sector
    metadata.sectors_per_cluster = boot_sector.sectors_per_cluster
    metadata.num_fats = boot_sector.num_fats
    // Initialize other metadata fields
    return metadata

// HF: read FAT table from disk image
function read_fat_table(disk_image, boot_sector):
    fat_offset = boot_sector.reserved_sector_count * boot_sector.bytes_per_sector
    fat_table_size = boot_sector.sectors_per_fat * boot_sector.bytes_per_sector
    fat_table = read_bytes(disk_image, fat_offset, fat_table_size)
    return fat_table

// HF: read root directory from disk image
function read_root_directory(disk_image, boot_sector, metadata):
    root_dir_offset = (boot_sector.reserved_sector_count + (boot_sector.num_fats * boot_sector.sectors_per_fat)) * boot_sector.bytes_per_sector
    root_directory = read_directory_entries(disk_image, root_dir_offset, metadata.root_entry_count)
    return root_directory

// HF: init file system operations
function initialize_file_system_operations():
    file_system_operations.open = fs_open
    file_system_operations.read = fs_read
    // Initialize other file system operations

// HF: store mounted file system metadata and DS
function store_mounted_file_system(metadata, fat_table, root_directory):
    mounted_file_system.metadata = metadata
    mounted_file_system.fat_table = fat_table
    mounted_file_system.root_directory = root_directory

// add helper functions for reading bytes, directory entries, etc.

function initialize_users():
    // Create regular user and super user accounts
    create_user("regular_user", home_directory="/home/regular_user", is_super=False)
    create_user("super_user", home_directory="/home/super_user", is_super=True)

function create_user(username, home_dir, is_super):
    // Create a new user with the specified username, home dir, and superuser status
    // update user configuration files or data structures?
    set_user_permissions(username, is_super)

function set_user_permissions(username, is_super):
    // Set file system permissions for the given user based on superuser status
    // Use flags to denote superuser status

function login_prompt():
    // Display a login prompt to the user
    while True:
        username = input("Username: ")
        password = input("Password: ")
        if authenticate_user(username, password):
            start_shell_for_user(username)
            break
        else:
            print("Invalid username or password. Please try again.")

function authenticate_user(username, password):
    // Authenticate the user by checking username and password
    // This might involve checking against a user database or file

function start_shell_for_user(username):
    // Start the shell for the authenticated user
     main loop: 
        // get command
        input = readline("${username}> ");
        
        if (!input):
            printf("\n");
            break;
    
        if (*input):
            execute_command(input);    

function execute_command(command):
    // Use previously established, working execute_command()
