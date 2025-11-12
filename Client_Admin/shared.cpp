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