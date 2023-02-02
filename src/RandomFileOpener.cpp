// RandomFileOpener.cpp : main file for the Random File Opener app for Windows
// @author Daniel Giménez López-Torres
//
// Compiled with Visual Studio Community 2022 (C++17)

#include <iostream>      /* console IO */
#include <windows.h>     /* Windows API functions */
#include <string>        /* strings */
#include <sstream>       /* stringstream for splitting */
#include <vector>        /* dynamic containers */
#include <filesystem>    /* navigate through files */

#include "termcolor.h"   /* easy console colors, available at https://github.com/ikalnytskyi/termcolor */

#include "FileManager.h" 

// Function declarations
void defaultAction( std::string& , std::vector<std::string>& , int , bool = true );
void showUsage( const char* );

// Global constant data
enum Actions{ xDefault = 0 , xHelp = 1 };
const struct Flags{
    const struct Shortened{
        const char* help       = "-h";
        const char* root       = "-r";
        const char* extensions = "-e";
        const char* depth      = "-d";
        const char* nocap      = "-nc";
    } shortened;
    const struct Whole{
        const char* help       = "--help";
        const char* root       = "--root";
        const char* extensions = "--extensions";
        const char* depth      = "--depth";
        const char* nocap      = "--nocap";
    } whole;
} flags;
const char extensionDelimiter = ';';

/**
* @brief main
*/
int main( int argc , char* argv[] ) {
    // Displays app name and version.
    std::cout << termcolor::bright_green << "\nRandom File Opener v1.1.0\n\n" << termcolor::reset;

    // Declares and initializes variables.
    std::string directoryPathString;       // Path to the root directory as a string.
    std::vector<std::string> extensions;   // Extension whitelist.
    int depth = FileManager::depthDefault; // Maximum depth to iterate to.
    bool areCapsEnabled = true;            // Whethter soft caps are enabled.
    
    int action = xDefault; // Action to perform.

    // Parse arguments.
    std::string arg;
    for( int i = 1 ; i < argc ; ++i ){
        // Get current argument.
        arg = argv[ i ];

        // Help (show usage).
        if( ( arg == flags.shortened.help ) || ( arg == flags.whole.help ) ){
            action = xHelp;
        }
        // Set root directory.
        else if( ( arg == flags.shortened.root ) || ( arg == flags.whole.root ) ){
            try{
                if( ( ++i >= argc ) ){
                    throw std::invalid_argument( "No path for the root directory was provided" );
                }
                else if( !( std::filesystem::exists( argv[ i ] ) ) ){
                    throw std::invalid_argument( "The provided path for the root directory is not valid." );
                }
                directoryPathString = argv[ i ];
            } catch( const std::exception& ex ){
                std::cerr << termcolor::bright_red << "ERROR resolving path for the root directory:\n" << ex.what() << termcolor::reset << std::endl;
                exit( EXIT_FAILURE );
            }
        }
        // Enable extension whitelisting and provide matches. An empty string counts as an extension.
        else if( ( arg == flags.shortened.extensions ) || ( arg == flags.whole.extensions ) ){
            try{
                if( ( ++i >= argc ) ){
                    throw std::invalid_argument( "Extension whitelisting was enabled, but no extensions were provided" );
                }

                // Split string into individual extensions.
                // Include a dot at the beginning for std::filesystem::path::extension compatibility.
                std::string temp;
                std::stringstream stringstream { argv[ i ] };
                while( std::getline( stringstream , temp , extensionDelimiter ) ){
                    extensions.emplace_back( '.' + temp );
                }
            } catch( const std::exception& ex ){
                std::cerr << termcolor::bright_red << "ERROR while whitelisting extensions:\n" << ex.what() << termcolor::reset << std::endl;
                exit( EXIT_FAILURE );
            }
        }
        // Set maximum depth.
        else if( ( arg == flags.shortened.depth ) || ( arg == flags.whole.depth ) ){
            try{
                if ( ( ++i >= argc ) ) throw std::invalid_argument( "No depth was provided" );
                depth = std::stoi( argv[ i ] );
            } catch( const std::exception& ex ){
                std::cerr << termcolor::bright_red << "ERROR while establishing maximum depth:\n" << ex.what() << termcolor::reset << std::endl;
                exit( EXIT_FAILURE );
            }
        }
        // Disable soft caps.
        else if( ( arg == flags.shortened.nocap ) || ( arg == flags.whole.nocap ) ){
            areCapsEnabled = false;
        }
    }

    // Performs an action if it was specified.
    switch( action ){
        // Default action
        case xDefault: defaultAction( directoryPathString , extensions , depth, areCapsEnabled ); break;
        // Help
        case xHelp: showUsage( argv[ 0 ] ); break;

        // None.
        default: break;
    }

    return EXIT_SUCCESS;
}

/**
* @brief Performs the default action (recursively iterates and stores the canonical paths into memory).
*/
void defaultAction( std::string& directoryPathString , std::vector<std::string>& extensions , int depth , bool checkCaps ){
    // Instantiates a file manager in the current directory or, if provided, a different one.
    FileManager fileManager = FileManager( directoryPathString );

    // Read ALL of the paths recursively into memory.
    fileManager.readPaths( extensions , depth, checkCaps );

    // Loop variables.
    std::string openAgain = "n";
    bool exit = false;
    bool validOption = false;

    // Open files until explicitly told otherwise.
    do{
        // Executes a random file.
        fileManager.executeRandomFile(  );

        // Asks whether to open another file.
        do{
            std::cout << "Open another file? (y/n): ";
            std::cin >> openAgain;

            // Checks the answer.
            if( ( openAgain == "y" ) || ( openAgain == "Y" ) ){
                exit = false;
                validOption = true;
            }
            else if( ( openAgain == "n" ) || ( openAgain == "N" ) ){
                exit = true;
                validOption = true;
            }
            else validOption = false;
        } while( validOption == false );
        
    } while( exit == false );
}

/**
* @brief Displays app usage.
*/
void showUsage( const char* name ){
    std::cerr << "Usage: " << termcolor::bright_blue << name << termcolor::reset << " [" << termcolor::bright_yellow << "-opts" << termcolor::reset << "]\n\n"
              << "Options:\n"

              << termcolor::bright_yellow << flags.shortened.help << termcolor::reset << ", " << termcolor::bright_yellow << flags.whole.help << termcolor::reset
                  << "\t\tShow this help message.\n"

              << termcolor::bright_yellow << flags.shortened.root << termcolor::reset << ", " << termcolor::bright_yellow << flags.whole.root << termcolor::reset
              << " [" << termcolor::bright_cyan << "DIRECTORY" << termcolor::reset << "]"
                  << "\t\tRelative or absolute path to the root directory. Defaults to the current working directory.\n"

              << termcolor::bright_yellow << flags.shortened.extensions << termcolor::reset << ", " << termcolor::bright_yellow << flags.whole.extensions << termcolor::reset
              << " [" << termcolor::bright_cyan << "extension1" << termcolor::reset << ";"
                      << termcolor::bright_cyan << "extension2" << termcolor::reset << ";...;"
                      << termcolor::bright_cyan << "extensionN" << termcolor::reset << "]"
                  << "\t\tEnables file extension whitelisting. Only files that exactly match any of the specified extensions (separated by a semicolon) will be taken into account.\n"

              << termcolor::bright_yellow << flags.shortened.depth << termcolor::reset << ", " << termcolor::bright_yellow << flags.whole.depth << termcolor::reset
              << " [" << termcolor::bright_cyan << "DEPTH" << termcolor::reset << "]"
                  << "\t\tMaximum (inclusive) levels of depth the recursive iterator is allowed to reach (min. "
                  << termcolor::bright_cyan << FileManager::minDepth     << termcolor::reset << ", max. "
                  << termcolor::bright_cyan << FileManager::maxDepth     << termcolor::reset << ", default. "
                  << termcolor::bright_cyan << FileManager::depthDefault << termcolor::reset
                  << "). 0 equals to the working directory.\n"
              
              << termcolor::bright_yellow << flags.shortened.nocap << termcolor::reset << ", " << termcolor::bright_yellow << flags.whole.nocap << termcolor::reset
                  << "\t\tDisable soft caps for storage of relative paths in memory (max. "
                  << termcolor::bright_cyan << FileManager::maxPaths << termcolor::reset
                  << " paths) and depth levels (max. "
                  << termcolor::bright_cyan << FileManager::maxDepth << termcolor::reset
                  << " levels). Caps are enabled by default.\n\n";
}