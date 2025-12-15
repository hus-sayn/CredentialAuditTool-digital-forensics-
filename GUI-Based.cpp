#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QInputDialog>
#include <QWidget>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
namespace fs = std::filesystem;

// Function to retrieve usernames and password hashes from /etc/shadow
void retrieveShadowPasswords(QTextEdit* outputArea) {
    ifstream shadowFile("/etc/shadow");
    if (!shadowFile.is_open()) {
        outputArea->append("Error: Unable to open /etc/shadow. Run as root.");
        return;
    }

    string line;
    while (getline(shadowFile, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string username = line.substr(0, colonPos);
            string hash = line.substr(colonPos + 1, line.find(':', colonPos + 1) - colonPos - 1);
            outputArea->append(QString::fromStdString("Username: " + username + ", Password Hash: " + hash));
        }
    }
    shadowFile.close();
}

// Function to check if a file contains password-related keywords
bool containsPassword(const string& filePath, QTextEdit* outputArea) {
    ifstream file(filePath);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.find("password") != string::npos || line.find("pwd") != string::npos) {
            outputArea->append(QString::fromStdString("Possible password found in file: " + filePath + "\nContent: " + line));
            return true;
        }
    }
    return false;
}

// Function to scan a directory recursively for files containing password-related keywords
void scanDirectory(const string& dirPath, QTextEdit* outputArea) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            containsPassword(entry.path().string(), outputArea);
        }
    }
}

// Function to retrieve encrypted usernames and passwords from Firefox's logins.json
void retrieveFirefoxPasswords(QTextEdit* outputArea) {
    string filePath = "/home/kali/.mozilla/firefox/p50jsnvg.default-esr/logins.json"; // Default path
    ifstream file(filePath, ifstream::binary);
    if (!file.is_open()) {
        outputArea->append(QString::fromStdString("Error: Unable to open " + filePath));
        return;
    }

    nlohmann::json loginsData;
    try {
        file >> loginsData;
    } catch (const nlohmann::json::parse_error& e) {
        outputArea->append(QString::fromStdString("Error parsing JSON file: " + string(e.what())));
        return;
    }

    if (loginsData.contains("logins")) {
        const auto& logins = loginsData["logins"];
        for (const auto& login : logins) {
            string hostname = login["hostname"];
            string username = login["encryptedUsername"];
            string password = login["encryptedPassword"];

            outputArea->append(QString::fromStdString("URL: " + hostname + "\n"
                                                      + "Encrypted Username: " + username + "\n"
                                                      + "Encrypted Password: " + password + "\n"));
        }
    } else {
        outputArea->append("No logins found in the file.");
    }
}

// Main function with GUI integration
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout layout;

    QPushButton shadowButton("Retrieve Shadow Passwords");
    QPushButton tempButton("Scan Temporary Files");
    QPushButton firefoxButton("Extract Browser Passwords");
    QTextEdit outputArea;

    layout.addWidget(&shadowButton);
    layout.addWidget(&tempButton);
    layout.addWidget(&firefoxButton);
    layout.addWidget(&outputArea);

    window.setLayout(&layout);
    window.setWindowTitle("Password Extractor Tool");
    window.resize(600, 400);
    window.show();

    // Connect buttons to respective functions
    QObject::connect(&shadowButton, &QPushButton::clicked, [&]() {
        outputArea.clear();
        retrieveShadowPasswords(&outputArea);
    });
    QObject::connect(&tempButton, &QPushButton::clicked, [&]() {
        outputArea.clear();
        bool ok;
        QString dirPath = QInputDialog::getText(&window, "Enter Directory",
                                                "Enter the directory path to scan for passwords:",
                                                QLineEdit::Normal, "/home/kali", &ok);
        if (ok && !dirPath.isEmpty()) {
            if (fs::exists(dirPath.toStdString()) && fs::is_directory(dirPath.toStdString())) {
                scanDirectory(dirPath.toStdString(), &outputArea);
            } else {
                outputArea.append("Invalid directory path!");
            }
        }
    });
    QObject::connect(&firefoxButton, &QPushButton::clicked, [&]() {
        outputArea.clear();
        retrieveFirefoxPasswords(&outputArea);
    });

    return app.exec();
}
