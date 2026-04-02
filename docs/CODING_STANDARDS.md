# Coding Standards

## Language

**All code, comments, and user-facing text MUST be in English.**

This includes:
- Source code comments
- Variable and function names
- User interface text (buttons, labels, messages)
- Error messages
- Documentation

### Examples

❌ **Incorrect:**
```cpp
// 检查文件是否存在
if (file.exists()) {
    showStatus("文件不存在");
}
```

✅ **Correct:**
```cpp
// Check if file exists
if (file.exists()) {
    showStatus("File does not exist");
}
```

## Code Style

### File Naming
- Header files: `.h` extension (e.g., `MainWindow.h`)
- Source files: `.cpp` extension (e.g., `MainWindow.cpp`)
- Use `PascalCase` for class names (e.g., `FileProcessor`, `RC4Cipher`)
- Use `camelCase` for function names and variables (e.g., `onEncrypt`, `m_inputFileEdit`)

### Member Variables
- Prefix member variables with `m_` (e.g., `m_statusBar`, `m_processor`)

### Comments
- Use `//` for single-line comments
- Keep comments clear and concise
- Comment complex logic, not obvious code
- No unnecessary section dividers (e.g., `// ===== Section =====`)

### Signal/Slot Naming
- Use `on` prefix for slot functions (e.g., `onBrowseInput`, `onStatusChanged`)

### Constants
- Use `UPPER_SNAKE_CASE` for constants (e.g., `MIN_TIMESTAMP`, `MAX_TIMESTAMP`)

### Error Messages
- Be specific and helpful
- Include context when useful

Example:
```cpp
return {false, "Cannot open input file: " + inputFile.errorString(), ""};
```

## Project Architecture

### Dual Interface Design

This project provides both GUI and CLI interfaces:

**GUI Application (`TxtCryptRC4`)**
- Qt Widgets based
- Event-driven architecture
- Progress tracking with signals/slots
- Status bar for user feedback

**CLI Tool (`txt_crypt_cli`)**
- QCoreApplication based
- Command line argument parsing
- Stream-based I/O
- Exit codes for error handling

### Shared Components

Both interfaces use these core modules:

| Component | File(s) | Purpose |
|-----------|----------|---------|
| RC4 Cipher | `RC4Cipher.h/cpp` | Encryption/decryption algorithm |
| Validator | `Validator.h/cpp` | Timestamp validation, format parsing, Base64 validation |
| File Processor | `FileProcessor.h/cpp` | GUI-only: coordinates operations, emits progress signals |

### Core Algorithm

**RC4 Implementation:**
- KSA (Key Scheduling Algorithm) - Initializes permutation
- PRGA (Pseudo-Random Generation Algorithm) - Generates keystream
- Symmetric encryption - encryption and decryption are identical

**File Format:**
```
timestamp@Base64-encoded-ciphertext
```

## Build System

### CMake Configuration

- Minimum CMake version: 3.16
- C++17 standard
- Qt automoc/rcc/uic enabled
- macOS: Includes C++ stdlib path for compatibility

### Executables

Two separate executables are built:

1. **TxtCryptRC4** - GUI application
   - Links: Qt6::Core, Qt6::Widgets
   - Requires: display server

2. **txt_crypt_cli** - CLI tool
   - Links: Qt6::Core only
   - Headless operation

## Commit Messages

Use conventional commit format:

```
<type>: <description>

[optional body]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `refactor`: Code refactoring
- `docs`: Documentation changes
- `chore`: Build/process changes
- `style`: Code style changes (no logic change)
- `revert`: Revert previous change

Examples:
```
feat: add command line interface tool
fix: reset progress bar on encryption failure
docs: update installation instructions
revert: restore default Fusion style
```

## Versioning

- Semantic versioning: MAJOR.MINOR.PATCH
- MAJOR: Breaking changes
- MINOR: New features (backward compatible)
- PATCH: Bug fixes

## Testing

### Unit Tests

Located in `tests/` directory:
- `test_rc4.cpp` - RC4 algorithm tests
- `test_validator.cpp` - Validation logic tests

### Running Tests

```bash
cd build
ctest --output-on-failure

# Or run individual tests
./tests/txt_crypt_rc4_tests
./tests/validator_tests
```

## Platform Support

### macOS
- Developed on macOS
- Uses Homebrew Qt6
- Native look with Fusion style

### Linux
- Should work with Qt6 packages
- May require package manager dependencies

### Windows
- Not tested but should work with MinGW/MSVC Qt6

## Security Considerations

- RC4 is suitable for educational purposes and non-critical data
- Timestamp as key provides basic protection
- Not suitable for high-security applications
- No key derivation function used
