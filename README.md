# RC4 Text File Encryption Tool

A simple Qt/C++ based text file encryption and decryption tool using the RC4 algorithm.

## Features

- Encrypt/decrypt text files using RC4 algorithm
- Timestamp as encryption key, no additional input required
- Output format: `timestamp@encrypted_string`
- Configurable timeout verification
- In-place mode (directly overwrite original file)
- Graphical user interface

## Building

### Dependencies

- Qt 5.15+ or Qt 6.x
- CMake 3.16+
- C++17 compiler

### Build Steps

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

1. Select input file
2. Select output file (or check In-place to overwrite original file)
3. Set Timeout value (0 or negative means no timeout check)
4. Click "Encrypt" or "Decrypt" button

## Encryption Format

The encrypted file content format is:

```
timestamp@Base64-encoded-ciphertext
```

For example:
```
1712345678@SGVsbG8gV29ybGQ=
```

During decryption, the program automatically extracts the timestamp as the key and verifies if it has timed out.

## License

MIT
