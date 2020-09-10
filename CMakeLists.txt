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


find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

file(GLOB TS_FILES "res/ts/*.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt5_add_resources(res_rcc res/res.qrc)


if(WIN32)

#  set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra")

  add_executable(shiftpicdate-gui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    fsdialog.ui
    fsdialog.cpp
    fsdialog.h
    shiftpicdate_win32.cpp
    shiftpicdate_win32.h
    data.h
    ${QM_FILES}
    ${res_rcc}
    ${EXIV2_FILES}
  )
endif(WIN32)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra")
    #set(CMAKE_CXX_FLAGS_DEBUG "-g")
    #set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    add_executable(shiftpicdate-gui
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        fsdialog.ui
        fsdialog.cpp
        fsdialog.h
        shiftpicdate.cpp
        shiftpicdate.h
        data.h
        ${QM_FILES}
        ${res_rcc}
  )
endif(UNIX)



if (WIN32)
    target_link_libraries(shiftpicdate-gui  PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets )
endif (WIN32)

if (UNIX)
    target_link_libraries(shiftpicdate-gui PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets )
    target_link_libraries(shiftpicdate-gui LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs -lexiv2 -lexif)
endif (UNIX)

