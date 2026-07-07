// Copyright (C) 2026 Matthew Anderson
// MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// AIAssistant.h

#pragma once

#include "TransactionData.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace moneytracker {

/**
 * @brief Callback type for streaming AI responses
 * @param chunk The incremental response chunk from the AI
 * @return true to continue, false to cancel
 */
using AIResponseCallback = std::function<bool(const std::string& chunk)>;

/**
 * @brief Configuration for the AI assistant
 */
struct AIConfig {
    std::string model = "llama3"; // Default model
    std::string ollamaHost = "http://localhost:11434";
    int timeoutSeconds = 120;
    int maxContextTransactions = 50; // How many transactions to include in context
    double temperature = 0.7;        // Creativity vs precision
};

/**
 * @brief Result type for AI responses
 */
struct AIResponse {
    bool success = false;
    std::string content;
    std::string errorMessage;
    int tokensUsed = 0;
};

/**
 * @brief AIAssistant - Local LLM integration using Ollama
 *
 * Provides a budget-focused AI assistant that runs entirely locally
 * using Ollama. No data leaves the user's machine.
 */
class AIAssistant
{
  public:
    AIAssistant();
    explicit AIAssistant(const AIConfig& config);
    ~AIAssistant();

    // Configuration
    void setConfig(const AIConfig& config);
    const AIConfig& getConfig() const;

    // Connection testing
    bool isAvailable() const;
    std::string testConnection();

    // List available models
    std::vector<std::string> listModels();

    // Chat functionality
    AIResponse chat(const std::string& userMessage);
    AIResponse chatWithContext(const std::string& userMessage, const TransactionData& transactions);

    // Streaming chat (callback-based)
    bool chatStream(const std::string& userMessage, AIResponseCallback callback);
    bool chatStreamWithContext(const std::string& userMessage, const TransactionData& transactions,
                               AIResponseCallback callback);

    // Pre-built prompts for common budget questions
    std::string buildSpendingSummary(const TransactionData& transactions);
    std::string buildCategoryAnalysis(const TransactionData& transactions);
    std::string buildMonthlyTrend(const TransactionData& transactions);

    // Check if Ollama is installed and running
    static bool checkOllamaInstalled();

  private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace moneytracker