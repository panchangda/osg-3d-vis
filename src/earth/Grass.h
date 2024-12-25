//
// Created by pcd on 2024/9/29.
//

#ifndef GRASS_H
#define GRASS_H
#include <osg/ref_ptr>
#include <osg/Group>

namespace osg_3d_vis {
    class Grass {
    public:
        explicit Grass(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera);
        osg::ref_ptr<osg::Geode> createPlane(float width, float height);
        osg::ref_ptr<osg::Geode> createInstancedQuad(int instanceCount);
        void createInstancePos();
        void setShader(osg::ref_ptr<osg::Geode> geode);
        void setUniforms(osg::ref_ptr<osg::Geode> geode);
        void setBlend(osg::ref_ptr<osg::Geode> geode);

    private:
        osg::ref_ptr<osg::Camera> viewerCamera;
        osg::ref_ptr<osg::Group> root;
        osg::ref_ptr<osg::Geode> quads;
        const int instanceCount = 2500;
        std::vector<osg::Matrix> instancePos;

    };


}




#endif //GRASS_H
