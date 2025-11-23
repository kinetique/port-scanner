# Port Scanner

A simple TCP port scanner written in C.  
The project implements a small command-line utility that checks the availability of TCP ports on a given IPv4 host using non-blocking sockets and timeouts.

---

## Group Project

**Team members:**

- Chernukha Sophia — `kinetique`
- Spirina Daria — `Hedgehog-blue`

---

## Project Overview

The goal of this project is to build a small, testable network utility with a clear modular structure:

- accept scan parameters (IP address, port range, timeout, verbosity level);
- attempt to connect to each port in the given range;
- classify each port as **OPEN**, **CLOSED**, or **FILTERED**;
- display results in a readable format.

The implementation focuses on:

- clean separation of responsibilities between modules;
- safe handling of input parameters;
- simple unit tests for core logic.

---

## Features

- Scan a range of TCP ports on an IPv4 host
- Per-port status: `OPEN`, `CLOSED`, `FILTERED`
- Configurable timeout for connection attempts
- Optional verbose mode with a short scan summary
- Minimal dependencies — only the C standard library and POSIX sockets
- Small set of unit tests for controller and output modules

---

## How to Build & Run

### Build the project

```bash
make
```

### Run the scanner

#### Scan the single port:

```bash
./port_scanner --ip <ip> --port <port> [--timeout ms] [--verbose]
```

#### Scan the range of ports:

```bash
./port_scanner --ip <ip> --port <start_port> --range <count> [--timeout ms] [--threads <num>] [--verbose]
```
**Example:**

```bash
# Scan single port 80
./port_scanner --ip 192.168.1.1 --port 80 --timeout 1000 --verbose

# Scan 50 ports starting from port 20 (ports 20-69)
./port_scanner --ip 192.168.1.1 --port 20 --range 50 --timeout 1000 --verbose

# Scan with 10 threads for faster execution
./port_scanner --ip 192.168.1.1 --port 20 --range 100 --threads 10 --timeout 1000
```

---

## Project Structure

```text
port-scanner/
├── demo/
│   └── demo_scanner.c          # Optional demo files / examples
├── include/                    # Header files (public API of modules)
│   ├── arg_parse.h
│   ├── network.h
│   ├── output.h
│   └── scan_controller.h
├── src/                        # Implementation of modules
│   ├── arg_parse.c
│   ├── main.c
│   ├── network.c
│   ├── output.c
│   └── scan_controller.c
├── tests/                      # Simple unit tests for core modules
│   ├── test_arg_parse.c
│   ├── test_network.c
│   ├── test_output.c
│   └── test_scan_controller.c
├── .gitignore
├── Makefile
└── README.md
```

---

## Module Description

- **arg_parse** — command-line argument parsing
- **network** — low-level socket operations and connections
- **scan_controller** — port range scanning logic
- **output** — result formatting and display
- **main** — program entry point

---

## Testing

To run tests, execute:

```bash
make test
```

Tests verify the correctness of core modules and error handling.

---

## Requirements

- GCC or Clang compiler
- POSIX-compatible operating system (Linux, macOS)
- Make

---

## License

This project was created for educational purposes.