// FileManager.cpp : descriptions for the file manager

#include "FileManager.h"

/**
* @brief Constructor.
*/
FileManager::FileManager(){
	// Set the working directory.
	std::string emptyString;
	if( setWorkingDirectory( emptyString ) == false ) exit( EXIT_FAILURE );

	// Initializes a random seed.
    srand( (unsigned int)time( NULL ) );
}

/**
* @brief Parametrized constructor.
* 
* @param {unprocessedDirectoryPathString} Absolute or relative path where the target directory is located.
*/
FileManager::FileManager( std::string& unprocessedDirectoryPathString ){
	// Gets the canonical path to the specified directory.
	if( setWorkingDirectory( unprocessedDirectoryPathString ) == false ) exit( EXIT_FAILURE );

	// Initializes a random seed.
    srand( (unsigned int)time( NULL ) );
}

/**
* @brief Sets the working directory.
* 
* @param {unprocessedDirectoryPathString} Absolute or relative path where the target directory is located.
*/
bool FileManager::setWorkingDirectory( std::string& unprocessedDirectoryPathString ){
	try{
		// Sets the working directory as the current path if no directory was provided, and the canonical path if it was.
        directoryPath = ( unprocessedDirectoryPathString.empty() ) ? std::filesystem::current_path() :
			                                                         std::filesystem::canonical( unprocessedDirectoryPathString ) ;
    }
    catch( const std::exception& ex ){
		std::cerr << termcolor::bright_red << "Resolving path \"" << termcolor::bright_cyan <<
			unprocessedDirectoryPathString << termcolor::bright_red << "\" threw exception:\n" << ex.what() << termcolor::reset << std::endl;
        return false;
    }
	
	// Initializes the string path.
	directoryPathString = directoryPath.generic_u8string() + "/";

	return true;
}

/**
* @brief Reads paths by iterating recursively and stores them into memory.
* 
* @param {extensions} Extension whitelist that will be checked for every file. If empty, no checks will be performed.
* @param {depth} Maximum depth that the recursive iterator is allowed to reach. Will be validated beforehand.
* @param {checkCaps} Whether to enable soft caps. Defaults to true.
*/
void FileManager::readPaths( std::vector<std::string>& extensions , int depth , bool checkCaps ){
	// Clears the vector.
	relativePathStrings.clear();

	// Determines whether an extension whitelist must be checked.
	bool isExtensionWhitelistEnabled = ( extensions.size() > 0 );

	// Validates (and adjusts, if necessary) depth value.
	depth = adjustDepth( depth );

	// Displays relevant directories and files.
	std::cout << "\nWorking directory: " << termcolor::bright_cyan << directoryPathString << termcolor::reset
		<< "\nDepth: " << termcolor::bright_cyan << depth << termcolor::reset;

	if( isExtensionWhitelistEnabled == true ){
		std::cout << "\nExtension whitelist: " << termcolor::bright_cyan;
		int i = 0;
		while( i < extensions.size() ){
			std::cout << extensions[ i ];
			if( ++i < extensions.size() ) std::cout << termcolor::reset << ", " << termcolor::bright_cyan;
		}
		std::cout << termcolor::reset;
	}

	std::cout << "\n\n====================================================================================\n\n";

	// Initializes counters.
	unsigned int fileCount = 0;
	unsigned int directoryCount = 0;

	unsigned long byteCount = 0;
	size_t vectorSize = 0;

	try{
		// Recursively iterates through the working directory.
		for( std::filesystem::recursive_directory_iterator i = std::filesystem::recursive_directory_iterator( directoryPath );
		     i != std::filesystem::recursive_directory_iterator();
		     ++i ){
			// If not disabled, check if path storage limit has been exceeded.
			if( ( checkCaps == true ) &&
				( fileCount >= maxPaths ) ){
				std::cout << termcolor::bright_yellow << "\n\nSoft cap for file paths ("
					<< termcolor::bright_cyan << maxPaths << termcolor::bright_yellow
					<< ") has been reached. Disable cap checking with flag -nc or --nocap" << termcolor::reset << "\n\n";
				break;
			}

			// Get the path of the current file.
			std::filesystem::path path = i->path();

			// Get the current depth.
			int currentDepth = i.depth();

			// Do not list directories.
			if( std::filesystem::is_directory( path ) ){
				// Do not go deeper than specified.
				if( currentDepth >= depth ) {
					i.disable_recursion_pending();
				}
				// Count directory.
				++directoryCount;
			}
			// Do list files.
			else{
				// Ignore if it the path points to this executable.
				if( ( currentDepth == 0 ) &&
					( path.filename().generic_u8string() == exeFileName ) ){
					continue;
				}

				// Ignore if extension whitelist is enabled and the current file's extension does not match any.
				if( isExtensionWhitelistEnabled == true ){
					size_t j = 0;
					for( j ; j < extensions.size() ; ++j ){
						if( path.extension().generic_u8string() == extensions[ j ] ) break;
					}

					if( j == extensions.size() ) continue;
				}

				// Reads the file path, relative to the working directory, in UTF8 format.
				std::string relativePathString = std::filesystem::relative( path , directoryPath ).generic_u8string();
				relativePathStrings.emplace_back( relativePathString );

				// Count file and path string bytes.
				++fileCount;
				byteCount += (unsigned long)relativePathString.size();
			}
		}
	} catch( const std::exception& ex ){
	    std::cerr << termcolor::bright_red << "ERROR while reading paths into memory:\n" << ex.what() << termcolor::reset << "\n";
		exit( EXIT_FAILURE );
	}
	
	// Calculates and displays the size of path strings in memory.
	vectorSize = sizeof( std::vector<std::string> ) + ( sizeof( std::string ) * relativePathStrings.size() );
	displayAllocatedMemory( vectorSize , byteCount );

	// Displays final file and directory counts.
	displayFileCounts( fileCount , directoryCount );
}

/**
* @brief Executes a random file.
*/
void FileManager::executeRandomFile(){
	if( relativePathStrings.size() > 0 ){
		executeFile( relativePathStrings[ rand() % relativePathStrings.size() ] );
	}
	else{
		std::cerr << termcolor::bright_red << "No paths have been stored into memory\n" << termcolor::reset;
	}
}

/**
* @brief Executes a file.
* 
* @param {unprocessedPathString} The unprocessed relative or canonical path as a string.
* @param {canonical} Whether the provided path is canonical (defaults to false - paths are assumed relative).
*/
void FileManager::executeFile( std::string unprocessedPathString , bool canonical ){
	// Constructs the path.
	std::string pathString;
	if( canonical == true ) pathString = unprocessedPathString;
	else                    pathString = directoryPathString + unprocessedPathString;
		 
	std::wstring wideFilePath = FileManager::utf8ToWide( pathString );
	
	// Displays the path.
	std::cout << "Opening " << termcolor::bright_cyan << pathString << termcolor::reset << " ...\n";
	
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
* @param {checkCaps} Whether to check for the soft cap for the levels of depth while adjusting.
*/
int FileManager::adjustDepth( int depth , bool checkCaps ){
	if( ( depth < minDepth ) ){
		return depthDefault;
	}
	else if( ( checkCaps == true ) &&
		     ( depth > maxDepth ) ){
		std::cout << termcolor::bright_yellow << "\n\nSoft cap for depth ("
			<< termcolor::bright_cyan << maxDepth << termcolor::bright_yellow
			<< ") has been reached. Disable cap checking with flag -nc or --nocap" << termcolor::reset << "\n\n";

		return maxDepth;
	}
	else return depth;
}

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
* @param {vectorSize} Vector size in bytes.
* @param {byteCount} Bytes of allocated strings.
*/
void FileManager::displayAllocatedMemory( size_t vectorSize , unsigned long byteCount ){
	std::cout << "\nSize of vector: "
		<< termcolor::bright_cyan << vectorSize << " bytes" << termcolor::reset << " (" << termcolor::bright_cyan << relativePathStrings.size() << " elements" << termcolor::reset << ")\n";
	std::cout << "Combined size of dynamically allocated strings: "
		<< termcolor::bright_cyan << byteCount << " bytes" << termcolor::reset << "\n\n";

	std::cout << "TOTAL ALLOCATED MEMORY: "
		<< termcolor::bright_cyan << vectorSize + byteCount << " bytes" << termcolor::reset
		<< "\n\n====================================================================================\n\n";
}
