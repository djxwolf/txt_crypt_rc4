# RC4 Text File Encryption Tool

A simple Qt/C++ based text file encryption and decryption tool using the RC4 algorithm.

## Features

- **Encrypt/decrypt text files** using RC4 algorithm
- **Timestamp as encryption key**, no additional input required
- **Configurable timeout verification** (default: 600 seconds, 0 or negative to disable)
- **In-place mode** - directly overwrite original file
- **Dual interface** - GUI and Command Line Interface
- **Automatic file type detection** - detects encrypted files and enables appropriate action
- **Progress tracking** - visual feedback during encryption/decryption
- **Status messages** - clear feedback in status bar
- **Cross-platform builds** - Automated CI/CD for Linux, macOS, and Windows

## Installation

### Pre-built Binaries

Download pre-built executables from the [Releases](https://github.com/djxwolf/txt_crypt_rc4/releases) page or [GitHub Actions Artifacts](https://github.com/djxwolf/txt_crypt_rc4/actions).

| Platform | Artifact |
|----------|----------|
| Linux | `txt_crypt_cli-linux` |
| macOS | `txt_crypt_cli-macos` |
| Windows | `txt_crypt_cli-windows.exe` |

### Build from Source

#### Dependencies

- Qt 6.x (or Qt 5.15+)
- CMake 3.16+
- C++17 compiler

#### macOS

```bash
brew install qt@6
git clone https://github.com/djxwolf/txt_crypt_rc4.git
cd txt_crypt_rc4
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
make -j4
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get install qt6-base-dev cmake g++
git clone https://github.com/djxwolf/txt_crypt_rc4.git
cd txt_crypt_rc4
mkdir build && cd build
cmake ..
make -j4
```

#### Windows (MSYS2)

```bash
# Install MSYS2 and required packages
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-qt6-base mingw-w64-x86_64-ninja

git clone https://github.com/djxwolf/txt_crypt_rc4.git
cd txt_crypt_rc4
mkdir build && cd build
cmake .. -G Ninja
cmake --build . --target txt_crypt_cli
```

Two executables will be built:
- `TxtCryptRC4` - GUI application
- `txt_crypt_cli` - Command line interface

### Install

#### GUI Application

```bash
# macOS: Copy to Applications folder
cp build/TxtCryptRC4 /Applications/TxtCryptRC4.app

# Or run directly
./build/TxtCryptRC4
```

#### Command Line Tool

```bash
# Create symlink for system-wide access
sudo ln -sf $(pwd)/build/txt_crypt_cli /usr/local/bin/txt_crypt_cli

# Or add to PATH
export PATH="$(pwd)/build:$PATH"
```

## Usage

### GUI Application

1. **Select Input File** - Browse or type file path
2. **Select Output File** - Or check "In-place" to overwrite original
3. **Set Timeout** - Default is 600 seconds (0 or negative = no check)
4. **Click Action** - "Encrypt" or "Decrypt" button

Features:
- Automatic detection of encrypted files
- Progress bar shows operation status
- Status bar displays messages
- Buttons enable/disable based on file type

### Command Line Interface

```bash
# Basic usage with positional argument
txt_crypt_cli encrypt <input-file> [output-file]
txt_crypt_cli decrypt <input-file> [output-file]

# Encrypt with auto-named output
txt_crypt_cli encrypt document.txt
# Output: document_encrypted.txt

# Decrypt with auto-named output
txt_crypt_cli decrypt document_encrypted.txt
# Output: document_encrypted_decrypted.txt

# Specify output file as positional argument
txt_crypt_cli encrypt document.txt encrypted.txt
txt_crypt_cli decrypt encrypted.txt restored.txt

# In-place mode (overwrite original)
txt_crypt_cli encrypt -i document.txt
txt_crypt_cli decrypt -i encrypted.txt

# Specify output file with -o option
txt_crypt_cli encrypt -o output.txt input.txt
txt_crypt_cli decrypt -o restored.txt encrypted.txt

# Disable timeout check
txt_crypt_cli decrypt -t 0 encrypted.txt

# Custom timeout (300 seconds)
txt_crypt_cli decrypt -t 300 encrypted.txt
```

#### Options

| Option | Short | Description |
|--------|-------|-------------|
| `--timeout` | `-t` | Timeout in seconds (default: 600) |
| `--output` | `-o` | Output file path |
| `--in-place` | `-i` | Overwrite input file |
| `--help` | `-h` | Show help message |
| `--version` | `-v` | Show version |

## Encryption Format

The encrypted file content format is:

```
timestamp@Base64-encoded-ciphertext
```

Example:
```
1712345678@SGVsbG8gV29ybGQ=
```

- **Timestamp**: Unix timestamp (seconds since epoch) used as encryption key
- **Base64**: Encrypted data encoded in Base64 format

During decryption:
1. Extract timestamp from encrypted data
2. Validate timestamp is within reasonable range (2020-2030)
3. Validate Base64 format
4. Verify timeout if enabled
5. Decrypt using timestamp as key

## File Path Handling

### Input File
- **Absolute path**: Used as-is
- **Relative path**: Resolved relative to user's home directory (`~`)
- **Filename only**: Resolved from home directory

### Output File
- **Absolute path**: Used as-is
- **Relative path**: Resolved relative to input file's directory
- **Filename only**: Created in input file's directory

## CI/CD

This project uses GitHub Actions for automated builds:

- **Daily builds** at 1 AM Beijing time (UTC 17:00)
- **Multi-platform support**: Linux, macOS, Windows
- **Automated testing** on each build
- **Artifact uploads** for easy download

View builds: https://github.com/djxwolf/txt_crypt_rc4/actions

## Development

### Code Style

See [docs/CODING_STANDARDS.md](docs/CODING_STANDARDS.md) for project coding standards.

### Project Structure

```
txt_crypt_rc4/
├── src/              # Source files
│   ├── MainWindow.h/cpp    # GUI main window
│   ├── RC4Cipher.h/cpp     # RC4 encryption algorithm
│   ├── Validator.h/cpp     # Timestamp and format validation
│   ├── FileProcessor.h/cpp # File processing coordination
│   ├── cli.cpp             # Command line interface
│   └── main.cpp            # GUI entry point
├── tests/            # Unit tests
├── docs/             # Documentation
├── .github/workflows/ # CI/CD workflows
└── .vscode/          # VS Code configuration
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Version History

- **v3.3** - GitHub Actions CI/CD for Linux/macOS/Windows, CLI positional argument support
- **v3.2** - Added command line interface tool
- **v3.1** - Centered status bar messages, code cleanup
- **v3.0** - Progress bar moved below buttons
- **v2.9** - Restored default Fusion style
- **v2.8** - Classic Windows style (removed rounded corners)
- **v2.7** - Added rounded corners styling
- **v2.6** - Buttons evenly distributed
- **v2.5** - Applied Fusion style
- **v2.4** - Converted all code to English
- **v2.3** - Improved encrypted file detection
- **v2.2** - Fixed progress bar reset on error
- **v2.1** - Fixed status bar layout
- **v2.0** - Chunk-style progress bar with dynamic width
- **v1.9** - Progress bar in status bar
- **v1.8** - Fixed status clearing when browsing
- **v1.7** - Status bar added, removed popup messages

## License

MIT

## Author

Created with ❤️ using Qt6 and C++17
