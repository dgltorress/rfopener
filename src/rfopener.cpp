/**
 * RandomFileOpener.cpp : main file for the Random File Opener app for Windows
 * @author Daniel Giménez López-Torres
 *
 * Compiled with Visual Studio Community 2022 (C++17)
 * Source code and binaries available at https://github.com/dgltorress/rfopener
 */

#include <iostream>      // console IO
#include <windows.h>     // Windows API functions
#include <string>        // strings
#include <sstream>       // stringstream for splitting
#include <vector>        // dynamic containers
#include <filesystem>    // navigate through files
#include <conio.h>       // Get keystrokes

#include "termcolor.h"   // easy console colors, available at https://github.com/ikalnytskyi/termcolor

#include "Args.h"
#include "Keys.h"
#include "FileManager.h"

static const char* VERSION = "2.0.0";

// Global constants
const enum Actions{
    xDefault,
    xPlaylist,
    xHelp
};

FileManager* buildFileManager(
    std::string& directoryPathString,
	std::vector<std::string>& forbiddenDirectories,
    std::vector<std::string>& allowedExtensions,
    int depth,
    bool checkCaps
) {
    // Instantiates a file manager in the current directory or, if provided, a different one.
    FileManager* fileManager = new FileManager(directoryPathString);

    // Read ALL of the paths recursively into memory.
    fileManager->readPaths(forbiddenDirectories, allowedExtensions, depth, checkCaps);

    return fileManager;
}

/**
 * @brief Allows the user to open one random file at a time until the designated exit key(s) is pressed.
 * 
 * @param fileManager File manager instance.
*/
void randomLoop(FileManager* fileManager) {
    std::cout << "\nPress Enter to open additional files, or Esc to exit\n";
    int c;
    do {
        c = _getch();
        if (Keys::isConfirmKey(c)) {
            fileManager->executeRandomFile();
        }
    } while (!Keys::isExitKey(c));
}

/**
 * @brief Allows the user to navigate through a shuffled file array until the designated exit key(s) is pressed.
 * 
 * @param fileManager File manager instance.
*/
void sequentialLoop(FileManager* fileManager) {
    std::cout << "\nPress the left and right arrow keys to navigate the playlist, or Esc to exit\n" << std::endl;
    int c;
    do {
        c = _getch();
        c = Keys::refreshArrow(c);
        if (Keys::isForwardKey(c)) {
            fileManager->executeSequentialFile();
        } else if (Keys::isBackKey(c)) {
            fileManager->executeSequentialFile(true);
        }
    } while (!Keys::isExitKey(c));
}

/**
 * @brief Performs the default action.
*/
void defaultAction(FileManager* fileManager){
    // Reads a file.
    fileManager->executeRandomFile();

    // Open random files loop.
    randomLoop(fileManager);
}

/**
 * @brief Performs the playlist action.
*/
void playlistAction(FileManager* fileManager){
    // Shuffles the read paths.
    fileManager->shuffle();

    // Opens the first file.
    fileManager->executeFirstFile();

    // Open files sequentially loop.
    sequentialLoop(fileManager);
}

/**
* @brief Displays a help message.
* 
* @param programName Name of the executable file.
*/
void showUsage(const char* programName){
    std::cerr <<
        "Usage: " << termcolor::bright_blue << programName << termcolor::reset << " [" << termcolor::bright_yellow << "-opts" << termcolor::reset << "]\n\n"
     << "Options:\n"
     
     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::help] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::help] << termcolor::reset
         << "\t\tShow this help message.\n"
     
     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::root] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::root] << termcolor::reset
     << termcolor::bright_cyan << " directory" << termcolor::reset
         << "\tRelative or absolute path to the root directory. Defaults to the current working directory.\n"

     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::playlist] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::playlist] << termcolor::reset
         << "\t\tEnable playlist mode, where files will be shuffled and accessible sequentially.\n"

     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::exclude] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::exclude] << termcolor::reset
     << " [" << termcolor::bright_cyan << "directory1" << termcolor::reset << Args::DELIMITER
             << termcolor::bright_cyan << "directory2" << termcolor::reset << Args::DELIMITER << "..." << Args::DELIMITER
             << termcolor::bright_cyan << "directoryN" << termcolor::reset << "]"
         << "\tEnables directory blacklisting. Directories matching any of the specified paths will be skipped.\n"
     
     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::extensions] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::extensions] << termcolor::reset
     << " [" << termcolor::bright_cyan << "extension1" << termcolor::reset << Args::DELIMITER
             << termcolor::bright_cyan << "extension2" << termcolor::reset << Args::DELIMITER << "..." << Args::DELIMITER
             << termcolor::bright_cyan << "extensionN" << termcolor::reset << "]"
         << "\tEnables file extension whitelisting. Only files that exactly match any of the specified extensions will be taken into account.\n"
     
     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::depth] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::depth] << termcolor::reset
     << termcolor::bright_cyan << " levels" << termcolor::reset
         << "\tMaximum (inclusive) levels of depth the recursive iterator is allowed to reach (min. "
         << termcolor::bright_cyan << FileManager::MIN_DEPTH     << termcolor::reset << ", max. "
         << termcolor::bright_cyan << FileManager::MAX_DEPTH     << termcolor::reset << ", default. "
         << termcolor::bright_cyan << FileManager::DEPTH_DEFAULT << termcolor::reset
         << "). 0 equals to the working directory.\n"
     
     << termcolor::bright_yellow << Args::FLAGS_SHORTENED[Args::nocap] << termcolor::reset << ", " << termcolor::bright_yellow << Args::FLAGS_WHOLE[Args::nocap] << termcolor::reset
         << "\t\tDisable soft caps for storage of relative paths in memory (max. "
         << termcolor::bright_cyan << FileManager::MAX_PATHS << termcolor::reset
         << " paths) and depth levels (max. "
         << termcolor::bright_cyan << FileManager::MAX_DEPTH << termcolor::reset
         << " levels). Caps are enabled by default.\n\n";
}

/**
* @brief main
*/
int main(int argc, char** argv) {
    // Stores the executable name.
    const char* executableName = argv[0];

    // Displays app name and version.
    std::cout << termcolor::bright_green << "\nRandom File Opener v" << VERSION << termcolor::reset << "\n\n";

    // Declares and initializes variables.
    FileManager* fileManager;                      // File manager.
    std::string directoryPathString;               // Path to the root directory as a string.
    std::vector<std::string> forbiddenDirectories; // Directory whitelist.
    std::vector<std::string> allowedExtensions;    // Extension whitelist.
    int depth = FileManager::DEPTH_DEFAULT;        // Maximum depth to iterate to.
    bool areCapsEnabled = true;                    // Whethter soft caps are enabled.
    
    int action = xDefault; // Action to perform.

    // === Parse arguments and perform setup instructions ===
    std::string arg;
    for(int i = 1; i < argc; ++i){
        // Get current argument.
        switch (Args::checkFlag(argv[i])) {
            // Help (show usage).
            case Args::help: {
                action = xHelp;
            } break;
            // Enable playlist mode.
            case Args::playlist: {
                action = xPlaylist;
            } break;
            // Set alternative root directory.
            case Args::root: {
                try{
                    if (++i >= argc) {
                        throw std::invalid_argument("Alternative path for the root directory was enabled, but no path was provided");
                    } else if (!std::filesystem::exists(argv[i])) {
                        throw std::invalid_argument("The provided path for the root directory is not valid.");
                    }
                    directoryPathString = argv[i];
                } catch(const std::exception& ex) {
                    std::cerr << termcolor::bright_red << "ERROR resolving path for the root directory:\n" << ex.what() << termcolor::reset << std::endl;
                    exit(EXIT_FAILURE);
                }
            } break;
            // Enable directory blacklisting and provide matches.
            case Args::exclude: {
                try{
                    if (++i >= argc) {
                        throw std::invalid_argument("Directory blacklisting was enabled, but no directories were provided");
                    }
                
                    // Split string into individual directories.
                    // Include a dot at the beginning for std::filesystem::path::extension compatibility.
                    std::string temp;
                    std::stringstream stringstream {argv[i]};

                    while (std::getline(stringstream, temp, Args::DELIMITER)) {
                        forbiddenDirectories.push_back(temp);
                    }
                } catch (const std::exception& ex) {
                    std::cerr << termcolor::bright_red << "ERROR blacklisting directories:\n" << ex.what() << termcolor::reset << std::endl;
                    exit(EXIT_FAILURE);
                }
            } break;
            // Enable extension whitelisting and provide matches. An empty string counts as an extension.
            case Args::extensions: {
                try{
                    if (++i >= argc) {
                        throw std::invalid_argument("Extension whitelisting was enabled, but no extensions were provided");
                    }
                
                    // Split string into individual extensions.
                    // Include a dot at the beginning for std::filesystem::path::extension compatibility.
                    std::string temp;
                    std::stringstream stringstream {argv[i]};

                    while (std::getline(stringstream, temp, Args::DELIMITER)) {
                        allowedExtensions.push_back(FileManager::EXTENSION_DOT + temp);
                    }
                } catch (const std::exception& ex) {
                    std::cerr << termcolor::bright_red << "ERROR whitelisting extensions:\n" << ex.what() << termcolor::reset << std::endl;
                    exit(EXIT_FAILURE);
                }
            } break;
            // Set maximum depth.
            case Args::depth: {
                try{
                    if (++i >= argc) {
                        throw std::invalid_argument("Alternative recursive depth was enabled, but no value was provided");
                    }
                    depth = std::stoi(argv[i]);
                } catch (const std::exception& ex) {
                    std::cerr << termcolor::bright_red << "ERROR while establishing maximum depth:\n" << ex.what() << termcolor::reset << std::endl;
                    exit(EXIT_FAILURE);
                }
            } break;
            // Disable soft caps.
            case Args::nocap: {
                areCapsEnabled = false;
            } break;

            default: break;
         }
    }

    // === Execute the selected action ===
    switch (action) {
        // Regular actions
        case xDefault:
        case xPlaylist:
            fileManager = buildFileManager(
                directoryPathString,
                forbiddenDirectories,
                allowedExtensions,
                depth,
                areCapsEnabled
            );
            switch (action) {
                case xDefault:  defaultAction(fileManager);  break;
                case xPlaylist: playlistAction(fileManager); break;
                default: break;
            }
            delete fileManager;
        break;
        // Show help
        case xHelp:
            showUsage(executableName);
        break;
        // None
        default: break;
    }

    return EXIT_SUCCESS;
}