cmake_minimum_required(VERSION 3.16)
project(cirulla LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
   
    set(Qt6_DIR "C:/Qt/6.11.1/mingw_64/lib/cmake/Qt6")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)


add_executable(cirulla
    main.cpp
    cirulla.cpp
    cirulla.h
    cirulla.ui
    resources.qrc
    GameTypes.h
    homeScreen.h
    homeScreen.cpp
    CharacterManager.h
    CharacterManager.cpp
    hallOfFameDialog.h
    hallOfFameDialog.cpp
    RegoleDialog.h
    RegoleDialog.cpp
    HeadlessRunner.h
    HeadlessRunner.cpp
    )

target_link_libraries(cirulla PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)




# Forza la directory di base per le risorse a essere la cartella del progetto
set(CMAKE_AUTORCC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")

set_source_files_properties(resources.qrc PROPERTIES QT_RESOURCE_PREFIX "/")

target_link_libraries(cirulla PRIVATE Qt6::Widgets)

