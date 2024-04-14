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


// Mount the file system from the specified disk image to the root directory
int fs_mount(const char *disk_image) {
   
    FILE *disk = fopen(disk_image, "rb"); //using Linux's version for now
    if (disk == NULL) {
        perror("Failed to open disk image");
        return -1;
    }

    // Read file system metadata from the disk image- how do we do this?
    FileSystemMetadata metadata = read_file_system_metadata(disk);
    if (metadata.type != FILE_SYSTEM_TYPE_FAT12) {
        fprintf(stderr, "Unsupported file system type\n");
        fclose(disk); //using Linux's version for now
        return -1;
    }

    initialize_file_system_structures(metadata);

    // Mount the file system at the root directory ("/")
    mount_point = "/";
    file_system_root = metadata.root_directory;

    // Configure file system operations
    file_system_operations.open = &fs_open;
    file_system_operations.read = &fs_read;
    // etc etc.

    return 0; // Mounting successful
}

function initialize_users():
    // Create regular user and super user accounts
    create_user("regular_user", home_directory="/home/regular_user", is_super_user=False)
    create_user("super_user", home_directory="/home/super_user", is_super_user=True)

function create_user(username, is_super_user):
    // Create a new user with the specified username, home dir, and superuser status
    // Update user configuration files or data structures?
    set_user_permissions(username, is_super_user)

function set_user_permissions(username, is_super_user):
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
