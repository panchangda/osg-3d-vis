//
// Created by pcd on 2024/9/29.
//

#include "Grass.h"

#include <osg/BlendFunc>
#include <osg/PrimitiveSet>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "../util.h"

const std::string grassImage = std::string(OSG_3D_VIS_DATA_PREFIX) + "gpu-instances/grass.png";
const std::string vs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/grass.vert";
const std::string fs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/grass.frag";

namespace osg_3d_vis {


    Grass::Grass(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera)
        :root(_root),viewerCamera(_camera)
    {

        quads = createInstancedQuad(instanceCount);
        // createInstancePos();
        setShader(quads);
        setUniforms(quads);
        setBlend(quads);
        root->addChild(quads);
        root->addChild(createPlane(10.0f, 10.0f));


    }

    osg::ref_ptr<osg::Geode> Grass::createPlane(float width, float height) {
        // 创建顶点数组
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        vertices->push_back(osg::Vec3(-width * 1.0f, 0.0f, -height * 1.0f));  // 左下
        vertices->push_back(osg::Vec3(width * 1.0f, 0.0f, -height * 1.0f));   // 右下
        vertices->push_back(osg::Vec3(width * 1.0f, 0.0f, height * 1.0f));    // 右上
        vertices->push_back(osg::Vec3(-width * 1.0f, 0.0f, height * 1.0f));   // 左上

        // 创建颜色数组
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(.5f, .5f, .5f, 1.0f));  // grey
        // 创建几何对象
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices);

        // 设置颜色
        geom->setColorArray(colors, osg::Array::BIND_OVERALL);

        // 创建四边形的顶点索引
        osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_QUADS);
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);
        indices->push_back(3);
        geom->addPrimitiveSet(indices);

        // 创建法线数组（用于光照）
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));  // 法线指向Y轴
        geom->setNormalArray(normals, osg::Array::BIND_OVERALL);

        // 创建Geode来包含几何对象
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(geom);

        return geode;
    }

    osg::ref_ptr<osg::Geode> Grass::createInstancedQuad(int instanceCount) {
        // 创建四边形顶点
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
        // 1st quad: 原始位置 (垂直朝向)
        v->push_back(osg::Vec3(-0.5f, 0.0f, 0.0f));  // 左下角
        v->push_back(osg::Vec3(0.5f, 0.0f, 0.0f));   // 右下角
        v->push_back(osg::Vec3(0.5f, 1.0f, 0.0f));   // 右上角
        v->push_back(osg::Vec3(-0.5f, 1.0f, 0.0f));  // 左上角

        // 2nd quad: 绕Y轴旋转30度
        {
            float angle = osg::DegreesToRadians(60.0f);
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            v->push_back(osg::Vec3(-0.5f * cosAngle, 0.0f, -0.5f * sinAngle)); // 左下角
            v->push_back(osg::Vec3(0.5f * cosAngle, 0.0f, 0.5f * sinAngle));   // 右下角
            v->push_back(osg::Vec3(0.5f * cosAngle, .8f, 0.5f * sinAngle));   // 右上角
            v->push_back(osg::Vec3(-0.5f * cosAngle, 0.8f, -0.5f * sinAngle)); // 左上角
        }

        // 3rd quad: 绕Y轴旋转-30度
        {
            float angle = osg::DegreesToRadians(-30.0f);
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            v->push_back(osg::Vec3(-0.5f * cosAngle, 0.0f, -0.5f * sinAngle)); // 左下角
            v->push_back(osg::Vec3(0.5f * cosAngle, 0.0f, 0.5f * sinAngle));   // 右下角
            v->push_back(osg::Vec3(0.5f * cosAngle, 1.2f, 0.5f * sinAngle));   // 右上角
            v->push_back(osg::Vec3(-0.5f * cosAngle, 1.2f, -0.5f * sinAngle)); // 左上角
        }

        // 4th quad: 绕Z轴倾斜10度
        {
            float angle = osg::DegreesToRadians(10.0f);
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            v->push_back(osg::Vec3(-0.5f * cosAngle, -0.5f * sinAngle, 0.0f)); // 左下角
            v->push_back(osg::Vec3(0.5f * cosAngle, -0.5f * sinAngle, 0.0f));  // 右下角
            v->push_back(osg::Vec3(0.5f * cosAngle, 0.7f * cosAngle, 0.0f));   // 右上角
            v->push_back(osg::Vec3(-0.5f * cosAngle, 0.7f * cosAngle, 0.0f));  // 左上角
        }

        // screen quad for debug
        // v->push_back(osg::Vec3(-1, -1, 0.0));
        // v->push_back(osg::Vec3(1, -1, 0.0));
        // v->push_back(osg::Vec3(1, 1, 0.0));
        // v->push_back(osg::Vec3(-1, 1, 0.0));

        vt->push_back(osg::Vec2(0, 0));
        vt->push_back(osg::Vec2(1, 0));
        vt->push_back(osg::Vec2(1, 1));
        vt->push_back(osg::Vec2(0, 1));

        vt->push_back(osg::Vec2(0, 0));
        vt->push_back(osg::Vec2(1, 0));
        vt->push_back(osg::Vec2(1, 1));
        vt->push_back(osg::Vec2(0, 1));

        vt->push_back(osg::Vec2(0, 0));
        vt->push_back(osg::Vec2(1, 0));
        vt->push_back(osg::Vec2(1, 1));
        vt->push_back(osg::Vec2(0, 1));

        vt->push_back(osg::Vec2(0, 0));
        vt->push_back(osg::Vec2(1, 0));
        vt->push_back(osg::Vec2(1, 1));
        vt->push_back(osg::Vec2(0, 1));

        geom->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
        geom->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);


        osg::ref_ptr<osg::DrawArrays> quad = new osg::DrawArrays(GL_QUADS, 0, 16);

        // 设置实例数量
        // instance draw must have this line!!!!
        // fuck you god damn son of a bitch mother fucker osg shitsssssssss
        geom->setUseDisplayList( false );
        quad->setNumInstances(instanceCount);
        geom->addPrimitiveSet(quad.get());

        // 创建几何体节点
        osg::ref_ptr<osg::Geode> quadGeode = new osg::Geode;
        quadGeode->addDrawable(geom);

        return quadGeode;
    }

    // instancePOs calculation was moved to gpu in vert shader
    void Grass::createInstancePos() {
        instancePos = std::vector<osg::Vec3>(instanceCount, osg::Vec3d());
        int X = std::pow(instanceCount, 0.5);
        int Y = std::pow(instanceCount, 0.5);

        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                instancePos[i*X + j] = osg::Vec3(0.2f * i, -1.0f, 0.2f * j);
            }
        }
    }

    void Grass::setShader(osg::ref_ptr<osg::Geode> geode) {

        // 创建着色器程序
        osg::ref_ptr<osg::Program> shaderProgram = new osg::Program;

        osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
        osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);

        vertexShader->loadShaderSourceFromFile(vs);
        fragmentShader->loadShaderSourceFromFile(fs);

        shaderProgram->addBindAttribLocation("Position", 0);
        shaderProgram->addBindAttribLocation("TexCoord", 1);

        shaderProgram->addShader(vertexShader.get());
        shaderProgram->addShader(fragmentShader.get());

        geode->getOrCreateStateSet()->setAttributeAndModes(shaderProgram, osg::StateAttribute::ON);
    }

    void Grass::setUniforms(osg::ref_ptr<osg::Geode> geode) {
        // 加载纹理图片
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
        texture->setImage(osgDB::readImageFile(grassImage));
        // samplers
        osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

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

        // instance pos
        // osg::ref_ptr<osg::Uniform> instancePosUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "instancePos", instanceCount);
        // for(int i=0;i<instanceCount;i++) {
        //     instancePosUniform->setElement(i, instancePos[i]);
        // }
        // stateset->addUniform(instancePosUniform);

        int X = std::pow(instanceCount, 0.5);
        int Y = std::pow(instanceCount, 0.5);
        stateset->addUniform(new osg::Uniform("instanceX", X));
        stateset->addUniform(new osg::Uniform("instanceY", Y));
    }

    void Grass::setBlend(osg::ref_ptr<osg::Geode> geode) {
        // 获取状态集
        osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

        // 启用混合模式 (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
        blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        stateSet->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);

        // 启用深度测试，但禁用深度写入
        stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); // 使用透明渲染队列
    }
}