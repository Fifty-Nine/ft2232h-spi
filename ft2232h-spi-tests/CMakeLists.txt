add_executable(
    ft2232h-spi-tests
    test-main.cpp
    packet-tests.cpp
)

target_link_libraries(
    ft2232h-spi-tests
    ft2232h-spi
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(
    NAME ft2232h-spi-tests
    COMMAND ft2232h-spi-tests
)
