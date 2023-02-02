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

        // Directory path.
        std::filesystem::path directoryPath;
        std::string directoryPathString;     // as string

        // Path string vector.
        std::vector<std::string> relativePathStrings;

    public:
        // Given names for file.
        const char* exeFileName = "rfopener.exe"; // Executable

        // Soft limits and default values
        static const int maxPaths = 50000; // Maximum (inclusive) amount of paths allowed to be stored in a single file
        static const int minDepth = 0;     // Minimum (inclusive) depth
        static const int maxDepth = 10;    // Maximum (inclusive) depth the recursive iterator is allowed to reach
        static const int depthDefault = 5; // Default depth the recursive iterator is allowed to reach

        // Constructors
        FileManager();               // Default (current directory)
        FileManager( std::string& ); // Parametrized

        // Main functions
        bool setWorkingDirectory( std::string& );                        // Sets the working directory
        void readPaths( std::vector<std::string>& , int , bool = true ); // Reads paths by iterating recursively and stores them in a vector
        void executeRandomFile();                                        // Executes a file corresponding to a random stored path.

        // Other functions
        static std::wstring utf8ToWide( const std::string& ); // Converts a UTF8 string into a wide string
        void executeFile( std::string , bool = false );       // Executes a file
        int adjustDepth( int , bool = true );                 // Adjusts a depth value

        void displayFileCounts( unsigned int , unsigned int ); // Displays the amount of scanned files and distinct directories
        void displayAllocatedMemory( size_t , unsigned long ); // Displays the total allocated memory for the strings in bytes
};

#endif