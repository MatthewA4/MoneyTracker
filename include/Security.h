//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <string>
#include <vector>

namespace mt {
namespace security {

// Secure file I/O checks
bool fileExists(const std::string& path);
bool isRegularFile(const std::string& path);
bool isRedactableField(const std::string& fieldName);
std::string redactSensitiveData(const std::string& data, const std::string& fieldName);

// Path validation for security
bool isValidFilePath(const std::string& path);
std::string sanitizeFilePath(const std::string& path);

// CSV-specific safety
bool isSafeCSVField(const std::string& field);
std::string escapeCSVField(const std::string& field);

} // namespace security
} // namespace mt
