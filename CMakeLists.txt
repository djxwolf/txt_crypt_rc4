cmake_minimum_required(VERSION 3.16)
project(TxtCryptRC4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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

# Enable testing
enable_testing()
add_subdirectory(tests)
