set(
    SOURCES
    main.cpp
    Banana.cpp
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

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:kong>/assets
)

add_dependencies(kong copy_assets)
