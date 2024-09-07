# Cerver: Simple Multithreaded HTTP Server in C

This project is a simple multithreaded HTTP server implemented in C. It handles incoming HTTP requests by spawning a new thread for each client connection and serves static files from a specified directory. The server is designed to handle basic HTTP GET requests and serve HTML files.

## Features

- Multithreaded: Each client connection is handled in a separate thread, allowing for concurrent processing of multiple requests.
- HTTP 1.1 Support: Basic HTTP 1.1 protocol implementation.
- Static File Serving: Serves static files (e.g., HTML files) from a local directory.
- Graceful Shutdown: Handles `SIGINT` signal (Ctrl+C) for shutting down the server cleanly.
- Logging: Logs important events, such as server startup, connection handling, and errors, with timestamps.

## Getting Started

### Prerequisites

- **GCC**: You need GCC installed to compile the C code.
- **POSIX Environment**: The code relies on POSIX system calls, so it should be run on Linux or macOS.

### Building the Project

To build the project, simply use the provided `Makefile`:

```bash
make build

```

### Running the Server

To run the server, simple use the run command from the Makefile:

```bash
make run
```

- The server will start listening on localhost:8080. You can hit this endpoint using something like curl or wget.
- The index.html file can be altered to suit your needs to serve the concerned static content.

### Closing the server

The server handles shutdown via SIGINT which is done by pressing Ctrl+C and exits cleanly.

## Known Limitations

- No MIME Type handling
- No Request Validation
- No Security Features
- Limited to a Single File (index.html) regardless of the requests
