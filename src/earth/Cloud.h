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
        // ����һ�� Geometry ����
        osg::ref_ptr<osg::Geometry> quad = new osg::Geometry();

        // ����ȫ����������
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
        vertices->push_back(osg::Vec3(-1.0f, -1.0f, 0.0f)); // ���½�
        vertices->push_back(osg::Vec3(1.0f, -1.0f, 0.0f)); // ���½�
        vertices->push_back(osg::Vec3(1.0f, 1.0f, 0.0f)); // ���Ͻ�
        vertices->push_back(osg::Vec3(-1.0f, 1.0f, 0.0f)); // ���Ͻ�

        // �����ı��εĶ�������
        osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);
        indices->push_back(2);
        indices->push_back(3);
        indices->push_back(0);

        quad->addPrimitiveSet(indices);

        // ������������
        osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
        texCoords->push_back(osg::Vec2(0.0f, 0.0f)); // ���½�
        texCoords->push_back(osg::Vec2(1.0f, 0.0f)); // ���½�
        texCoords->push_back(osg::Vec2(1.0f, 1.0f)); // ���Ͻ�
        texCoords->push_back(osg::Vec2(0.0f, 1.0f)); // ���Ͻ�

        quad->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
        quad->setVertexAttribArray(1, texCoords.get(), osg::Array::BIND_PER_VERTEX);
        return quad;
    }
};