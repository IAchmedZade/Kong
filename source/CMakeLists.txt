set(
    SOURCES
    main.cpp
)

set(
    HEADERS
)

add_executable(
    kong
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(kong PRIVATE SFML::Graphics)
