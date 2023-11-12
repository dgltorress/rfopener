// FileManager.h : declarations for the file manager

#pragma once

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <iostream>    // console IO
#include <string>      // strings
#include <vector>      // dynamic containers
#include <windows.h>   // Windows API functions
#include <algorithm>   // shuffle, find
#include <random>      // default_random_engine
#include <chrono>      // chrono, system_clock

#include <filesystem>  // file navigation. C++17 ONLY.

#include "termcolor.h" // easy console colors, available at https://github.com/ikalnytskyi/termcolor

#undef max // undefine any macros for max(), such as Visual Studio's 

class FileManager {

    private:
        // Directory paths.
        std::filesystem::path rootDirectory;
        std::string rootDirectoryString;
        std::vector<std::filesystem::path> directoryBlacklist;

        // Extensions.
        std::vector<std::string> extensionWhitelist;

        // Path string vector.
        std::vector<std::string> relativePathStrings;

        // Shuffle index.
        int shuffleIndex;

        // Other
        static constexpr const char* EXTENSION_SEPARATOR = ", ";
        static constexpr const char* CONSOLE_LINE = "\n\n====================================================================================\n\n";

        // Random.
        std::default_random_engine randomEngine;
        std::uniform_int_distribution<> distribution;

        // == Main functions ==
        /**
        * @brief Set up centralization for constructors.
        */
        void setUp(const std::string&);
        /**
        * @brief Sets the working directory.
        * 
        * @param unprocessedDirectoryPathString Absolute or relative path where the target directory is located.
        * @return If an error was thrown, returns `false`.
        */
        bool setWorkingDirectory(const std::string&);

        // == Other functions ==
        /**
        * @brief Converts a UTF8 string into a wide string.
        * 
        * @param utf8str UTF8-encoded string.
        * @return Wide string.
        */
        static std::wstring utf8ToWide(const std::string&);
        /**
        * @brief Adjusts a depth value.
        * 
        * @param depth Initial depth.
        * @param checkCaps Whether to check for the soft cap for the levels of depth while adjusting.
        */
        int adjustDepth(const int, const bool = true);
        /**
        * @brief Adjusts the shuffle index.
        * 
        * @param backwards Whether to go backwards. Defaults to false.
        */
        void adjustShuffleIndex(const bool = false);
        /**
        * @brief Parses absolute or relative directory paths.
        * 
        * @param forbiddenDirectories List of blacklisted directories as absolute or relative path strings.
        */
        void parseBlacklistedDirectories(const std::vector<std::string>&);
        /**
        * @brief Parses extension names.
        * 
        * @param allowedExtensions List of allowed extensions.
        */
        void parseWhitelistedExtensions(const std::vector<std::string>&);
        /**
        * @brief Determines whether a directory is blacklisted.
        * 
        * @param directory Directory.
        */
        bool isDirectoryBlacklisted(const std::filesystem::path&);
        /**
        * @brief Determines whether an extension is whitelisted.
        * 
        * @param extension Extension.
        */
        bool isExtensionWhitelisted(const std::string&);

        // == Display functions ==
        /**
        * @brief Displays the working directory and the selected depth cap.
        * 
        * @param depth Depth cap for this instance.
        */
        void displayBasicInfo(const int) const;
        /**
        * @brief Displays a line containing a friendly list of extensions.
        */
        void displayExtensionWhitelist() const;
        /**
        * @brief Displays all blacklisted directories.
        */
        void displayDirectoryBlacklist() const;
        /**
        * @brief Displays a warning notifying that a cap has been reached.
        * 
        * @param capName Name of the cap.
        * @param cap Amount reached.
        */
        void displayCapWarning(const char*, const int) const;
        /**
        * @brief Displays the amount of scanned files and distinct directories.
        * 
        * @param fileCount File count.
        * @param directoryCount Directory count.
        */
        void displayFileCounts(const unsigned int, const unsigned int) const;
        /**
        * @brief Displays the current playlist index and amount of elements.
        */
        void displayPlaylistInfo() const;

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
        * @brief Shuffles read paths.
        */
        void shuffle();
        /**
        * @brief Executes a file.
        * 
        * @param relativePath The unprocessed relative path as a string.
        */
        void executeFile(const std::string&) const;
        /**
        * @brief Executes a random file.
        */
        void executeRandomFile();
        /**
        * @brief Executes the following or previous shuffled file.
        * 
        * @param backwards Whether to go backwards. Defaults to false.
        */
        void executeSequentialFile(const bool = false);
        /**
        * @brief Executes the first file.
        */
        void executeFirstFile();
        /**
        * @brief Prints a line surrounded by double newline characters.
        */
        static void printLine();
};

#endif