cmake_minimum_required(VERSION 3.0)

project(shiftpicdate-gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

file(GLOB TS_FILES "res/ts/*.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt5_add_resources(res_rcc res/res.qrc)

if(ANDROID)
  add_library(shiftpicdate-gui SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    shiftpicdate.h
    ${QM_FILES}
  )
endif(ANDROID)

if(WIN32)
  add_executable(shiftpicdate-gui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    shiftpicdate_win32.cpp
    shiftpicdate_win32.h
    ${QM_FILES}
    ${res_rcc}
  )
endif(WIN32)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    #set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    add_executable(shiftpicdate-gui
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        shiftpicdate.cpp
        shiftpicdate.h
        ${QM_FILES}
        ${res_rcc}
  )
endif(UNIX)

#find_package( Boost 1.40 COMPONENTS program_options
#                         REQUIRED timer filesystem unit_test_framework)



target_link_libraries(shiftpicdate-gui PRIVATE Qt5::Widgets )

if (UNIX)
    target_link_libraries(shiftpicdate-gui LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs -lexiv2 -lexif)
endif (UNIX)


