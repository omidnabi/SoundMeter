cmake_minimum_required(VERSION 3.10)

project(SoundMeter)

set(CMAKE_CXX_STANDARD 17)

# Enable automatic handling for UIC, MOC, and RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)

include_directories(include)

add_executable(SoundMeter
    src/main.cpp
    src/MainWindow.cpp
    src/AudioInput.cpp
    src/FrequencyAnalyzer.cpp
    src/FrequencyMeter.cpp  # Ensure FrequencyMeter.cpp is listed here
    src/MainWindow.ui
    src/AudioWorker.cpp
    include/AudioWorker.h
    include/MainWindow.h
    include/AudioInput.h
    include/FrequencyAnalyzer.h
    include/FrequencyMeter.h

)

target_link_libraries(SoundMeter Qt5::Widgets Qt5::Multimedia)

