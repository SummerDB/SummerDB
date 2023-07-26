#ifndef SUMMERDB_COMMON_FILE_SYSTEM_HPP
#define SUMMERDB_COMMON_FILE_SYSTEM_HPP

#include <string>

namespace SumemrDB {

//! Check if a directory exists
bool DirectoryExists(const std::string& directory);
//! Create a directory if it does not exist
void CreateDirectory(const std::string& directory);
//! Recursively remove a directory and all files in it
void RemoveDirectory(const std::string& directory);
//! Sets the current working directory
void SetWorkingDirectory(const std::string& directory);
//! Gets the current working directory
std::string GetWorkingDirectory();
//! Check if a file exists
bool FileExists(const std::string& filename);
//! Path separator for the current file system
std::string PathSeparator();
//! Join two paths together
std::string JoinPath(const std::string& a, const std::string& path);
//! Sync a file descriptor to disk
void FileSync(FILE* file);

}  // namespace SumemrDB

#endif  // SUMMERDB_COMMON_FILE_SYSTEM_HPP