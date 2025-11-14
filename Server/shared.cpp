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

std::string handleCommand(const std::string& cmd) {

    if (cmd.starts_with("/list")) return listFiles();

    if (cmd.starts_with("/read "))
        return readFile(cmd.substr(6));

    if (!isAuthenticated)
        return "Gabim: Duhet /auth letmein";

    if (cmd.starts_with("/delete "))
        return deleteFile(cmd.substr(8));

    if (cmd.starts_with("/search "))
        return searchFiles(cmd.substr(8));

    if (cmd.starts_with("/info "))
        return fileInfo(cmd.substr(6));

    if (cmd.starts_with("/upload ")) {
        size_t pos = cmd.find('|');
        if (pos == std::string::npos) return "Format i gabuar.";
        return uploadFile(cmd.substr(8, pos - 8), cmd.substr(pos + 1));
    }

    if (cmd.starts_with("/download "))
        return downloadFile(cmd.substr(10));

    return "Komande e panjohur!";
}