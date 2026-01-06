#!/bin/bash
set -e

FRONTEND_DIR="$1"
OUTPUT_DIR="$2"

echo "================================"
echo "Building front-end application"
echo "================================"
echo "Frontend dir: $FRONTEND_DIR"
echo "Output dir: $OUTPUT_DIR"
echo ""

cd "$FRONTEND_DIR"

# Check if Node.js is available
if ! command -v npm &> /dev/null; then
    echo "ERROR: npm is not installed!"
    echo "Please rebuild your devcontainer to install Node.js"
    exit 1
fi

echo "Node.js version: $(node --version)"
echo "npm version: $(npm --version)"
echo ""

# Install dependencies if node_modules doesn't exist
if [ ! -d "node_modules" ]; then
    echo "Installing npm dependencies..."
    npm install
else
    echo "Dependencies already installed (node_modules exists)"
fi

echo ""
echo "Running npm run deploy..."
npm run deploy

echo ""
echo "Front-end build completed successfully!"
echo "Output directory: $OUTPUT_DIR"

# Verify output exists
if [ ! -d "$OUTPUT_DIR" ]; then
    echo "ERROR: Output directory was not created!"
    exit 1
fi

echo "✓ Front-end ready for LittleFS image creation"
