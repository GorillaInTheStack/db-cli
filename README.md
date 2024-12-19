# Simple File Database

Do NOT use this in production :)

## Short Description

This is a simple file-based database application that allows you to create, store, retrieve, update, and delete employee records. The project is designed in a modular way, and it can be interacted with either through a command-line interface (CLI) or via a client-server architecture using a custom protocol over TCP.

This is something I created to play around with, not meant for production.

## Features

- **Create**: Add new employee records to the database.
- **Read**: Retrieve and display employee records.
- **Update Hours**: Modify existing employee hours records.
- **Delete**: Remove employee records from the database.
- **CLI**: Interact with the database using a command-line interface.
- **Client-Server**: Uses a custom protocol over TCP for client-server interactions.

## Available Build Options

- **`make all`**:  
  Builds the CLI, server, and client binaries. This is the default target, so running `make` without arguments will produce all three executables:  
  - `bin/db_cli`  
  - `bin/db_server`  
  - `bin/db_client`

- **`make cli`**:  
  Builds only the CLI binary (`bin/db_cli`).

- **`make server`**:  
  Builds only the server binary (`bin/db_server`).

- **`make client`**:  
  Builds only the client binary (`bin/db_client`).

- **`make test`**:  
  Runs all test binaries. It will execute each test file, print the results, and summarize how many tests passed or failed.

- **`make clean`**:  
  Cleans up all compiled objects and binaries.

## Using the CLI Binary

**Examples:**

- Create a new database file:

```bash
  ./bin/db_cli -n -f ./employees.db
```

- Add a new employee record:

```bash
./bin/db_cli -f ./employees.db -a "John Doe,123 Some St,40"
```

- List all employees:

```bash
./bin/db_cli -f ./employees.db -l
```

- Update an employee’s hours:

```bash
./bin/db_cli -f ./employees.db -u "John Doe" -v 50
```

- Delete an employee:

```bash
./bin/db_cli -f ./employees.db -d "John Doe"
```

## Using the Clinet-Server binaries

```bash
./bin/db_server # port can be modified in PORT in networking.h
```

```bash
./bin/db_client -h 127.0.0.1 -a "John D,Ahh street,22"
./bin/db_client -h 127.0.0.1 -l
./bin/db_client -h 127.0.0.1 -u "John D" -v 800
./bin/db_client -h 127.0.0.1 -d "John D"
```

## Threat Model

TODO

## Protocol analysis

TODO