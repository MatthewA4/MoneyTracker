#!/bin/bash
# MoneyTracker GUI Launcher
# Runs the GUI from the correct directory with proper setup

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check if GUI is built
if [ ! -f "build/moneytracker-gui" ]; then
    echo "❌ GUI not built. Building now..."
    mkdir -p build
    cd build
    cmake .. && ninja
    cd ..
fi

# Check Ollama status
if command -v ollama &> /dev/null; then
    if curl -s http://localhost:11434/api/tags > /dev/null 2>&1; then
        echo "✅ Ollama is running"
    else
        echo "⚠️  Ollama not running. For AI features:"
        echo "   1. Run: ollama serve"
        echo "   2. Pull a model: ollama pull llama3"
    fi
else
    echo "💡 Ollama not installed. For AI features:"
    echo "   Install from: https://ollama.ai"
fi

echo ""
echo "🚀 Starting MoneyTracker GUI..."
./build/moneytracker-gui
