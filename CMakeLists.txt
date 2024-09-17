cmake_minimum_required(VERSION 3.18)
project("OSG-3D-VIS")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型（针对单配置生成器，如 Makefile、Ninja）
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


# <dep: Qt>
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找PCL依赖
set(PCL_PATH "E:/Libraries/PCL 1.14.1")
find_package(PCL REQUIRED)
add_definitions(${PCL_DEFINITIONS})

# 查找Qt5依赖
set(QT_PATH "E:/Libraries/Qt/Qt5.12.12/5.12.12/msvc2017_64")
set(CMAKE_PREFIX_PATH ${QT_PATH}/lib/cmake)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

# 设置osg依赖，lib文件夹中自带release版本，debug版本需自行下载
set(OSG_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenSceneGraph-3.6.5-VC2022-64-Release-2023-01"
CACHE PATH "Root of OpenSceneGraph library (Release)")
set(OSG_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenSceneGraph-3.6.5-VC2022-64-Debug-2023-01"
CACHE PATH "Root of OpenSceneGraph library (Debug)")

# 自动链接所有 .lib 静态库
file(GLOB OSG_RELEASE_LIBS "${OSG_RELEASE}/lib/*.lib")
file(GLOB OSG_DEBUG_LIBS "${OSG_DEBUG}/lib/*.lib")


# 设置包含目录
if(NOT CMAKE_CONFIGURATION_TYPES)
	# 单配置生成器（如 Makefile、Ninja）
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(OSG_INC_DIR "${OSG_DEBUG}/include")
		set(OSG_LIB_DIR "${OSG_DEBUG}/lib")
		message(STATUS "Using OSG Debug Version")
	else()
		set(OSG_INC_DIR "${OSG_RELEASE}/include")
		set(OSG_LIB_DIR "${OSG_RELEASE}/lib")
		message(STATUS "Using OSG Release Version")
	endif()
else()
	# 多配置生成器（如 Visual Studio）
	# 假设包含目录在 Debug 和 Release 中相同
	set(OSG_INC_DIR "${OSG_RELEASE}/include")
	message(STATUS "Using OSG Include Directory")
endif()

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
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
file(GLOB RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc)

# 使用source_group组织头文件
source_group("Header Files" FILES ${HEADER_FILES})


# 设置可执行文件
set(TARGET_NAME ${PROJECT_NAME})
# 可执行文件夹输出路径
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
	${PCL_LIBRARY_DIRS}
)

# 链接库
if(NOT CMAKE_CONFIGURATION_TYPES)
	# 单配置生成器
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(${TARGET_NAME}
				Qt5::Core
				Qt5::Gui
				Qt5::Widgets
				${OSG_DEBUG_LIBS}
				${PCL_LIBRARIES}
		)
	else()
		target_link_libraries(${TARGET_NAME}
				Qt5::Core
				Qt5::Gui
				Qt5::Widgets
				${OSG_RELEASE_LIBS}
				${PCL_LIBRARIES}
		)
	endif()
else()
	# 多配置生成器（如 Visual Studio）
	target_link_libraries(${TARGET_NAME}
			Qt5::Core
			Qt5::Gui
			Qt5::Widgets
			$<$<CONFIG:Debug>:${OSG_DEBUG_LIBS}>
			$<$<CONFIG:Release>:${OSG_RELEASE_LIBS}>
			${PCL_LIBRARIES}
	)
endif()


# 手动指定 osg 的所有 .dll 文件并进行拷贝
file(GLOB_RECURSE OSG_RELEASE_DLLS "${OSG_RELEASE}/bin/*.dll")
file(GLOB_RECURSE OSG_DEBUG_DLLS "${OSG_DEBUG}/bin/*.dll")

if(NOT CMAKE_CONFIGURATION_TYPES)
	# 单配置生成器（如 Makefile、Ninja）
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
				# copy dlls to target folder, to avoid vs error: missing .dll files
				COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
				# 根据构建配置复制 OSG DEBUG 的 DLLs
				COMMAND ${CMAKE_COMMAND} -E copy ${OSG_DEBUG_DLLS} $<TARGET_FILE_DIR:${TARGET_NAME}>
				COMMAND_EXPAND_LISTS
				COMMENT "Copying all osg debug dlls"
		)
	else()
		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
				# copy dlls to target folder, to avoid vs error: missing .dll files
				COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
				# 根据构建配置复制 OSG RELEASE 的 DLLs
				COMMAND ${CMAKE_COMMAND} -E copy ${OSG_RELEASE_DLLS} $<TARGET_FILE_DIR:${TARGET_NAME}>
				COMMAND_EXPAND_LISTS
				COMMENT "Copying all osg release DLLs"
		)
	endif()
else()
	# 多配置生成器（如 Visual Studio）
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
			# copy dlls to target folder, to avoid vs error: missing .dll files
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
			COMMAND_EXPAND_LISTS
			COMMENT "Copying DLLs based on build configuration"
	)
	# 多配置生成器（如 Visual Studio）
	# buggy when copying osg dlls: set local error
#	foreach(dll ${OSG_RELEASE_DLLS})
#		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#				COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND} -E copy ${dll} $<TARGET_FILE_DIR:${TARGET_NAME}>>
#				COMMAND_EXPAND_LISTS
#				COMMENT "Copying Release DLLs"
#		)
#	endforeach()
#	foreach(dll ${OSG_DEBUG_DLLS})
#		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#				COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND} -E copy ${dll} $<TARGET_FILE_DIR:${TARGET_NAME}>>
#				COMMAND_EXPAND_LISTS
#				COMMENT "Copying Debug DLLs"
#		)
#	endforeach()

endif()


