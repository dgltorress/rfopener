// FileManager.h : declarations for the file manager

#pragma once

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <iostream>     /* console IO */
#include <string>       /* strings */
#include <vector>       /* dynamic containers */
#include <windows.h>    /* Windows API functions */
#include <limits>       /* numeric limits interface */

#include <fstream>      /* file manipulation */
#include <filesystem>   /* file navigation. C++17 ONLY. */

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "termcolor.h"  /* easy console colors, available at https://github.com/ikalnytskyi/termcolor */

#undef max              /* undefine any macros for max(), such as Visual Studio's */

class FileManager{

    private:
        // Streams
        std::ifstream fileRead;  // Read
        std::ofstream fileWrite; // Write

        // Paths
        std::filesystem::path scanFileDirectoryPath; // Directory where the scan file is located
        std::filesystem::path scanFilePath;          // Path to the scanned paths file
        std::filesystem::path scanSizePath;          // Path to the file containing the amount of paths scanned
        
        // Paths as strings
        std::string scanFileDirectoryPathString;     // Directory where the scan file is located
        std::string scanFilePathString;              // Path to the scanned paths file
        std::string scanSizePathString;              // Path to the file containing the amount of paths scanned

        // Given names for files
        const char* exeFileName = "rfopener.exe";       // Executable
        const char* scanFileName = "rfopener_scan.txt"; // Scanned paths file
        const char* scanSizeName = "rfopener_size.txt"; // File containing the amount of paths scanned

    public:
        // Soft limits and default values
        static const unsigned int maxPaths = 50000; // Maximum (inclusive) amount of paths allowed to be stored in a single file
        static const unsigned int minDepth = 0;     // Minimum (inclusive) depth
        static const unsigned int maxDepth = 8;     // Maximum (inclusive) depth the recursive iterator is allowed to reach
        static const unsigned int depthDefault = 2; // Default depth the recursive iterator is allowed to reach

        // Constructors
        FileManager();              // Default
        FileManager( std::string ); // Parametrized

        // Main functions
        void readPathsRecursively( std::vector<std::string>& , int ); // Reads paths by iterating recursively and stores them in a vector
        void readPathsFromFile( std::vector<std::string>& );          // Reads paths from a file and stores them in a vector

        void storePathsRecursively( int = depthDefault );       // Finds and stores file paths
        void openRandomFilePath();                              // Opens the path on a random line on a file
        int getSizeFromFile();                                  // Gets the amount of stored paths from the size file

        // Other functions
        static std::wstring utf8ToWide( const std::string& ); // Converts a UTF8 string into a wide string
        void executeFile( std::string , bool = false );       // Executes a file
        int adjustDepth( int );                               // Adjusts a depth value

        void displayFileCounts( unsigned int , unsigned int );                              // Displays the amount of scanned files and distinct directories
        void displayAllocatedMemory(  std::vector<std::string>& , size_t , unsigned long ); // Displays the total allocated memory
};

#endif