//shared.cpp 
#include "shared.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <chrono>


namespace fs = std::filesystem;

// ===============================
// --- KONSTANTAT GLOBALE ---
// ===============================
const string ADMIN_SECRET = "letmein";   // Fjalëkalimi për admin
const char* DATA_DIR = "data";           // Folder ku ruhen fajllat
const int PORT = 12345;                  // Porti i serverit
const int TIMEOUT_SECONDS = 60;          // Timeout për klientët

// ===============================
// --- VARIABLA GLOBALE ---
// ===============================
bool isAuthenticated = false;

// ===============================
// --- FUNKSIONET NDIHMËSE ---
// ===============================

// Krijon folderin "data" nëse nuk ekziston
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
    return ss.str().empty() ? "Asnjë fajll në data/." : ss.str();
}

// Fshin fajllin
string deleteFile(const string& filename) {
    ensureDataDir();
    string path = string(DATA_DIR) + "/" + filename;
    if (!fs::exists(path))
        return "Gabim: Fajlli nuk ekziston.";
    fs::remove(path);
    return " Fajlli u fshi me sukses.";
}

// Kthen informacionet e fajllit
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
    ss << "Madhësia: " << fsize << " bajte\n";
    ss << "Data e fundit e modifikimit: "
        << put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << "\n";
    return ss.str();
}
std::string uploadFile(const std::string& name, const std::string& content) {
    std::ofstream file(DATA_DIR + std::string("/") + name);
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
    return result.empty() ? "Asnjë fajll nuk përmban këtë fjalë." : result;
}

string handleCommand(const string& input) {
    stringstream ss(input);
    string cmd;
    ss >> cmd;

    // /auth <sekreti>
    if (cmd == "/auth") {
        string key;
        ss >> key;
        if (key == ADMIN_SECRET) {
            isAuthenticated = true;
            return "Autentikimi u realizua me sukses!";
        }
        return " Fjalëkalimi gabim.";
    }

    // Nëse nuk është autentikuar – lejo vetëm disa komanda
    if (!isAuthenticated) {
        if (cmd == "/read") { string f; ss >> f; return readFile(f); }
        if (cmd == "/search") { string k; ss >> k; return searchFiles(k); }
        if (cmd == "/list") return listFiles();
        return "Gabim: Duhet të autentikoheni me /auth për komandat admin.";
    }

    // Komandat e lejuara për admin
    if (cmd == "/list") return listFiles();
    else if (cmd == "/read") { string f; ss >> f; return readFile(f); }
    else if (cmd == "/delete") { string f; ss >> f; return deleteFile(f); }
    else if (cmd == "/search") { string k; ss >> k; return searchFiles(k); }
    else if (cmd == "/info") { string f; ss >> f; return infoFile(f); }
    else if (cmd == "/upload") {
        string data; getline(ss, data);
        size_t sep = data.find('|');
        if (sep == string::npos)
            return "Sintaksë: /upload <filename>|<content>";
        string fn = data.substr(0, sep);
        string content = data.substr(sep + 1);
        fn.erase(remove(fn.begin(), fn.end(), ' '), fn.end());
        return uploadFile(fn, content);
    }
    else if (cmd == "/download") { string f; ss >> f; return downloadFile(f); }

    return "Komandë e panjohur.";
}
