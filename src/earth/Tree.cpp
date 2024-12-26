//
// Created by pcd on 2024/11/6.
//

//
// Created by pcd on 2024/9/29.
//

#include "Tree.h"

#include <osg/BlendFunc>
#include <osg/PrimitiveSet>

#include <osgDB/ReadFile>
#include "../globals.h"

#include "../util.h"
#include "osg/PositionAttitudeTransform"

const std::string treeOBJ = std::string(OSG_3D_VIS_DATA_PREFIX) + "gpu-instances/trees9.obj";
const std::string vs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/tree.vert";
const std::string fs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/tree.frag";

namespace osg_3d_vis {


    Tree::Tree(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera)
        :root(_root),viewerCamera(_camera)
    {
        Geos = new osg::Geode;
        osg::ref_ptr<osg::Node> objModel = osgDB::readNodeFile(treeOBJ.c_str());



        if (!objModel) {
            std::cerr << "Failed to load model.obj" << std::endl;
            exit(1);
        }

        GeometryExtractor geoExtractor;
        objModel->accept(geoExtractor);

        for(int i=0;i<1; ++i) {
            CreareSingleGeode(geoExtractor,i);
        }


        setShader();
        setUniforms();

        auto  calculateOrientation = [&](const osg::Vec3& position) {
            osg::Vec3 normal = position;
            normal.normalize();

            osg::Vec3 up(0, 0, 1);

            osg::Quat quat;
            quat.makeRotate(up, normal); 

            return quat;
        };
        float step = 0.1;
        int p = 5;

        for(int i=-p; i<p; ++i)
        {
	        for(int j=-p; j<p; ++j)
	        {
                osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
                osg::Vec3d pos;
                llh2xyz_Ellipsoid(osg::Vec3d(osg::DegreesToRadians(30+step*i), osg::DegreesToRadians(300+step*j), 200000), pos.x(), pos.y(), pos.z());
                transform->setPosition(pos);
                transform->setScale({ 20000,20000,20000 });
                transform->setAttitude(calculateOrientation(pos));
                transform->addChild(Geos);
                std::cout << pos.x()<<' '<<pos.y()<<' '<<pos.z()<<std::endl;
                root->addChild(transform);
	        }
        }
     }

    void Tree::setShader() {
        osg::ref_ptr<osg::StateSet> stateset = Geos->getOrCreateStateSet();

        // 创建着色器程序
        osg::ref_ptr<osg::Program> shaderProgram = new osg::Program;

        osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
        osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);

        vertexShader->loadShaderSourceFromFile(vs);
        fragmentShader->loadShaderSourceFromFile(fs);

        shaderProgram->addShader(vertexShader.get());
        shaderProgram->addShader(fragmentShader.get());

        shaderProgram->addBindAttribLocation("Position", 0);
        shaderProgram->addBindAttribLocation("TexCoord", 1);


        Geos->getOrCreateStateSet()->setAttributeAndModes(shaderProgram, osg::StateAttribute::ON);

    }

    void Tree::setUniforms() {
        osg::ref_ptr<osg::StateSet> stateset = Geos->getOrCreateStateSet();

        // views
        auto modelUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "model");
        modelUniform->setUpdateCallback(new osg_3d_vis::ModelMatrixCallback(viewerCamera));
        auto viewUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "view");
        viewUniform->setUpdateCallback(new osg_3d_vis::ViewMatrixCallback(viewerCamera));
        auto projUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "proj");
        projUniform->setUpdateCallback(new osg_3d_vis::ProjectionMatrixCallback(viewerCamera));

        stateset->addUniform(modelUniform);
        stateset->addUniform(viewUniform);
        stateset->addUniform(projUniform);

        // time & wind
        auto timerUniform = new osg::Uniform(osg::Uniform::FLOAT, "time");
        timerUniform->setUpdateCallback(new osg_3d_vis::TimerCallback());
        stateset->addUniform(timerUniform);
        stateset->addUniform(new osg::Uniform("windDirection", osg::Vec3(1.0f, 0.0f, 1.0f)));

		
    }


    void Tree::CreareSingleGeode(const GeometryExtractor &data, int index) {
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexAttribArray(0, data.geometries[index].vertices, osg::Array::BIND_PER_VERTEX);
        geom->setVertexAttribArray(1, data.geometries[index].uvs, osg::Array::BIND_PER_VERTEX);
        auto Primitives = data.geometries[index].indices;
        // key of instance draw 
    	//geom->setUseDisplayList(false);
     //   Primitives->setNumInstances(instanceCount);
        geom->addPrimitiveSet(Primitives);


        // samplers
        osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0, data.geometries[index].textures[0], osg::StateAttribute::ON);
        Geos->addDrawable(geom);
    }

}
