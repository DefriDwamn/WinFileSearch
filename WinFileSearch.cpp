#include <string>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
using namespace std;
using namespace filesystem;

string searchPath;
string searchTarget;

string outputFile;
string extensionsFile;

bool UseExtensionValidator;

int SearchData[3];

vector<string> allowedExtensions;

void Search();
bool ExtensionValidator(string inuptExtension);
void SearchInFile(string filePath);
void WriteInOutputFile(string line, string file);
void ClearOutputFile();
void CreateExtensionsFile();
void SetExtensionsFromFile();

int main()
{
    SetConsoleOutputCP(1251); SetConsoleCP(1251);

    UseExtensionValidator = 0;
    extensionsFile = "EXT.txt";
    outputFile = "OUT.txt";

    CreateExtensionsFile();
    cout << "В файле EXT.txt(В папке с программой) укажите расширения файлов для поиска с новой строки!" << endl;

    while (true) {
        SearchData[0] = 0; SearchData[1] = 0; SearchData[2] = 0;

        cout << "Введите путь: "; getline(cin, searchPath);
        cout << "Поиск: "; getline(cin, searchTarget);

        SetExtensionsFromFile();
        ClearOutputFile();

        try {
            Search();

            cout << "*Просмотрено файлов: " << SearchData[0] << "; Строк: " << SearchData[1] << "; Match-Строк: " << SearchData[2] << endl;
            cout << "*Output file: " << absolute(outputFile).generic_string() << endl;
        } catch(exception& e) {
            cerr << e.what();
        }
    }

    return 0;
}

void Search() {
    for (const auto& dir_entry : recursive_directory_iterator(searchPath)) {
        if (dir_entry.is_regular_file()) {
            string dir_entry_string = dir_entry.path().generic_string();
            string extension = dir_entry.path().extension().generic_string();

            if (ExtensionValidator(extension)) {
                SearchInFile(dir_entry_string);
            }
            SearchData[0]++;
        }
    }
}

bool ExtensionValidator(string inuptExtension)
{
    bool extensionMatch = false;

    if (!UseExtensionValidator)
        return true;

    for (const string& allowedExtension : allowedExtensions) {
        if (inuptExtension == allowedExtension) {
            extensionMatch = true;
            break;
        }
        extensionMatch = false;
    }
    return extensionMatch;
}

void SearchInFile(string filePath)
{
    ifstream fileStream;
    fileStream.open(filePath);
    string line;
    while (getline(fileStream, line)) {
        SearchData[1]++;
        if (line.find(searchTarget) != string::npos) {
            WriteInOutputFile(line, filePath);
            SearchData[2]++;
        }
    }
    fileStream.close();
}

void WriteInOutputFile(string line, string file)
{
    ofstream ofileStream(outputFile, ios::app);
    ofileStream << "File: " << file << endl << "--Line: " + line + "\n\n";
    ofileStream.close();
}

void ClearOutputFile()
{
    ofstream cofs(outputFile, ios::out | ios::trunc);
    cofs.close();
}

void CreateExtensionsFile()
{
    ofstream oExtFileStream;
    oExtFileStream.open(extensionsFile, ios::app);
    oExtFileStream.close();
}

void SetExtensionsFromFile()
{
    ifstream extFileStream;
    extFileStream.open(extensionsFile);
    string line;
    while (getline(extFileStream, line)) {
        if (line[0] == '.' )
            allowedExtensions.push_back(line);
    }
    extFileStream.close();
}