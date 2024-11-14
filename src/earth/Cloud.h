#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>

#include <osg/PrimitiveSet>
#include<osg/PolygonMode>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/Depth>


class Cloud {
public:
	Cloud(osg::ref_ptr<osg::Group> _root);
    osg::ref_ptr<osg::Geometry> createFullScreenQuad()
    {
        // 创建一个 Geometry 对象
        osg::ref_ptr<osg::Geometry> quad = new osg::Geometry();

        // 定义全屏顶点坐标
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
        vertices->push_back(osg::Vec3(-1.0f, -1.0f, 0.0f)); // 左下角
        vertices->push_back(osg::Vec3(1.0f, -1.0f, 0.0f)); // 右下角
        vertices->push_back(osg::Vec3(1.0f, 1.0f, 0.0f)); // 右上角
        vertices->push_back(osg::Vec3(-1.0f, 1.0f, 0.0f)); // 左上角

        // 定义四边形的顶点索引
        osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);
        indices->push_back(2);
        indices->push_back(3);
        indices->push_back(0);

        quad->addPrimitiveSet(indices);

        // 设置纹理坐标
        osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
        texCoords->push_back(osg::Vec2(0.0f, 0.0f)); // 左下角
        texCoords->push_back(osg::Vec2(1.0f, 0.0f)); // 右下角
        texCoords->push_back(osg::Vec2(1.0f, 1.0f)); // 右上角
        texCoords->push_back(osg::Vec2(0.0f, 1.0f)); // 左上角

        quad->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
        quad->setVertexAttribArray(1, texCoords.get(), osg::Array::BIND_PER_VERTEX);
        return quad;
    }
};