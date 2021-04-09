cmake_minimum_required(VERSION "3.20.1")


project("cpp_task")
include_directories("nlohmann")

add_executable("${PROJECT_NAME}" "main.cpp")
set(MY_URL "https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp")
set(MY_DOWNLOAD_PATH "json.hpp")

file(DOWNLOAD "${MY_URL}" "${MY_DOWNLOAD_PATH}")


install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
install(FILES "main.cpp" DESTINATION src)
 