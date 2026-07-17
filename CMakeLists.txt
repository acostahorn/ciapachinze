cmake_minimum_required(VERSION 3.16)
project(cirulla LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

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

