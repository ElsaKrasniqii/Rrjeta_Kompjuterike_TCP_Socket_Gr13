#include "shared.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

namespace fs = filesystem;

const string ADMIN_SECRET = "letmein";
const char* DATA_DIR = "data";
const int PORT = 12345;
const int TIMEOUT_SECONDS = 60;

bool isAuthenticated = false;

void ensureDataDir() {
    if (!fs::exists(DATA_DIR))
        fs::create_directory(DATA_DIR);
}

string readFile(const string& filename) {
    ensureDataDir();
    string path = string(DATA_DIR) + "/" + filename;
    ifstream file(path, ios::binary);
    if (!file)
        return "Gabim: Fajlli nuk ekziston.";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
string listFiles() {
    ensureDataDir();
    stringstream ss;
    for (const auto& entry : fs::directory_iterator(DATA_DIR)) {
        ss << entry.path().filename().string() << "\n";
    }
    return ss.str().empty() ? "Asnje fajll ne data/." : ss.str();
}

string deleteFile(const string& filename) {
    ensureDataDir();
    string path = string(DATA_DIR) + "/" + filename;
    if (!fs::exists(path))
        return "Gabim: Fajlli nuk ekziston.";
    fs::remove(path);
    return " Fajlli u fshi me sukses.";
}

string infoFile(const string& filename) {
    ensureDataDir();
    string path = string(DATA_DIR) + "/" + filename;
    if (!fs::exists(path))
        return "Gabim: Fajlli nuk ekziston.";

    auto fsize = fs::file_size(path);
    auto ftime = fs::last_write_time(path);

    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + chrono::system_clock::now()
    );
    time_t cftime = chrono::system_clock::to_time_t(sctp);

    tm timeinfo{};
    localtime_s(&timeinfo, &cftime);

    stringstream ss;
    ss << "Madhesia: " << fsize << " bajte\n";
    ss << "Data e fundit e modifikimit: "
        << put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << "\n";
    return ss.str();
}

string uploadFile(const string& name, const string& content) {
    ofstream file(DATA_DIR + string("/") + name);
    if (!file) return "Gabim: nuk mund te krijohet fajlli.";

    file << content;
    return "Fajlli u ruajt me sukses.";
}

std::string downloadFile(const std::string& name) {
    return readFile(name);
}

string searchFiles(const string& keyword) {
    ensureDataDir();
    stringstream ss;
    for (const auto& entry : fs::directory_iterator(DATA_DIR)) {
        ifstream file(entry.path(), ios::binary);
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        if (content.find(keyword) != string::npos)
            ss << entry.path().filename().string() << "\n";
    }

    string result = ss.str();
    return result.empty() ? "Asnje fajll nuk permban kete fjale." : result;
}
string handleCommand(const string& input) {
    stringstream ss(input);
    string cmd;
    ss >> cmd;

    if (cmd == "/auth") {
        string key;
        ss >> key;
        if (key == ADMIN_SECRET) {
            isAuthenticated = true;
            return "Autentikimi u realizua me sukses!";
        }
        return " Fjalekalimi gabim.";
    }

    if (!isAuthenticated) {
        if (cmd == "/read") { string f; ss >> f; return readFile(f); }
        if (cmd == "/search") { string k; ss >> k; return searchFiles(k); }
        if (cmd == "/list") return listFiles();
        return "Gabim: Duhet te autentikoheni me /auth per komandat admin.";
    }

    if (cmd == "/list") return listFiles();
    else if (cmd == "/read") { string f; ss >> f; return readFile(f); }
    else if (cmd == "/delete") { string f; ss >> f; return deleteFile(f); }
    else if (cmd == "/search") { string k; ss >> k; return searchFiles(k); }
    else if (cmd == "/info") { string f; ss >> f; return infoFile(f); }
    else if (cmd == "/upload") {
        string data; getline(ss, data);
        size_t sep = data.find('|');
        if (sep == string::npos)
            return "Sintakse: /upload <filename>|<content>";
        string fn = data.substr(0, sep);
        string content = data.substr(sep + 1);
        fn.erase(remove(fn.begin(), fn.end(), ' '), fn.end());
        return uploadFile(fn, content);
    }
    else if (cmd == "/download") { string f; ss >> f; return downloadFile(f); }

    return "Komande e panjohur.";
}
