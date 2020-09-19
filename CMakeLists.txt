cmake_minimum_required(VERSION 3.0)

project(shiftpicdate-gui LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILER gcc)

if(NOT WIN32)
    if (COMPILER STREQUAL "gcc")
      set(CMAKE_CC_COMPILER "gcc")
      set(CMAKE_CXX_COMPILER "g++")
      message("CC: gcc")
      message("CXX: g++")
    elseif (COMPILER STREQUAL "clang")
      set(CMAKE_CC_COMPILER "clang")
      set(CMAKE_CXX_COMPILER "clang++")
      message("CC: clang")
      message("CXX: clang++")
    elseif (COMPILER STREQUAL "intel") #does not work as usual
      set(INTEL_DIR  /opt/intel/sw_dev_tools/bin)
      set(CMAKE_CC_COMPILER ${INTEL_DIR}/icc)
      set(CMAKE_CXX_COMPILER ${INTEL_DIR}/icpc)
      message("CC: icc")
      message("CXX: icpc")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED QT_VERSION_MAJOR)
    set(QT_VERSION_MAJOR 5)
endif()

if(WIN32)
    find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools Charts Network Winextras REQUIRED)
endif(WIN32)
if(UNIX)
    find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools Charts Network REQUIRED)
endif(UNIX)

file(GLOB TS_FILES "res/ts/*.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt5_add_resources(res_rcc res/res.qrc)

if(WIN32)
  set(OPENSSL_ROOT_DIR C:/Qt/Tools/OpenSSL/Win_x64)
  find_package(OpenSSL)

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
#    set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    find_package(Boost 1.40 OPTIONAL_COMPONENTS filesystem)

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
    set_property(TARGET shiftpicdate-gui PROPERTY WIN32_EXECUTABLE true)
    target_link_libraries(shiftpicdate-gui PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WinExtras OpenSSL::SSL OpenSSL::Crypto)
endif (WIN32)

if (UNIX)
    target_link_libraries(shiftpicdate-gui PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Network)
    target_link_libraries(shiftpicdate-gui LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs -lstdc++fs -lexiv2)
endif (UNIX)

