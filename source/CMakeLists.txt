set(
    SOURCES
    main.cpp
    Level.cpp
)

set(
    HEADERS
    Banana.h
    Game.h
    Level.h
    Player.h
)

add_executable(
    kong
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(kong PRIVATE SFML::Graphics)
