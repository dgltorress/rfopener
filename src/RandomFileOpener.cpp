// RandomFileOpener.cpp : main file for the Random File Opener app for Windows
// @author Daniel Giménez López-Torres

#include <iostream>      /* console IO */
#include <windows.h>     /* Windows API functions */
#include <string>        /* strings */
#include <vector>        /* dynamic containers */

#include <stdlib.h>      /* srand, rand */
#include <time.h>        /* time */

#include "termcolor.h"   /* easy console colors, available at https://github.com/ikalnytskyi/termcolor */

#include "FileManager.h" 

// Enumerates actions.
enum Action{
    xOpen = 0, 
    xScan = 1,
    xNone = -1
};

/**
* @brief Displays app usage.
*/
static void showUsage( const char* name )
{
    std::cerr << "Usage: " << termcolor::bright_blue << name << termcolor::reset << " [" << termcolor::bright_yellow << "-opts" << termcolor::reset << "]\n\n"
              << "Options:\n"

              << termcolor::bright_yellow << "-h" << termcolor::reset << "," << termcolor::bright_yellow << " --help" << termcolor::reset
                  << "\t\tShow this help message.\n"

              << termcolor::bright_yellow << "-s" << termcolor::reset << "," << termcolor::bright_yellow << " --scan" << termcolor::reset
              << " [" << termcolor::bright_cyan << "DIRECTORY" << termcolor::reset << "]"
                  << "\t\tRecursively scan and store file paths from the specified directory and its subdirectories into a file within said directory.\n"

              << termcolor::bright_yellow << "\t-d" << termcolor::reset << "," << termcolor::bright_yellow << " --depth" << termcolor::reset
              << " [" << termcolor::bright_cyan << "DEPTH" << termcolor::reset << "]"
                  << "\t\tMaximum (inclusive) depth the recursive iterator is allowed to reach. min. " << FileManager::minDepth << ", max. " << FileManager::maxDepth << ", default " << FileManager::depthDefault << " (0 equals to the working directory).\n"

              << termcolor::bright_yellow << "-o" << termcolor::reset << "," << termcolor::bright_yellow << " --open" << termcolor::reset
              << " [" << termcolor::bright_cyan << "DIRECTORY" << termcolor::reset << "]"
                  << "\t\tOpens a file corresponding to a random path from an already created scan file located in the specified directory.\n"
              
              << termcolor::bright_yellow << "\t-m" << termcolor::reset << "," << termcolor::bright_yellow << " --memory" << termcolor::reset
                  << "\t\tWhether to enable Memory Mode, which stores all read paths into memory. Grants constant complexity to subsequent accesses to random files, but cost could potentially be prohibitive. Disabled by default.\n\n";
}

/**
* @brief main
*/
int main( int argc , char* argv[] ) {
    // Displays app name and version.
    std::cout << termcolor::bright_green << "\nRandom File Opener v1.0.0\n\n" << termcolor::reset;

    // Initializes random seed.
    srand( (unsigned int)time( NULL ) );

    // Declares and initializes variables.
    int action = xNone;                // Default action to take. None by default.
    int depth = -1;                    // Depth to iterate to. Undefined (invalid value) by default.
    std::string scanFileDirectoryPath; // Path to the scanned paths file.
    std::vector<std::string> paths;    // Path string vector.
    bool memoryMode = false;           // Whether memory mode is enabled. Disables by default.

    // Argument check
    if( argc <= 1 ){
        std::cerr << "Argument expected. Type --help to see all available commands." << std::endl;
        return EXIT_FAILURE;
    }
    else{
        std::string arg;

        // Parse arguments.
        for( int i = 1 ; i < argc ; ++i ){
            arg = argv[i];

            // Help (usage)
            if( ( arg == "-h" ) || ( arg == "--help" ) ){
                showUsage( argv[0] );
                return 0;
            }
            // Enable memory mode (store all paths in memory instead of looping every time)
            if( ( arg == "-m" ) || ( arg == "--memory" ) ){
                memoryMode = true;
            }
            // Scan depth.
            else if( ( arg == "-d" ) || ( arg == "--depth" ) ){
                try {
                    if ( ( ++i >= argc ) ) throw std::invalid_argument( "Depth is not a valid integer" );
                    depth = std::stoi( argv[i] );
                }
                catch (const std::exception& ex) {
                    std::cerr << termcolor::bright_red << ex.what() << termcolor::reset << std::endl;
                    exit( EXIT_FAILURE );
                }
            }
            // Try to set actions if none of the above.
            else{
                // Only if unset.
                if( action == xNone ) {
                    // Scan (store recursive file names)
                    if( ( arg == "-s" ) || ( arg == "--scan" ) ){
                        action = xScan;
                    }
                    // Open (open a random file from the list)
                    else if( ( arg == "-o" ) || ( arg == "--open" ) ){
                        action = xOpen;
                    }

                    // Gets directory if an action is to be performed
                    if( action != xNone ){
                        try{
                            if( ( ++i >= argc ) ||
                                !( std::filesystem::exists( argv[i] ) ) ){
                                throw std::invalid_argument( "The provided scan file path is not valid. Try placing it between quotation marks such as: \"C:\\...\"" );
                            }
                        } catch( const std::exception& ex ){
                            std::cerr << termcolor::bright_red << "ERROR enabling file IO:\n" << ex.what() << termcolor::reset << std::endl;
                            exit( EXIT_FAILURE );
                        }
                        scanFileDirectoryPath = argv[i];
                    }
                }
            }
        }
    }

    // Performs an action if it was specified.
    switch( action ){
        // Scan paths.
        case xScan: {
            // Instantiates a file manager.
            FileManager fileManager = FileManager( scanFileDirectoryPath );

            // Recursively scans and stores the file paths whether a preferred depth has been provided.
            if( depth != -1 ){
                fileManager.storePathsRecursively( depth );
            }
            else{
                fileManager.storePathsRecursively();
            }
        } break;
        // Open file.
        case xOpen: {
            // Instantiates a file manager.
            FileManager fileManager = FileManager( scanFileDirectoryPath );
            // Loop variable.
            std::string openAgain = "n";

            // (Option 1 [memory mode]) Stores all paths into memory. cost could potentially be prohibitive.
            if( memoryMode == true ){
                // Displays the memory mode prompt.
                std::cout << termcolor::magenta << "Memory mode enabled." << termcolor::reset <<
                    " Paths will be stored in memory instead of the file being looped every time.\n";

                // Read ALL of the paths already stored in a file into memory.
                fileManager.readPaths( paths );

                // Reserves space for path strings.
                std::string filePath; // UTF8
                std::wstring wideFilePath; // Wide
                
                // Open files until explicitly told otherwise.
                do{
                    // Constructs the path.
                    filePath = fileManager.getWorkingDirectory() + paths[ rand() % paths.size() ];
                    wideFilePath = FileManager::utf8ToWide( filePath );

                    // Displays the path.
                    std::cout << "Opening " << termcolor::bright_cyan << filePath << termcolor::reset << " ...\n";

                    // Tries to open the file corresponding to the path.
                    ShellExecuteW( 0 , 0 , wideFilePath.c_str() , 0 , 0 , SW_SHOW );

                    // Asks whether to open another file.
                    std::cout << "Open another file? (y/n): ";
                    std::cin >> openAgain;
                } while( ( openAgain == "y" ) ||
                         ( openAgain == "Y" ) );
            }
            // (Option 2 [direct mode]) Reads up to a random path every time (default)
            else {
                do{
                    // Relays the task to the file manager.
                    fileManager.openRandomFilePath();

                    // Asks whether to open another file.
                    std::cout << "Open another file? (y/n): ";
                    std::cin >> openAgain;
                } while ( ( openAgain == "y" ) ||
                          ( openAgain == "Y" ) );
            }
        } break;

        // None.
        default: {
            showUsage( argv[0] );
        } break;
    }
    

    return EXIT_SUCCESS;
}