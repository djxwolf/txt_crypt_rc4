# 使用 Qt Test 框架
find_package(Qt6 REQUIRED COMPONENTS Test)
# Or for Qt5:
# find_package(Qt5 REQUIRED COMPONENTS Test)

set(TEST_SOURCES
    test_rc4.cpp
)

add_executable(txt_crypt_rc4_tests
    ${TEST_SOURCES}
    ../src/RC4Cipher.cpp
)

target_link_libraries(txt_crypt_rc4_tests
    Qt6::Test
    Qt6::Core
)

# Add tests to CTest
add_test(NAME RC4CipherTest COMMAND txt_crypt_rc4_tests)
