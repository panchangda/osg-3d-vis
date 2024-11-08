//
// Created by pcd on 2024/11/6.
//

#ifndef TREE_H
#define TREE_H

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/NodeVisitor>     // For osg::NodeVisitor
#include <osg/Geode>           // For osg::Geode
#include <osg/Geometry>        // For osg::Geometry and related types
#include <osg/Texture>         // For osg::Texture
#include <osg/StateSet>        // For osg::StateSet

#include <iostream>
namespace osg_3d_vis {

struct GeometryData {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;  // Vertex positions as Vec3Array
    osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;       // Texture coordinates as Vec2Array
    std::vector<osg::ref_ptr<osg::Texture>> textures;            // Textures
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES); // Index array
};

class GeometryExtractor : public osg::NodeVisitor {
public:
    std::vector<GeometryData> geometries;

    GeometryExtractor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    virtual void apply(osg::Geode& geode) {
        for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
            osg::ref_ptr<osg::Geometry> geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry) {
                GeometryData data;

                // Get vertex array
                osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                if (vertices) {
                    data.vertices->insert(data.vertices->end(), vertices->begin(), vertices->end());
                }

                // Get texture coordinate array (only handling the first UV set)
                if (geometry->getNumTexCoordArrays() > 0) {
                    osg::ref_ptr<osg::Vec2Array> uvs = dynamic_cast<osg::Vec2Array*>(geometry->getTexCoordArray(0));
                    if (uvs) {
                        data.uvs->insert(data.uvs->end(), uvs->begin(), uvs->end());
                    }
                }

                // Extract index array from each primitive set
                for (unsigned int p = 0; p < geometry->getNumPrimitiveSets(); ++p) {
                    osg::PrimitiveSet* primitiveSet = geometry->getPrimitiveSet(p);
                    osg::DrawElementsUInt* drawElements = dynamic_cast<osg::DrawElementsUInt*>(primitiveSet);

                    if (drawElements) {
                        data.indices->insert(data.indices->end(), drawElements->begin(), drawElements->end());
                    }
                }

                // Check for and extract textures on all texture units
                osg::StateSet* stateSet = geometry->getStateSet();
                if (stateSet) {
                    for (unsigned int unit = 0; unit < 16; ++unit) { // Assuming up to 16 texture units
                        osg::Texture* texture = dynamic_cast<osg::Texture*>(stateSet->getTextureAttribute(unit, osg::StateAttribute::TEXTURE));
                        if (texture) {
                            data.textures.push_back(texture);
                            std::cout << "Found texture on unit " << unit << std::endl;
                        }
                    }
                }


                // 将数据存入 geometries 向量
                geometries.push_back(data);
            }
        }
        traverse(geode);
    }
};

    class Tree {
    public:
        explicit Tree(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera);
        void createInstancePos() {
            instancePos = std::vector<osg::Vec3>(instanceCount, osg::Vec3d());
            int X = std::pow(instanceCount, 0.5);
            int Y = std::pow(instanceCount, 0.5);

            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    instancePos[i*X + j] = osg::Vec3( 0,(i+j)*20,0);
                }
            }
        }
        void setShader();
        void setUniforms();
        void CreareSingleGeode(const GeometryExtractor& data, int index);
      private:
        osg::ref_ptr<osg::Geode> Geos;
        osg::ref_ptr<osg::Camera> viewerCamera;
        osg::ref_ptr<osg::Group> root;
        osg::ref_ptr<osg::Geode> quads;
        const int instanceCount = 100;
        std::vector<osg::Vec3> instancePos;

    };


}


#endif //TREE_H
