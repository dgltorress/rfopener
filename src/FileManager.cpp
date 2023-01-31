// FileManager.cpp : descriptions for the file manager

#include "FileManager.h"

/**
* @brief Constructor.
*/
FileManager::FileManager(){
	// Gets the current path.
	try{
        scanFileDirectoryPath = std::filesystem::current_path();
    }
    catch( const std::exception& ex ){
        std::cerr << termcolor::bright_red << "Current path path threw exception:\n" << ex.what() << termcolor::reset << std::endl;
		exit( EXIT_FAILURE );
    }

	// Initializes the string paths.
	scanFileDirectoryPathString = scanFileDirectoryPath.generic_u8string() + "/"; // Directory where the scan file is located
	scanFilePathString = scanFileDirectoryPathString + scanFileName;			  // Path to the scanned paths file
	scanSizePathString = scanFileDirectoryPathString + scanSizeName;			  // Path to the file containing the amount of paths scanned

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

	// Initializes a random seed.
    srand( (unsigned int)time( NULL ) );
}

/**
* @brief Reads paths by iterating recursively and stores them into memory.
* 
* @param {paths} Path vector (up to `maxPaths` elements will be stored).
*/
void FileManager::readPathsRecursively( std::vector<std::string>& paths , int depth ){
	// Validates (and adjusts, if necessary) depth value.
	depth = adjustDepth( depth );

	// Displays relevant directories and files.
	std::cout << "\nWorking directory: " << termcolor::bright_cyan << scanFileDirectoryPathString << termcolor::reset <<
		"\nDepth: " << termcolor::bright_cyan << depth << termcolor::reset <<
		"\n\n====================================================================================\n\n";

	// Initializes counters.
	unsigned int fileCount = 0;
	unsigned int directoryCount = 0;

	unsigned long byteCount = 0;
	size_t vectorSize = 0;

	// Stores the working directory.
	paths.emplace_back( scanFileDirectoryPathString );

	try{
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
				// Ignore this executable.
				if( i.depth() == 0 ){
					std::string fileName = i->path().filename().generic_u8string();
					if ( fileName == exeFileName ) {
						continue;
					}
				}

				// Reads the file path, relative to the working directory, in UTF8 format.
				std::string relativePath = std::filesystem::relative( i->path() , scanFileDirectoryPath ).generic_u8string();
				paths.emplace_back( relativePath );

				// Count file and path string bytes.
				++fileCount;
				byteCount += (unsigned long)relativePath.size();
			}
		}
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "ERROR while storing paths:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
	
	// Calculates and displays the size of reserved variables.
	vectorSize = sizeof( std::vector<std::string> ) + ( sizeof( std::string ) * paths.size() );
	displayAllocatedMemory( paths , vectorSize , byteCount );

	// Displays final counts.
	displayFileCounts( fileCount , directoryCount );
}

/**
* @brief Reads all paths from a file and stores them into memory.
* 
* @param {paths} Path vector (up to `maxPaths` elements will be stored).
*/
void FileManager::readPathsFromFile( std::vector<std::string>& paths ){
	try {
		// Gets the amount of stored paths.
		int numberOfPaths = getSizeFromFile();

		// Validates the number of paths.
		if( numberOfPaths == -1 ){
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
		std::string line;
		while( ( i++ < maxPaths ) &&
			( std::getline( fileRead,line ) ) ){
			paths.emplace_back( line );
			byteCount += (unsigned long)line.size();
		}

		// Calculates and displays the size of reserved variables.
		vectorSize = sizeof( std::vector<std::string> ) + ( sizeof( std::string ) * paths.size() );
		displayAllocatedMemory( paths , vectorSize , byteCount );

		// Closes the scan file.
		fileRead.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error opening file from stored path:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
}

/**
* @brief Iterates over every file in the working directory and its subdirectories
* up to the specified depth and stores their paths.
* 
* Soft limit is `maxPaths` files and `maxDepth` levels of depth.
* 
* @param {depth} Maximum (inclusive) depth the recursive iterator is allowed to reach. 2 by default (working directory is 0).
*/
void FileManager::storePathsRecursively( int depth ){
	// Validates (and adjusts, if necessary) depth value.
	depth = adjustDepth( depth );

	// Displays relevant directories and files.
	std::cout << "\nWorking directory: " << termcolor::bright_cyan << scanFileDirectoryPathString << termcolor::reset <<
		"\nScan file: " << termcolor::bright_cyan << scanFilePathString << termcolor::reset <<
		"\nSize file: " << termcolor::bright_cyan << scanSizePathString << termcolor::reset <<
		"\nDepth: " << termcolor::bright_cyan << depth << termcolor::reset <<
		"\n\n====================================================================================\n\n";

	// Initializes counters.
	unsigned int fileCount = 0;
	unsigned int directoryCount = 0;

	try{
		// Checks if the scan file already exists (skips the size file, as they are assumed to go together)
		std::ifstream test( scanFilePathString );
		if( !test ){
			std::cout << termcolor::bright_yellow << "\nThe file " << termcolor::bright_cyan <<
				scanFilePathString << termcolor::bright_yellow << " does not exist and will be created" << termcolor::reset << "\n";
		}

		// Opens the scan file in write mode.
		fileWrite.open( scanFilePathString );

		// Writes the canonical path to the working directory into the first line.
		fileWrite << scanFileDirectoryPathString << "\n";

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
				// Executable (if not on PATH variable) and scan files do not get stored.
				if( i.depth() == 0 ){
					std::string fileName = i->path().filename().generic_u8string();
					if ( fileName == exeFileName || 
						 fileName == scanFileName || 
						 fileName == scanSizeName ) {
						continue;
					}
				}

				// Stores the file path, relative to the working directory, in UTF8 format.
				fileWrite << std::filesystem::relative( i->path() , scanFileDirectoryPath ).generic_u8string() << "\n";

				// Count file.
				++fileCount;
			}
		}
		// Closes the scan file.
		fileWrite.close();

		// Stores the amount of paths into another file.
		// (grants constant complexity that helps reserve vector slots in memory mode and randomize a line in direct read mode).
		fileWrite.open( scanSizePathString );
		fileWrite << fileCount;
		fileWrite.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "ERROR while storing paths:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}

	// Displays final counts.
	displayFileCounts( fileCount , directoryCount );
}

/**
* @brief Directly reads a random path from a file (read every time approach).
* Paths are expected to be separated by newline characters.
*/
void FileManager::openRandomFilePath(){
	try {
		// Gets the amount of stored paths.
		int numberOfPaths = getSizeFromFile();

		// Validates the number of paths.
		if( numberOfPaths == -1 ){
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
		
		// Gets the line and executes de file.
		std::string line;
		if( std::getline( fileRead , line ) ) executeFile( line );

		// Closes the scan file.
		fileRead.close();
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "Error opening file from stored path:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
}

/**
* @brief Gets the amount of stored paths from the size file.
*/
int FileManager::getSizeFromFile() {
	int numberOfPaths = -1;

	try {
		// Opens the size file in read mode.
		fileRead.open( scanSizePathString );
		
		// Reads the size into an integer.
		std::string line;
		
		if ( std::getline( fileRead,line ) ) {
			numberOfPaths = std::stoi( line );
		}

		// Closes the size file.
		fileRead.close();

		// Validates the number of paths.
		if( numberOfPaths < 1 ){
			throw std::invalid_argument( "Invalid path amount" );
		}
	} catch( const std::exception& ex ){
		std::cerr << termcolor::bright_red << "ERROR:\n" << ex.what() << termcolor::reset << "\n";
		numberOfPaths = -1;
	}

	return numberOfPaths;
}

/**
* @brief Executes a file.
* 
* @param {canonical} Whether the provided path is canonical (defaults to false - paths are assumed relative).
*/
void FileManager::executeFile( std::string pathString , bool canonical ){
	// Constructs the path.
	std::string filePath;
	if( canonical == true ) filePath = pathString;
	else                    filePath = scanFileDirectoryPathString + pathString;
		 
	std::wstring wideFilePath = FileManager::utf8ToWide( filePath );
	
	// Displays the path.
	std::cout << "Opening " << termcolor::bright_cyan << filePath << termcolor::reset << " ...\n";
	
	// Tries to open the file corresponding to the path.
	ShellExecuteW( 0 , 0 , wideFilePath.c_str() , 0 , 0 , SW_SHOW );
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
* @brief Adjusts a depth value.
* 
* @param {depth} Initial depth.
*/
int FileManager::adjustDepth( int depth ){
	if( ( depth < minDepth ) || ( depth > maxDepth ) ) return depthDefault;
	else                                               return depth;
};

/**
* @brief Displays the amount of scanned files and distinct directories.
* 
* @param {fileCount} File count.
* @param {directoryCount} Directory count.
*/
void FileManager::displayFileCounts( unsigned int fileCount , unsigned int directoryCount ){
	std::cout << "\n"
		<< termcolor::bright_cyan << fileCount      << termcolor::reset << " files, "
		<< termcolor::bright_cyan << directoryCount << termcolor::reset << " subdirectories scanned" << "\n\n";
}

/**
* @brief Displays the total allocated memory.
* 
* @param {paths} Relative path vector.
* @param {vectorSize} Vector size in bytes.
* @param {byteCount} Bytes of allocated strings.
*/
void FileManager::displayAllocatedMemory( std::vector<std::string>& paths , size_t vectorSize , unsigned long byteCount ){
	std::cout << "\nSize of vector: "
		<< termcolor::bright_cyan << vectorSize << " bytes" << termcolor::reset << " (" << termcolor::bright_cyan << paths.size() << " elements" << termcolor::reset << ")\n";
	std::cout << "Combined size of dynamically allocated strings: "
		<< termcolor::bright_cyan << byteCount << " bytes" << termcolor::reset << "\n\n";

	std::cout << "TOTAL ALLOCATED MEMORY: "
		<< termcolor::bright_cyan << vectorSize + byteCount << " bytes" << termcolor::reset
		<< "\n\n====================================================================================\n\n";
}
