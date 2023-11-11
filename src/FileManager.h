// FileManager.h : declarations for the file manager

#pragma once

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <iostream>    // console IO
#include <string>      // strings
#include <vector>      // dynamic containers
#include <windows.h>   // Windows API functions
#include <limits>      // numeric limits interface

#include <fstream>     // file manipulation
#include <filesystem>  // file navigation. C++17 ONLY.

#include <stdlib.h>    // srand, rand
#include <time.h>      // time

#include "termcolor.h" // easy console colors, available at https://github.com/ikalnytskyi/termcolor

#undef max // undefine any macros for max(), such as Visual Studio's 

class FileManager {

    private:
        // Streams
        std::ifstream fileRead;  // Read
        std::ofstream fileWrite; // Write

        // Directory path.
        std::filesystem::path rootDirectory;
        std::string rootDirectoryString;

        // Path string vector.
        std::vector<std::string> relativePathStrings;

        // Other
        static constexpr const char* EXTENSION_SEPARATOR = ", ";
        static constexpr const char* CONSOLE_LINE = "\n\n====================================================================================\n\n";

        // == Main functions ==
        /**
        * @brief Set up centralization for constructors.
        */
        void setUp(std::string&);
        /**
        * @brief Sets the working directory.
        * 
        * @param unprocessedDirectoryPathString Absolute or relative path where the target directory is located.
        * @return If an error was thrown, returns `false`.
        */
        bool setWorkingDirectory(std::string&);

        // == Other functions ==
        /**
        * @brief Converts a UTF8 string into a wide string.
        * 
        * @param utf8str UTF8-encoded string.
        * @return Wide string.
        */
        static std::wstring utf8ToWide(const std::string&);
        /**
        * @brief Executes a file.
        * 
        * @param unprocessedPathString The unprocessed relative or canonical path as a string.
        * @param canonical Whether the provided path is canonical (defaults to `false` - paths are assumed relative to the root directory).
        */
        void executeFile(std::string, bool = false);
        /**
        * @brief Adjusts a depth value.
        * 
        * @param depth Initial depth.
        * @param checkCaps Whether to check for the soft cap for the levels of depth while adjusting.
        */
        int adjustDepth(int, bool = true);

        // == Display functions ==
        /**
        * @brief Displays the working directory and the selected depth cap.
        * 
        * @param depth Depth cap for this instance.
        */
        void displayBasicInfo(int);
        /**
        * @brief Displays a line containing a friendly list of extensions.
        * 
        * @param extensions Extension vector.
        */
        void displayExtensionWhitelist(std::vector<std::string>&);
        /**
        * @brief Displays a warning notifying that a cap has been reached.
        * 
        * @param capName Name of the cap.
        * @param cap Amount reached.
        */
        void displayCapWarning(const char*, int);
        /**
        * @brief Displays the amount of scanned files and distinct directories.
        * 
        * @param fileCount File count.
        * @param directoryCount Directory count.
        */
        void displayFileCounts(unsigned int, unsigned int);

    public:
        // Soft limits and default values
        static const int MAX_PATHS = 50000; // Maximum (inclusive) amount of paths allowed to be stored in a single file
        static const int MIN_DEPTH = 0;     // Minimum (inclusive) depth
        static const int MAX_DEPTH = 10;    // Maximum (inclusive) depth the recursive iterator is allowed to reach
        static const int DEPTH_DEFAULT = 5; // Default depth the recursive iterator is allowed to reach

        static const char EXTENSION_DOT = '.';

        // == Constructor ==
        FileManager();
        /**
        * @param unprocessedDirectoryPathString Absolute or relative path of the root directory.
        */
        FileManager(std::string&);
        ~FileManager();

        /**
        * @brief Reads paths by iterating recursively and stores them into memory.
        * 
        * @param forbiddenDirectories Directory blacklist.
        * @param allowedExtensions Extension whitelist. If empty, no checks will be performed.
        * @param depth Maximum depth that the recursive iterator is allowed to reach.
        * @param checkCaps Whether to enable soft caps. Defaults to true.
        */
        void readPaths(
            std::vector<std::string>&,
            std::vector<std::string>&,
            int,
            bool = true
        );
        /**
        * @brief Executes a random file.
        */
        void executeRandomFile();
        /**
        * @brief Prints a line surrounded by double newline characters.
        */
        static void printLine();
};

#endif