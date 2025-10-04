cmake_minimum_required(VERSION 3.16)
project(LibraryManager)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(LibraryManager
    main.cpp
    LoginDialog.cpp
    LoginDialog.h
    MainWindow.cpp
    MainWindow.h
    RegisterDialog.cpp
    RegisterDialog.h
    ProfileDialog.cpp
    ProfileDialog.h
)

target_link_libraries(LibraryManager PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Убедитесь, что MOC обрабатывается правильно
set_target_properties(LibraryManager PROPERTIES
    AUTOMOC ON
)
