#pragma once
#include <string>
#include <chrono>

// --- Konstantat globale ---
extern const std::string ADMIN_SECRET;
extern const char* DATA_DIR;

extern const int PORT;
extern const int TIMEOUT_SECONDS;

#define BUFFER_SIZE 1024

// --- Variabla globale ---
extern bool isAuthenticated; 
