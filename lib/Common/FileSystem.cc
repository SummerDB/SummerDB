#include "SummerDB/Common/FileSystem.hpp"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/StringUtil.hpp"

namespace SummerDB {

bool DirectoryExists(const std::string& directory) {
  if (!directory.empty()) {
    if (access(directory.c_str(), 0) == 0) {
      struct stat status;
      stat(directory.c_str(), &status);
      if (status.st_mode & S_IFDIR)
        return true;
    }
  }
  // if any condition fails
  return false;
}

bool FileExists(const std::string& filename) {
  if (!filename.empty()) {
    if (access(filename.c_str(), 0) == 0) {
      struct stat status;
      stat(filename.c_str(), &status);
      if (!(status.st_mode & S_IFDIR))
        return true;
    }
  }
  // if any condition fails
  return false;
}

void CreateDirectory(const std::string& directory) {
  struct stat st;

  if (stat(directory.c_str(), &st) != 0) {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(directory.c_str(), 0755) != 0 && errno != EEXIST) {
      throw IOException("Failed create directory!");
    }
  } else if (!S_ISDIR(st.st_mode)) {
    throw IOException("Could not create directory!");
  }
}

void RemoveDirectory(const std::string& directory) {
  auto command = "rm -r " + StringUtil::Replace(directory, " ", "\\ ");
  system(command.c_str());
}

void SetWorkingDirectory(const std::string& directory) {
  chdir(directory.c_str());
}

std::string PathSeparator() { return "/"; }

std::string JoinPath(const std::string& a, const std::string& b) {
  // FIXME: sanitize paths
  return a + PathSeparator() + b;
}

void FileSync(FILE* file) { fsync(fileno(file)); }

#define GetCurrentDir getcwd

std::string GetWorkingDirectory() {
  char current_path[FILENAME_MAX];

  if (!GetCurrentDir(current_path, sizeof(current_path))) {
    return std::string();
  }
  return std::string(current_path);
}

}  // namespace SummerDB