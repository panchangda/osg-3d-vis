# Visualization on OpenSceneGraph
## To-Dos
- [x] Streamline 
- [x] LIC
- [x] Vortex Detection
- [x] Radar 
- [x] OSGB Load 
- [x] Point Cloud Load(PCL) 
- [ ] 3D Charts
- [ ] 
## Env
### Downloads
1. Osg Windows Binary Download:
https://objexx.com/OpenSceneGraph.html

2. Qt Download
https://download.qt.io/archive/qt/

3. Qt Install
install Component: MSVC(64bit), Source

4. PCL Download & Install
https://github.com/PointCloudLibrary/pcl/releases

### Configure
Change Env in CMakeLists.txt
* set PCL_PATH & QT5_PATH
```
# 查找PCL依赖
set(PCL_PATH "E:/Libraries/PCL 1.14.1")
find_package(PCL REQUIRED)
add_definitions(${PCL_DEFINITIONS})

# 查找Qt5依赖
set(QT_PATH "E:/Libraries/Qt/Qt5.12.12/5.12.12/msvc2017_64") 
set(CMAKE_PREFIX_PATH ${QT_PATH}/lib/cmake)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
```


### Build & Run
1. `mkdir build && cd build && cmake .. && make` (use `-DCMAKE_BUILD_TYPE=Debug` to use debug version)
2. Open `OSG-3D-VIS.sln`, Generate `OSG-3D-VIS`
3. Copy All Required .dlls to `bin/Debug/` or `bin/Release/` 
4. Run

### Dev