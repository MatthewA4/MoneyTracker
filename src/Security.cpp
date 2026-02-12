//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "Security.h"
#include <filesystem>
#include <algorithm>

namespace mt {
namespace security {

bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool isRegularFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool isRedactableField(const std::string& fieldName) {
    static const std::vector<std::string> redactable = {
        "amount", "balance", "salary", "income", "card", "account", "routing"};
    return std::find(redactable.begin(), redactable.end(), fieldName) != redactable.end();
}

std::string redactSensitiveData(const std::string& data, const std::string& fieldName) {
    if (!isRedactableField(fieldName)) {
        return data;
    }
    // For financial data, show only last N chars
    if (data.length() > 4) {
        return "****" + data.substr(data.length() - 4);
    }
    return "****";
}

bool isValidFilePath(const std::string& path) {
    if (path.empty()) return false;
    if (path.find("..") != std::string::npos) return false;
    if (path.find('\0') != std::string::npos) return false;
    return true;
}

std::string sanitizeFilePath(const std::string& path) {
    std::string sanitized = path;
    std::replace(sanitized.begin(), sanitized.end(), '\\', '/');
    return sanitized;
}

bool isSafeCSVField(const std::string& field) {
    // Reject fields with suspicious patterns
    if (field.find('\0') != std::string::npos) return false;
    if (field.length() > 10000) return false; // Reject oversized fields
    return true;
}

std::string escapeCSVField(const std::string& field) {
    if (field.find('"') == std::string::npos && field.find(',') == std::string::npos) {
        return field;
    }
    return "\"" + field + "\"";
}

} // namespace security
} // namespace mt
