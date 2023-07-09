#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct SearchData {
  std::string searchPath;
  std::vector<std::string> searchTargets;
  std::string outputFile;
  std::string extsFile;
  size_t checkedFiles = 0;
  size_t checkedStrs = 0;
  size_t matchStrs = 0;
};

std::vector<std::string> Split(const std::string& str, char delimiter) {
  std::vector<std::string> strs;
  size_t last = 0;
  for (size_t i = 0; i != str.size(); ++i) {
    if (str[i] == delimiter) {
      strs.push_back(str.substr(last, (i - last)));
      last = i + 1;
    }
  }
  strs.push_back(str.substr(last, str.size() - last));
  return strs;
}

std::vector<std::string> extensionsFromFile(std::string& extFileName) {
  std::ifstream ifs(extFileName);
  std::vector<std::string> allowedExtensions;
  std::string ext;
  while (ifs >> ext)
    if (!ext.empty()) allowedExtensions.push_back('.' + ext);
  ifs.close();
  return allowedExtensions;
}

bool ExtensionValidator(const std::vector<std::string>& exts,
                        const std::string& nowExt) {
  if (exts.size() == 0) return true;
  for (const std::string& allowedExt : exts) {
    if (nowExt == allowedExt) return true;
  }
  return false;
}

void WriteInOutputFile(std::string& outputFile, const std::string& line,
                       const std::string& file) {
  std::ofstream ofileStream(outputFile, std::ios::app);
  ofileStream << "File: " << file << "\n --Line: " + line + "\n\n";
  ofileStream.close();
}

void SearchInFile(SearchData& data, const std::string& filePath) {
  std::ifstream ifs;
  ifs.open(filePath);
  std::string line;
  while (getline(ifs, line)) {
    ++data.checkedStrs;
    for (std::string& str : data.searchTargets) {
      if (line.find(str) != std::string::npos) {
        ++data.matchStrs;
        WriteInOutputFile(data.outputFile, line, filePath);
      }
    }
  }
  ifs.close();
}

void PrintSearchResult(SearchData& data) {
  std::cout << "*Viewed files: " << data.checkedFiles
            << "; Lines: " << data.checkedStrs
            << "; Match-Lines: " << data.matchStrs << std::endl;
  std::cout << "*Output file: "
            << std::filesystem::absolute(data.outputFile).generic_string()
            << std::endl;
}

void Search(SearchData& data) {
  try {
    std::vector<std::string> exts = extensionsFromFile(data.extsFile);
    for (const auto& dirEntry :
         std::filesystem::recursive_directory_iterator(data.searchPath)) {
      if (dirEntry.is_regular_file()) {
        std::string dirEntry_string = dirEntry.path().generic_string();
        std::string ext_string = dirEntry.path().extension().generic_string();
        if (ExtensionValidator(exts, ext_string)) {
          SearchInFile(data, dirEntry_string);
        }
        ++data.checkedFiles;
      }
    }
    PrintSearchResult(data);
  } catch (std::exception& error) {
    std::cerr << error.what() << std::endl;
  }
}

int main() {
  while (true) {
    SearchData searchData{.outputFile = "OUT.txt", .extsFile = "EXTS.txt"};

    std::cout << "Enter the path: ";
    getline(std::cin, searchData.searchPath);
    std::cout << "Enter the target: ";
    std::string strTarget;
    getline(std::cin, strTarget);
    searchData.searchTargets = Split(strTarget, ' ');

    Search(searchData);
  }
}
