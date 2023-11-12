// FileManager.cpp : descriptions for the file manager

#include "FileManager.h"
#include "Args.h"

FileManager::FileManager(){
	std::string emptyString;
	setUp(emptyString);
}

FileManager::FileManager(std::string& unprocessedDirectoryPathString) {
	setUp(unprocessedDirectoryPathString);
}

FileManager::~FileManager() {
	rootDirectory.clear();
	rootDirectoryString.clear();
	directoryBlacklist.clear();

	extensionWhitelist.clear();

    relativePathStrings.clear();
}

void FileManager::setUp(const std::string& unprocessedDirectoryPathString) {
	// Gets the canonical path to the specified directory.
	if (!setWorkingDirectory(unprocessedDirectoryPathString)) {
		exit(EXIT_FAILURE);
	}

	// Sets the shuffle index to 0
	shuffleIndex = 0;

	// Initializes a random seed.
	randomEngine.seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
}

bool FileManager::setWorkingDirectory(const std::string& unprocessedDirectoryPath) {
	try {
		// Sets the working directory as the current path if no directory was provided, and the canonical path if it was.
        rootDirectory = (unprocessedDirectoryPath.empty()) ?
			std::filesystem::current_path() :
			std::filesystem::canonical(unprocessedDirectoryPath);
    }
    catch (const std::exception& ex) {
		std::cerr << termcolor::bright_red << "Resolving path \"" << termcolor::bright_cyan <<
			unprocessedDirectoryPath << termcolor::bright_red << "\" threw exception:\n" << ex.what() << termcolor::reset << std::endl;
        return false;
    }
	
	// Initializes the string path.
	rootDirectoryString = rootDirectory.generic_u8string() + "/";

	return true;
}

void FileManager::readPaths(
	std::vector<std::string>& forbiddenDirectories,
	std::vector<std::string>& allowedExtensions,
	int depth,
	bool checkCaps
) {
	// Clears the vector.
	relativePathStrings.clear();

	// Validates (and adjusts, if necessary) the allowed depth value.
	depth = adjustDepth(depth);

	// Display basic info
	displayBasicInfo(depth);

	// Determines whether a directory blacklist and an extension whitelist must be included.
	bool isDirectoryBlacklistEnabled = (forbiddenDirectories.size() > 0);
	bool isExtensionWhitelistEnabled = (allowedExtensions.size() > 0);

	// If applicable, parse and display blacklisted directories.
	if (isDirectoryBlacklistEnabled) {
		directoryBlacklist.clear();
		parseBlacklistedDirectories(forbiddenDirectories);
		displayDirectoryBlacklist();
	}

	// If applicable, parse and display whitelisted extensions.
	if (isExtensionWhitelistEnabled) {
		extensionWhitelist.clear();
		parseWhitelistedExtensions(allowedExtensions);
		displayExtensionWhitelist();
	}

	printLine();

	// Initializes counters.
	unsigned int fileCount = 0;
	unsigned int directoryCount = 0;

	try {
		// Recursively iterates through the working directory.
		for (std::filesystem::recursive_directory_iterator i = std::filesystem::recursive_directory_iterator(rootDirectory);
		     i != std::filesystem::recursive_directory_iterator();
		     ++i
		) {
			// If not disabled, check if path storage limit has been exceeded.
			if (
				(fileCount >= MAX_PATHS) &&
				checkCaps
			) {
				displayCapWarning("file paths", MAX_PATHS);
				break;
			}

			// Get the path of the current file.
			std::filesystem::path path = i->path();

			// Do not list directories.
			if (std::filesystem::is_directory(path)) {
				// Disable recursion for this directory if
				if (
					(
						isDirectoryBlacklistEnabled &&
					    isDirectoryBlacklisted(path)
					) ||
					i.depth() >= depth // maximum depth has been reached
				) {
					i.disable_recursion_pending(); // skip
				} else {
					// Count directory.
					++directoryCount;
				}
			}
			// Do list files.
			else {
				// Ignore if extension whitelist is enabled and the current file's extension does not match any.
				if (isExtensionWhitelistEnabled &&
					!isExtensionWhitelisted(path.extension().generic_u8string())) {
					continue;
				}

				// Reads the file path, relative to the working directory, in UTF8 format.
				relativePathStrings.push_back(
					std::filesystem::relative(path, rootDirectory).generic_u8string()
				);

				// Count file and path string bytes.
				++fileCount;
			}
		}
	} catch (const std::exception& ex) {
	    std::cerr << termcolor::bright_red << "ERROR while reading paths into memory:\n" << ex.what() << termcolor::reset << "\n";
		exit(EXIT_FAILURE);
	}

	// Sets the distribution.
	distribution = std::uniform_int_distribution<>(0, (int)relativePathStrings.size() - 1);

	// Displays the final file and directory counts.
	displayFileCounts(fileCount, directoryCount);
	printLine();
}

void FileManager::shuffle() {
	std::shuffle(
		relativePathStrings.begin(),
		relativePathStrings.end(),
		randomEngine
	);
}



void FileManager::executeRandomFile(){
	if (relativePathStrings.size() > 0) {
		executeFile(relativePathStrings[distribution(randomEngine)]);
	} else {
		std::cerr << termcolor::bright_yellow << "No paths have been stored into memory\n" << termcolor::reset;
	}
}

void FileManager::executeSequentialFile(const bool backwards){
	if (relativePathStrings.size() > 0) {
		adjustShuffleIndex(backwards);
		displayPlaylistInfo();
		executeFile(relativePathStrings[shuffleIndex]);
	} else {
		std::cerr << termcolor::bright_yellow << "No paths have been stored into memory\n" << termcolor::reset;
	}
}

void FileManager::executeFirstFile(){
	if (relativePathStrings.size() > 0) {
		displayPlaylistInfo();
		executeFile(relativePathStrings[0]);
	} else {
		std::cerr << termcolor::bright_yellow << "No paths have been stored into memory\n" << termcolor::reset;
	}
}

void FileManager::executeFile(const std::string& relativePath) const{
	// Constructs the path.
	std::wstring wideFilePath = FileManager::utf8ToWide(rootDirectoryString + relativePath);
	
	// Displays the path.
	std::cout << termcolor::bright_cyan << relativePath << termcolor::reset << std::endl;
	
	// Executes the file corresponding to the path.
	ShellExecuteW(0, 0, wideFilePath.c_str(), 0, 0, SW_SHOW);
}

std::wstring FileManager::utf8ToWide(const std::string& utf8str) {
    int count = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str() , (int)utf8str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), (int)utf8str.length(), &wstr[0], count);
    return wstr;
}

int FileManager::adjustDepth(const int depth, const bool checkCaps) {
	if (depth < MIN_DEPTH) {
		return DEPTH_DEFAULT;
	} else if (
		(depth > MAX_DEPTH) &&
		checkCaps
	) {
		displayCapWarning("depth", MAX_DEPTH);
		return MAX_DEPTH;
	}
	else return depth;
}

void FileManager::adjustShuffleIndex(const bool backwards) {
	const int shuffleLastIndex = (int)relativePathStrings.size() - 1;
	if (backwards) {
		if (shuffleIndex == 0) {
			shuffleIndex = shuffleLastIndex;
		} else {
			--shuffleIndex;
		}
	} else {
		if (shuffleIndex == shuffleLastIndex) {
			shuffleIndex = 0;
		} else {
			++shuffleIndex;
		}
	}
}

void FileManager::parseBlacklistedDirectories(const std::vector<std::string>& forbiddenDirectories) {
	try {
		for (const std::string& forbiddenDirectory : forbiddenDirectories) {
			directoryBlacklist.push_back(
				std::filesystem::canonical(forbiddenDirectory)
			);
		}
	} catch (const std::exception& ex) {
	    std::cerr << termcolor::bright_red << "ERROR while reading blacklisted directories into memory:\n" << ex.what() << termcolor::reset << "\n";
		exit(EXIT_FAILURE);
	}
}

void FileManager::parseWhitelistedExtensions(const std::vector<std::string>& allowedExtensions) {
	for (const std::string allowedExtension : allowedExtensions) {
		extensionWhitelist.push_back(
			EXTENSION_DOT + allowedExtension
		);
	}
}

bool FileManager::isDirectoryBlacklisted(const std::filesystem::path& directory) {
	const std::vector<std::filesystem::path>::iterator it = std::find(
	    directoryBlacklist.begin(),
	    directoryBlacklist.end(),
	    directory
	);
	return (it != directoryBlacklist.end());
}

bool FileManager::isExtensionWhitelisted(const std::string& extension) {
	const std::vector<std::string>::iterator it = std::find(
	    extensionWhitelist.begin(),
	    extensionWhitelist.end(),
	    extension
	);
	return (it != extensionWhitelist.end());
}



void FileManager::displayBasicInfo(const int depth) const{
	std::cout << "\nWorking directory: " << termcolor::bright_cyan << rootDirectoryString << termcolor::reset
		<< "\nDepth: " << termcolor::bright_cyan << depth << termcolor::reset;
}

void FileManager::displayDirectoryBlacklist() const{
	std::cout << "\nDirectory blacklist: \n" << termcolor::bright_cyan;
	for (const std::filesystem::path& directory : directoryBlacklist) {
		std::cout << directory.generic_u8string();
	}
	std::cout << termcolor::reset;
}

void FileManager::displayExtensionWhitelist() const{
	std::cout << "\nExtension whitelist: " << termcolor::bright_cyan;
	int i = 0;
	while(i < extensionWhitelist.size()){
		std::cout << extensionWhitelist[i];
		if(++i < extensionWhitelist.size()) std::cout << termcolor::reset << EXTENSION_SEPARATOR << termcolor::bright_cyan;
	}
	std::cout << termcolor::reset;
}

void FileManager::displayCapWarning(const char* capName, const int cap) const{
	std::cout << termcolor::bright_yellow << "\n\nSoft cap for " << capName << " ("
		<< termcolor::bright_cyan << cap << termcolor::bright_yellow
		<< ") has been reached. Disable cap checking with flags "
		<< Args::FLAGS_SHORTENED[Args::nocap] << " or " << Args::FLAGS_WHOLE[Args::nocap] << termcolor::reset << "\n\n";
}

void FileManager::displayFileCounts(const unsigned int fileCount, const unsigned int directoryCount) const{
	std::cout
		<< termcolor::bright_cyan << fileCount      << termcolor::reset << " files, "
		<< termcolor::bright_cyan << directoryCount << termcolor::reset << " subdirectories scanned";
}

void FileManager::displayPlaylistInfo() const{
	std::cout << termcolor::bright_magenta << "["
		<< termcolor::bright_cyan << shuffleIndex + 1
		<< termcolor::bright_magenta << " of "
		<< termcolor::bright_cyan << relativePathStrings.size()
		<< termcolor::bright_magenta << "] " << termcolor::reset;
}

void FileManager::printLine() {
	std::cout << CONSOLE_LINE;
}
