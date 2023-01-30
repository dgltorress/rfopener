// FileManager.cpp : descriptions for the file manager

#include "FileManager.h"

/**
* @brief Constructor.
*/
FileManager::FileManager(){
	// Gets the current path.
	scanFileDirectoryPath = std::filesystem::current_path();

	// Initializes the string paths.
	scanFileDirectoryPathString = scanFileDirectoryPath.generic_u8string() + "/"; // Directory where the scan file is located
	scanFilePathString = scanFileDirectoryPathString + scanFileName;			  // Path to the scanned paths file
	scanSizePathString = scanFileDirectoryPathString + scanSizeName;			  // Path to the file containing the amount of paths scanned

	// Checks if the scan file already exists (skips the size file, as they are assumed to go together)
	std::ifstream test( scanFilePathString );
	if( !test ){
		std::cout << termcolor::bright_yellow << "\nThe file " << termcolor::bright_cyan <<
			scanFilePathString << termcolor::bright_yellow << " does not exist and will be created" << termcolor::reset << "\n";
	}

	// Initializes a random seed.
    srand( (unsigned int)time( NULL ) );
}

/**
* @brief Parametrized constructor.
* 
* @param {p_scanFileDirectoryPathString} Absolute or relative path where the target directory is located.
*/
FileManager::FileManager( std::string p_scanFileDirectoryPathString ){
	// Gets the canonical path to the specified directory.
	try{
        scanFileDirectoryPath = std::filesystem::canonical( p_scanFileDirectoryPathString );
    }
    catch( const std::exception& ex ){
        std::cerr << termcolor::bright_red << "Canonical path (" << termcolor::bright_cyan <<
			p_scanFileDirectoryPathString << termcolor::bright_red << ") threw exception:\n" << ex.what() << termcolor::reset << std::endl;
		exit( EXIT_FAILURE );
    }
	
	// Initializes the string paths.
	scanFileDirectoryPathString = scanFileDirectoryPath.generic_u8string() + "/"; // Directory where the scan file is located
	scanFilePathString = scanFileDirectoryPathString + scanFileName;			  // Path to the scanned paths file
	scanSizePathString = scanFileDirectoryPathString + scanSizeName;			  // Path to the file containing the amount of paths scanned

	// Checks if the scan file already exists (skips the size file, as they are assumed to go together)
	std::ifstream test( scanFilePathString );
	if( !test ){
		std::cout << termcolor::bright_yellow << "\nThe file " << termcolor::bright_cyan <<
			scanFilePathString << termcolor::bright_yellow << " does not exist and will be created" << termcolor::reset << "\n";
	}

	// Initializes a random seed.
    srand( (unsigned int)time( NULL ) );
}

/**
* @brief Iterates over every file in the working directory and its subdirectories
* up to the specified depth and stores their paths.
* 
* Soft limit is 20000 files and 8 levels of depth.
* 
* @param {depth} Maximum (inclusive) depth the recursive iterator is allowed to reach. 2 by default (working directory is 0).
*/
void FileManager::storePathsRecursively( int depth ){
	// Validates depth value.
	if( ( depth < minDepth ) || ( depth > maxDepth ) ) depth = depthDefault;

	// Displays relevant directories and files.
	std::cout << "\nWorking directory: " << termcolor::bright_cyan << scanFileDirectoryPathString << termcolor::reset <<
		"\nScan file: " << termcolor::bright_cyan << scanFilePathString << termcolor::reset <<
		"\nSize file: " << termcolor::bright_cyan << scanSizePathString << termcolor::reset <<
		"\n\n====================================================================================\n\n";

	// Initializes counters.
	unsigned int fileCount = 0;
	unsigned int directoryCount = 0;

	try{
		// Opens the scan file in write mode.
		fileWrite.open( scanFilePathString );

		// Writes the canonical path to the working directory into the first line.
		fileWrite << scanFileDirectoryPathString << "\n";
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error while opening scan file:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}

	// Recursively iterates through the working directory.
	for( std::filesystem::recursive_directory_iterator i = std::filesystem::recursive_directory_iterator( scanFileDirectoryPathString );
         i != std::filesystem::recursive_directory_iterator();
         ++i ){
		// Do not list directories.
		if( std::filesystem::is_directory( i->path() ) ){
			// Do not go deeper than specified.
			if( i.depth() >= depth ) {
				i.disable_recursion_pending();
			}
			// Count directory.
			++directoryCount;
		}
		// Do list files.
		else{
			// Executable and scan files do not get stored.
			if( i.depth() == 0 ){
				std::string fileName = i->path().filename().generic_u8string();
				if ( fileName == exeFileName || 
					 fileName == scanFileName || 
					 fileName == scanSizeName ) {
					continue;
				}
			}

			try {
				// Stores the file path, relative to the working directory, in UTF8 format.
				fileWrite << std::filesystem::relative( i->path() , scanFileDirectoryPath ).generic_u8string() << "\n";
			} catch( const std::exception& ex ){
			    std::cerr << termcolor::bright_red << "Error while writing file path:\n" << ex.what() << termcolor::reset << "\n";
				exit( EXIT_FAILURE );
			}
			// Count file.
			++fileCount;
		}
	}

	try {
		// Closes the scan file.
		fileWrite.close();

		// Stores the amount of paths into another file.
		// (grants constant complexity that helps reserve vector slots in memory mode and randomize a line in direct read mode).
		fileWrite.open( scanSizePathString );
		fileWrite << fileCount;
		fileWrite.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error while writing:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}

	// Displays final counts.
	std::cout << "\n"
		<< termcolor::bright_cyan << fileCount      << termcolor::reset << " files, "
		<< termcolor::bright_cyan << directoryCount << termcolor::reset << " subdirectories scanned" << "\n";
}

/**
* @brief Reads all paths into memory (brute force approach).
* 
* @param {paths} Path vector (up to 20000 elements will be stored).
*/
void FileManager::readPaths( std::vector<std::string>& paths ){
	try {
		// Opens the size file in read mode.
		fileRead.open( scanSizePathString );
		
		// Reads the size into an integer.
		std::string line;
		int numberOfPaths = 0;
		if( std::getline( fileRead , line ) ) {
			numberOfPaths = std::stoi( line );
		}

		// Closes the size file.
		fileRead.close();

		// Validates the number of paths.
		if( numberOfPaths < 1 ){
			std::cerr << termcolor::bright_red << "Invalid path amount\n" << termcolor::reset << "\n";
			exit( EXIT_FAILURE );
		}

		// Reserves vector space.
		paths.reserve( ( numberOfPaths < maxPaths ) ? numberOfPaths : maxPaths );


		// Declares variables and counters.
		unsigned int i = 0;
		unsigned long byteCount = 0;
		size_t vectorSize = 0;

		// Opens the size file in read mode.
		fileRead.open( scanFilePathString );

		// Read paths into the passed vector.
		while( ( i++ < maxPaths ) &&
			( std::getline( fileRead,line ) ) ){
			paths.emplace_back( line );
			byteCount += (unsigned long)line.size();
		}

		// Calculates and displays the size of reserved variables.
		vectorSize = sizeof( std::vector<std::string> ) + ( sizeof( std::string ) * paths.size() );

		std::cout << "\nSize of vector: "
			<< termcolor::bright_cyan << vectorSize << " bytes" << termcolor::reset << " (" << termcolor::bright_cyan << paths.size() << " elements" << termcolor::reset << ")\n";
		std::cout << "Combined size of dynamically allocated strings: "
			<< termcolor::bright_cyan << byteCount << " bytes" << termcolor::reset << "\n\n";

		std::cout << "TOTAL ALLOCATED MEMORY: "
			<< termcolor::bright_cyan << vectorSize + byteCount << " bytes" << termcolor::reset
			<< "\n\n====================================================================================\n\n";

		// Closes the scan file.
		fileRead.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error opening file from stored path:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
}

/**
* @brief Directly reads a random path from a file (read every time approach).
* Paths are expected to be separated by newline characters.
*/
void FileManager::openRandomFilePath(){
	try {
		// Opens the size file in read mode.
		fileRead.open( scanSizePathString );
		
		// Reads the size into an integer.
		std::string line;
		int numberOfPaths = 0;
		if ( std::getline( fileRead,line ) ) {
			numberOfPaths = std::stoi( line );
		}

		// Closes the size file.
		fileRead.close();

		// Validates the number of paths.
		if( numberOfPaths < 1 ){
			std::cerr << termcolor::bright_red << "Invalid path amount\n" << termcolor::reset << "\n";
			exit( EXIT_FAILURE );
		}

		// Picks a random line.
		int pathNo = rand() % numberOfPaths; // integer in the range 0 to the amount of paths


		// Opens the scan file in read mode.
		fileRead.open( scanFilePathString );

		// Moves the iterator to the specified line.
		for( int i = 0 ; i < pathNo + 1 ; ++i ){
		    fileRead.ignore( std::numeric_limits<std::streamsize>::max() , '\n' );
		}

		// Gets the line.
		if( std::getline( fileRead , line ) ){
			// Constructs the path.
			std::string filePath = scanFileDirectoryPathString + line;
			std::wstring wideFilePath = FileManager::utf8ToWide( filePath );
			
			// Displays the path.
			std::cout << "Opening " << termcolor::bright_cyan << filePath << termcolor::reset << " ...\n";
			
			// Tries to open the file corresponding to the path.
			ShellExecuteW( 0 , 0 , wideFilePath.c_str() , 0 , 0 , SW_SHOW );
		}

		// Closes the scan file.
		fileRead.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error opening file from stored path:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
}

/**
* @brief Converts a UTF8 string into a wide string.
*/
std::wstring FileManager::utf8ToWide( const std::string& str ){
    int count = MultiByteToWideChar( CP_UTF8 , 0 , str.c_str() , (int)str.length() , NULL , 0 );
    std::wstring wstr( count , 0 );
    MultiByteToWideChar( CP_UTF8 , 0 , str.c_str() , (int)str.length() , &wstr[0] , count );
    return wstr;
}

/**
* @brief Working directory getter.
*/
std::string FileManager::getWorkingDirectory(){ return scanFileDirectoryPathString; }