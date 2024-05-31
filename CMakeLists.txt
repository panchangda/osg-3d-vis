cmake_minimum_required(VERSION 3.18)
project("OSG-3D-VIS")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# <dep: Qt>
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找PCL依赖
<<<<<<< HEAD
set(PCL_PATH "C:/Program Files/PCL 1.14.1")
=======
set(PCL_PATH "F:/PCL 1.14.1")
>>>>>>> 996a6bb019c466a2dbbc2647e1b9620260442318
find_package(PCL REQUIRED)
add_definitions(${PCL_DEFINITIONS})

# 查找Qt5依赖
<<<<<<< HEAD
set(QT_PATH "C:/Qt/Qt5.12.12/5.12.12/msvc2017_64") 
=======
set(QT_PATH "F:/QT/5.12.12/msvc2017_64") 
>>>>>>> 996a6bb019c466a2dbbc2647e1b9620260442318
set(CMAKE_PREFIX_PATH ${QT_PATH}/lib/cmake)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

# 设置osg依赖，lib文件夹中自带release版本，debug版本需自行下载
set(OSG_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenSceneGraph-3.6.5-VC2022-64-Release-2023-01"
CACHE PATH "Root of OpenSceneGraph library (Release)")
set(OSG_DEBUG "D:/OpenSceneGraph-3.6.5-VC2022-64-Debug"
CACHE PATH "Root of OpenSceneGraph library (Debug)")

list(
	APPEND
	OSG_LIBS
	"osg"
	"osgText"
	"osgViewer"
	"osgDB"
	"osgGA"
	"OpenThreads"
    "osgUtil"
)

# if ((${CMAKE_BUILD_TYPE} AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR
# 	${CMAKE_CONFIGURATION_TYPES} STREQUAL "Debug"
# 	)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	set(OSG_INC_DIR "${OSG_DEBUG}/include")
	set(OSG_LIB_DIR "${OSG_DEBUG}/lib")

	foreach(OSG_LIB ${OSG_LIBS})
		list(APPEND NEW_LIBS "${OSG_LIB}d")
	endforeach()
	set(OSG_LIBS ${NEW_LIBS})
    message(STATUS "Using OSG Debug Version ")

else()
	set(OSG_INC_DIR "${OSG_RELEASE}/include")
	set(OSG_LIB_DIR "${OSG_RELEASE}/lib")
    message(STATUS "Using OSG Release Version ")
endif()
message(STATUS "OSG_LIBS: ${OSG_LIBS}")

# </dep: OSG>

# add definitions:
add_compile_definitions(
	OSG_3D_VIS_SHADER_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}/shader/"
)
add_compile_definitions(
	OSG_3D_VIS_DATA_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)
add_compile_definitions(
	OSG_3D_VIS_CONFIG_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}/config/"
)

# qt .h & .ui
file(GLOB GUI_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.h")
file(GLOB GUI_UI "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.ui")
# C++ .h & .cpp
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc)

# 使用source_group组织头文件
source_group("Header Files" FILES ${HEADER_FILES})


# 设置可执行文件
set(TARGET_NAME ${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
add_executable(${TARGET_NAME} 
    ${SOURCE_FILES} ${HEADER_FILES} 
    ${RESOURCE_FILE} 
    # ${GUI_HEADER} ${GUI_UI}
)

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${OSG_INC_DIR}
	${PCL_INCLUDE_DIRS}
)

target_link_directories(${TARGET_NAME}
	PRIVATE
	${OSG_LIB_DIR}
	&{PCL_LIBRARY_DIRS}
)

# 链接库
target_link_libraries(${TARGET_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${OSG_LIBS}
	${PCL_LIBRARIES}
)
