//Copyright (C) 2026 Matthew Anderson
//MIT License
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//AIAssistant.cpp

#include "AIAssistant.h"
#include "Logger.h"
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <curl/curl.h>

namespace moneytracker {

// Forward declaration of implementation
class AIAssistant::Impl {
public:
    Impl() : config(), curl(nullptr) {
        curl = curl_easy_init();
    }
    
    ~Impl() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }
    
    AIConfig config;
    CURL* curl;
    
    // libcurl write callback
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
    // Streaming callback for chunked responses
    static size_t writeCallbackStream(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t realsize = size * nmemb;
        auto* response = static_cast<std::string*>(userp);
        response->append((char*)contents, realsize);
        
        // Process complete JSON objects (separated by newlines)
        std::string buffer = *response;
        response->clear();
        
        std::istringstream stream(buffer);
        std::string line;
        while (std::getline(stream, line)) {
            if (line.find("\"response\"") != std::string::npos || 
                line.find("\"done\"") != std::string::npos) {
                // Keep partial JSON for next iteration
                if (!line.empty()) {
                    response->append(line);
                    response->append("\n");
                }
            }
        }
        return realsize;
    }
    
    bool isAvailable() const {
        return curl != nullptr;
    }
    
    std::string testConnection() {
        if (!curl) {
            return "CURL not initialized";
        }
        
        std::string response;
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, (config.ollamaHost + "/api/tags").c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            return std::string("Connection failed: ") + curl_easy_strerror(res);
        }
        
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        
        if (httpCode == 200) {
            return "Connected to Ollama";
        }
        
        return "Ollama returned error: " + std::to_string(httpCode);
    }
    
    std::vector<std::string> listModels() {
        std::vector<std::string> models;
        
        if (!curl) {
            return models;
        }
        
        std::string response;
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, (config.ollamaHost + "/api/tags").c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        CURLcode res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            // Parse JSON response to extract model names
            // Simple parsing - look for "name" fields
            size_t pos = 0;
            while ((pos = response.find("\"name\":", pos)) != std::string::npos) {
                size_t start = response.find("\"", pos + 7) + 1;
                size_t end = response.find("\"", start);
                if (start != std::string::npos && end != std::string::npos) {
                    std::string model = response.substr(start, end - start);
                    models.push_back(model);
                }
                pos = end;
            }
        }
        
        return models;
    }
    
    std::string escapeJson(const std::string& input) {
        std::string output;
        for (char c : input) {
            switch (c) {
                case '"': output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\n': output += "\\n"; break;
                case '\r': output += "\\r"; break;
                case '\t': output += "\\t"; break;
                default: output += c; break;
            }
        }
        return output;
    }
    
    AIResponse sendChatRequest(const std::string& prompt, bool stream = false) {
        AIResponse result;
        
        if (!curl) {
            result.errorMessage = "CURL not initialized";
            return result;
        }
        
        // Build JSON request
        std::ostringstream json;
        json << "{"
             << "\"model\": \"" << escapeJson(config.model) << "\","
             << "\"prompt\": \"" << escapeJson(prompt) << "\","
             << "\"stream\": " << (stream ? "true" : "false") << ","
             << "\"options\": {"
             << "\"temperature\": " << config.temperature << ","
             << "\"num_predict\": 512"
             << "}"
             << "}";
        
        std::string requestBody = json.str();
        std::string response;
        
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, (config.ollamaHost + "/api/generate").c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        if (stream) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallbackStream);
        } else {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        }
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config.timeoutSeconds);
        
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            result.errorMessage = std::string("Request failed: ") + curl_easy_strerror(res);
            return result;
        }
        
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        
        if (httpCode != 200) {
            result.errorMessage = "HTTP error: " + std::to_string(httpCode);
            return result;
        }
        
        // Parse response - look for "response" field
        size_t pos = response.find("\"response\":");
        if (pos != std::string::npos) {
            size_t start = response.find("\"", pos + 11) + 1;
            size_t end = response.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                result.content = response.substr(start, end - start);
                result.success = true;
            }
        } else {
            // Try alternative parsing for streaming responses
            std::istringstream stream(response);
            std::string line;
            while (std::getline(stream, line)) {
                size_t p = line.find("\"response\":");
                if (p != std::string::npos) {
                    size_t s = line.find("\"", p + 11) + 1;
                    size_t e = line.find("\"", s);
                    if (s != std::string::npos && e != std::string::npos) {
                        result.content += line.substr(s, e - s);
                        result.success = true;
                    }
                }
            }
        }
        
        return result;
    }
    
    std::string buildTransactionContext(const TransactionData& transactions) {
        std::ostringstream context;
        
        auto allTransactions = transactions.getAllTransactions();
        int count = std::min((int)allTransactions.size(), config.maxContextTransactions);
        
        context << "You are a helpful budget assistant. Here is the user's transaction data:\n\n";
        
        if (count == 0) {
            context << "No transactions available.\n";
            return context.str();
        }
        
        context << "Recent Transactions (showing " << count << " of " << allTransactions.size() << "):\n";
        context << "Date       | Description                    | Amount    | Category    | Account\n";
        context << "-----------|--------------------------------|-----------|-------------|--------\n";
        
        // Show most recent transactions
        int start = std::max(0, (int)allTransactions.size() - count);
        for (int i = start; i < (int)allTransactions.size(); ++i) {
            const auto& t = allTransactions[i];
            context << std::left << std::setw(10) << t.date << " | "
                    << std::left << std::setw(30) << t.description.substr(0, 30) << " | "
                    << std::right << std::setw(9) << std::fixed << std::setprecision(2) << t.amount << " | "
                    << std::left << std::setw(11) << t.category << " | "
                    << t.accountName << "\n";
        }
        
        // Add category totals
        context << "\nSpending by Category:\n";
        auto categoryTotals = transactions.getCategoryTotals();
        for (const auto& [category, total] : categoryTotals) {
            context << "  " << category << ": $" << std::fixed << std::setprecision(2) << total << "\n";
        }
        
        // Add monthly totals
        context << "\nMonthly Totals:\n";
        auto monthlyTotals = transactions.getMonthlyTotals();
        for (const auto& [month, total] : monthlyTotals) {
            context << "  " << month << ": $" << std::fixed << std::setprecision(2) << total << "\n";
        }
        
        context << "\nTotal Spending: $" << std::fixed << std::setprecision(2) 
                << transactions.getTotalSpending() << "\n";
        context << "Average Transaction: $" << std::fixed << std::setprecision(2) 
                << transactions.getAverageTransaction() << "\n";
        
        return context.str();
    }
};

// Constructor
AIAssistant::AIAssistant() : pImpl(std::make_unique<Impl>()) {}

// Constructor with config
AIAssistant::AIAssistant(const AIConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

// Destructor
AIAssistant::~AIAssistant() = default;

// Configuration
void AIAssistant::setConfig(const AIConfig& config) {
    pImpl->config = config;
}

const AIConfig& AIAssistant::getConfig() const {
    return pImpl->config;
}

// Connection testing
bool AIAssistant::isAvailable() const {
    return pImpl->isAvailable();
}

std::string AIAssistant::testConnection() {
    return pImpl->testConnection();
}

// List available models
std::vector<std::string> AIAssistant::listModels() {
    return pImpl->listModels();
}

// Simple chat
AIResponse AIAssistant::chat(const std::string& userMessage) {
    std::string prompt = "You are a helpful budget and personal finance assistant. ";
    prompt += "Provide clear, actionable advice about spending, saving, and budgeting. ";
    prompt += "Keep responses concise and practical.\n\n";
    prompt += "User: " + userMessage + "\n\nAssistant:";
    
    return pImpl->sendChatRequest(prompt);
}

// Chat with transaction context
AIResponse AIAssistant::chatWithContext(const std::string& userMessage, 
                                         const TransactionData& transactions) {
    std::string context = pImpl->buildTransactionContext(transactions);
    
    std::string prompt = "You are a helpful budget and personal finance assistant. ";
    prompt += "Use the transaction data provided below to answer the user's question accurately. ";
    prompt += "Provide specific numbers and insights when available.\n\n";
    prompt += context + "\n\nUser question: " + userMessage + "\n\nPlease provide a helpful response:";
    
    return pImpl->sendChatRequest(prompt);
}

// Streaming chat
bool AIAssistant::chatStream(const std::string& userMessage, AIResponseCallback callback) {
    std::string prompt = "You are a helpful budget and personal finance assistant. ";
    prompt += "Provide clear, actionable advice about spending, saving, and budgeting.\n\n";
    prompt += "User: " + userMessage + "\n\nAssistant:";
    
    AIResponse response = pImpl->sendChatRequest(prompt, true);
    if (response.success && !response.content.empty()) {
        return callback(response.content);
    }
    return false;
}

// Streaming chat with context
bool AIAssistant::chatStreamWithContext(const std::string& userMessage,
                                         const TransactionData& transactions,
                                         AIResponseCallback callback) {
    std::string context = pImpl->buildTransactionContext(transactions);
    
    std::string prompt = "You are a helpful budget and personal finance assistant. ";
    prompt += "Use the transaction data provided below.\n\n";
    prompt += context + "\n\nUser: " + userMessage + "\n\nAssistant:";
    
    AIResponse response = pImpl->sendChatRequest(prompt, true);
    if (response.success && !response.content.empty()) {
        return callback(response.content);
    }
    return false;
}

// Pre-built prompts
std::string AIAssistant::buildSpendingSummary(const TransactionData& transactions) {
    return "Provide a spending summary: What are the top spending categories? "
           "What percentage of spending goes to each? Any surprising findings?";
}

std::string AIAssistant::buildCategoryAnalysis(const TransactionData& transactions) {
    return "Analyze the spending patterns: Which categories have the most transactions? "
           "Are there any categories where spending seems unusually high or low?";
}

std::string AIAssistant::buildMonthlyTrend(const TransactionData& transactions) {
    return "Analyze monthly trends: How does spending compare month over month? "
           "Are there any seasonal patterns? What's the trend direction?";
}

// Static method to check Ollama
bool AIAssistant::checkOllamaInstalled() {
    CURL* testCurl = curl_easy_init();
    if (!testCurl) {
        return false;
    }
    
    std::string response;
    curl_easy_reset(testCurl);
    curl_easy_setopt(testCurl, CURLOPT_URL, "http://localhost:11434/api/tags");
    curl_easy_setopt(testCurl, CURLOPT_WRITEFUNCTION, Impl::writeCallback);
    curl_easy_setopt(testCurl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(testCurl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(testCurl, CURLOPT_CONNECTTIMEOUT, 3L);
    
    CURLcode res = curl_easy_perform(testCurl);
    curl_easy_cleanup(testCurl);
    
    return res == CURLE_OK;
}

} // namespace moneytracker