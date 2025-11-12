//shared.cpp 
#include "shared.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <chrono>


using namespace std;
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