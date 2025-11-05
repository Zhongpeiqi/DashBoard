cmake_minimum_required(VERSION 3.16)

project(DashBoard VERSION 0.1 LANGUAGES CXX)
#123
# Qt 自动功能
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置FFmpeg的路径（请根据您的实际路径修改）
set(FFMPEG_DIR "D:/Project/qtProject/vlctest/ffmpeg")  # 假设FFmpeg内容放在项目内的ffmpeg文件夹

# 添加FFmpeg头文件路径
include_directories(${FFMPEG_DIR}/include)

# 添加FFmpeg库文件路径
link_directories(${FFMPEG_DIR}/lib)


# 查找 Qt
# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Mqtt Network Widgets Mqtt)
find_package(Qt6 REQUIRED COMPONENTS Core Mqtt Network Widgets Svg Concurrent)
find_package(SDL2 REQUIRED)

# 包含头文件路径（让 #include <wind/wind_rose_widget.h> 可用）
include_directories(${CMAKE_SOURCE_DIR}/include)

# === 正确列出源文件 ===
set(PROJECT_SOURCES
    src/main.cpp
    src/wind_roll/wind_rose_widget.cpp
    include/wind_roll/wind_rose_widget.h
)

# === 可执行文件 ===
qt_add_executable(DashBoard
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    include/wind_roll/wind_gauge.h
    src/wind_roll/wind_gauge.cpp
    include/nav_info/nav_info_widget.h
    src/nav_info/nav_info_widget.cpp
    include/wind_roll/roll_gauge_widget.h
    src/wind_roll/roll_gauge_widget.cpp
    include/nav_info/pitch_gauge_widget.h
    src/nav_info/pitch_gauge_widget.cpp
    include/engine/engine_bar_widget.h
    src/engine/engine_bar_widget.cpp
    include/engine/rudder_gauge_widget.h
    src/engine/rudder_gauge_widget.cpp
    include/engine/port_engine_container.h
    src/engine/port_engine_container.cpp
    include/engine/stbd_engine_container.h
    src/engine/stbd_engine_container.cpp
    include/propulsion/thruster_gauge_widget.h
    src/propulsion/thruster_gauge_widget.cpp
    include/propulsion/speed_log_widget.h
    src/propulsion/speed_log_widget.cpp
    include/propulsion/propulsion_panel_widget.h
    src/propulsion/propulsion_panel_widget.cpp
    include/dashboard.h
    src/dash_board.cpp
    src/backend/mqttclient.cpp
    src/backend/ruddercontroller.cpp
    # src/backend/testmqttclient.cpp
    include/backend/mqttclient.h
    include/backend/ruddercontroller.h
    # include/backend/testmqttclient.h
    include/main_window.h
    src/main_window.cpp
    include/wave_config_dialog.h
    src/wave_config_dialog.cpp
    include/backend/waveconfig.h
    include/backend/ffmpegdecoder.h
    src/backend/ffmpegdecoder.cpp
    include/video/video_player_widget.h
    src/video/video_player_widget.cpp
    include/video/video_player_window.h
    src/video/video_player_window.cpp
    main.qrc

)

# 链接 QtWidgets 库
target_link_libraries(DashBoard PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(DashBoard PUBLIC
    Qt::Core
    Qt::Mqtt
    Qt::Network
    Qt::Concurrent
    SDL2::SDL2
    Qt::Svg
    # 链接FFmpeg核心库（链接器会根据名称找到lib目录下的对应库文件）
    avcodec
    avformat
    avutil
    swscale
)

# 设置属性
set_target_properties(DashBoard PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Qt6 finalize
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DashBoard)
endif()
