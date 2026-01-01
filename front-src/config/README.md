# Configuration Page Source

This directory contains the source code for the configuration page of the Weather Ticker project.

## Technology Stack

This project is built using [Svelte](https://svelte.dev/) and [Vite](https://vitejs.dev/). It compiles into a static single-page application that is served by the ESP32.

## Project Setup

Before running any commands, ensure you have Node.js installed and install the dependencies:

```bash
npm install
```

## Development

To run the configuration page locally for development and testing:

```bash
npm run dev
```

This will start a local development server (usually at http://localhost:5173).

## Deployment

To build the project and add it to the main firmware project, run:

```bash
npm run deploy
```

*What this does:*

1) Builds the Svelte application using Vite.
2) Uses shx (for cross-platform compatibility on Windows/Linux) to copy the build artifacts.
3) Places the files into ../../front/config,

where they will be picked up and packed into the LittleFS partition during the firmware build.