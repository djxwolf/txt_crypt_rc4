cmake_minimum_required(VERSION 3.16)
project(TxtCryptRC4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add C++ standard library include path for macOS
if(APPLE)
    include_directories(SYSTEM /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1)
endif()

# Find Qt packages
# Try Qt6 first, then Qt5
find_package(Qt6 QUIET COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 QUIET COMPONENTS Core Widgets)
endif()

# Provide helpful message if Qt is not found
if(NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(WARNING "Qt6 or Qt5 not found. Please install Qt to build this project.")
    message(WARNING "On macOS: brew install qt@5")
    message(WARNING "Then set CMAKE_PREFIX_PATH: -DCMAKE_PREFIX_PATH=/usr/local/opt/qt@5")
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/RC4Cipher.cpp
    src/FileProcessor.cpp
    src/Validator.cpp
)

set(HEADERS
    src/MainWindow.h
    src/RC4Cipher.h
    src/FileProcessor.h
    src/Validator.h
)

# GUI Application
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Widgets
    )
elseif(Qt5_FOUND)
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
    )
else()
    message(FATAL_ERROR "Neither Qt6 nor Qt5 was found. Please install Qt to continue.")
endif()

# Command Line Interface Tool
add_executable(txt_crypt_cli
    src/cli.cpp
    src/RC4Cipher.cpp
    src/Validator.cpp
)

if(Qt6_FOUND)
    target_link_libraries(txt_crypt_cli Qt6::Core)
elseif(Qt5_FOUND)
    target_link_libraries(txt_crypt_cli Qt5::Core)
else()
    message(FATAL_ERROR "Neither Qt6 nor Qt5 was found. Please install Qt to continue.")
endif()

# Enable testing
enable_testing()
add_subdirectory(tests)
