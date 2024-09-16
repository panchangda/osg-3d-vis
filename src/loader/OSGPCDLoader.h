#ifndef OSGPCDLOADER_H_
#define OSGPCDLOADER_H_

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <iostream>
#include <filesystem>
#include <string>

#include <osg/Geometry>
#include <osg/Geode>

class OSGPCDLoader {
public:
    osg::ref_ptr<osg::Geode>  geode;
    int pointsCounter;
    
    
    ~OSGPCDLoader() {}
    OSGPCDLoader(): geode(new osg::Geode()), pointsCounter(0) {  }

    void LoadFromFolder(std::string folderPath) {
        osg::ref_ptr<osg::Geode> geode(new osg::Geode());
        std::filesystem::directory_entry entry(folderPath);
        if (!entry.is_directory()) {
            std::cout << folderPath << " is NOT a directory!" << std::endl;
            return;
        }
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".pcd") {
                this->LoadFromFileXYZRGB(entry.path().string());
            }
        }
        std::cout << "Loaded " << pointsCounter << " data points from folder" << folderPath << std::endl;

    }
    void LoadFromFileXYZRGB(std::string pcd_file) {
        pcl::PointCloud<pcl::PointXYZRGB> cloud(*new pcl::PointCloud<pcl::PointXYZRGB>);

        if (pcl::io::loadPCDFile<pcl::PointXYZRGB>(pcd_file, cloud) == -1)  // load the file
        {
            std::cerr << "Couldn't read file " << pcd_file << std::endl;
        }
        else {
            std::cout << "Loaded " << cloud.width * cloud.height << " data points from " << pcd_file << std::endl;
            pointsCounter += cloud.width * cloud.height;

            osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry());

            osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());
            osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array());

            for (int i = 0; i < cloud.points.size(); i++) {
                vertices->push_back(osg::Vec3(cloud.points[i].x, cloud.points[i].y, cloud.points[i].z));
                uint32_t rgb_val_;
                memcpy(&rgb_val_, &(cloud.points[i].rgb), sizeof(uint32_t));

                uint32_t red, green, blue;
                blue = rgb_val_ & 0x000000ff;
                rgb_val_ = rgb_val_ >> 8;
                green = rgb_val_ & 0x000000ff;
                rgb_val_ = rgb_val_ >> 8;
                red = rgb_val_ & 0x000000ff;

                colors->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
                if (blue == 0 && red == 0 && green == 0) {
                    colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
                }

            }

            geometry->setVertexArray(vertices.get());
            geometry->setColorArray(colors.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

            geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));

            geode->addDrawable(geometry.get());
            osg::StateSet* state = geometry->getOrCreateStateSet();
            state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        }
    }
    void LoadFromFileXYZI(std::string pcd_file) {
        pcl::PointCloud<pcl::PointXYZI> cloud(*new pcl::PointCloud<pcl::PointXYZI>);;

        if (pcl::io::loadPCDFile<pcl::PointXYZI>(pcd_file, cloud) == -1)  // load the file
        {
            std::cerr << "Couldn't read file " << pcd_file << std::endl;
        }
        else {
            std::cout << "Loaded " << cloud.width * cloud.height << " data points from " << pcd_file << std::endl;
            pointsCounter += cloud.width * cloud.height;

            osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry());

            osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());
            osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array());

            for (int i = 0; i < cloud.points.size(); i++) {
                vertices->push_back(osg::Vec3(cloud.points[i].x, cloud.points[i].y, cloud.points[i].z));
                float intensity = cloud.points[i].intensity;
                colors->push_back(osg::Vec4f(intensity, intensity, intensity, 1.0));
            }

            geometry->setVertexArray(vertices.get());
            geometry->setColorArray(colors.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

            geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));

            geode->addDrawable(geometry.get());
            osg::StateSet* state = geometry->getOrCreateStateSet();
            state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        }

    }




  

};
#endif