cmake_minimum_required(VERSION 2.8)

project(bar)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(X11 REQUIRED x11)


include_directories(. skb ${X11_INCLUDE_DIR})

add_executable(bar main.c helpers.c skb/skb.c)

TARGET_LINK_LIBRARIES(bar ${X11_LIBRARIES})

install(TARGETS bar DESTINATION ~/.i3)
