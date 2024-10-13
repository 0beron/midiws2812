cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(leds C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_compile_definitions(RMFPPLAT FASTLED_RP2040_CLOCKLESS_PIO)

pico_sdk_init()

add_executable(leds
        example.cpp
        effect.cpp
        particles.cpp
        keys.cpp
        render.cpp
        beatchaser.cpp
        bargraph.cpp
        pianoroll.cpp
        streb.cpp
        beard.cpp
        globaleffect.cpp
        )

#add_subdirectory(${CMAKE_SOURCE_DIR}/../FastLED ${CMAKE_BINARY_DIR}/FastLED)

# pull in common dependencies
target_link_libraries(leds PUBLIC pico_stdlib PUBLIC hardware_pio PUBLIC hardware_flash PRIVATE picofastled)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(leds)
pico_enable_stdio_uart(leds 1)
#pico_generate_pio_header(leds ${CMAKE_CURRENT_LIST_DIR}/WS2812.pio)

