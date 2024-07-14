project(tests)

file(GLOB SOURCE_FILE
        src/main.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILE})