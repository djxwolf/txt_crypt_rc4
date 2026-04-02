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

### Signal/Slot Naming
- Use `on` prefix for slot functions (e.g., `onBrowseInput`, `onStatusChanged`)

### Constants
- Use `UPPER_SNAKE_CASE` for constants (e.g., `MIN_TIMESTAMP`, `MAX_TIMESTAMP`)

### Error Messages
- Be specific and helpful
- Start with lowercase for continuation messages
- Include context when useful

Example:
```cpp
return {false, "Cannot open input file: " + inputFile.errorString(), ""};
```

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

Examples:
```
feat: add progress bar to status bar
fix: reset progress bar on encryption failure
docs: update installation instructions
```
