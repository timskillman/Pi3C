add_library(pico_graphics 
    ${CMAKE_CURRENT_LIST_DIR}/types.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics.cpp
)

target_include_directories(pico_graphics INTERFACE ${CMAKE_CURRENT_LIST_DIR})

