
#include "Axis.h"

#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <osgText/Text>

using namespace std;
osg::ref_ptr<osg::Geode> Axis::DrawAxis(float ori_x, float ori_y, float ori_z, float a_x, float a_y, float a_z, float font_size)
{
    osg::ref_ptr<osg::Sphere> pSphereShape = new osg::Sphere(osg::Vec3(ori_x, ori_y, ori_z), 0.3f);
    osg::ref_ptr<osg::ShapeDrawable> pShapeDrawable = new osg::ShapeDrawable(pSphereShape.get());
    pShapeDrawable->setColor(osg::Vec4(0.62, 0.62, 0.62, 0.1f));

    //�������漸����Ϣ�Ķ���
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

    //�����ĸ�����
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    v->push_back(osg::Vec3(ori_x, ori_y, ori_z));
    v->push_back(osg::Vec3(ori_x + a_x, ori_y, ori_z));

    v->push_back(osg::Vec3(ori_x, ori_y, ori_z));
    v->push_back(osg::Vec3(ori_x, ori_y + a_y, ori_z));
    v->push_back(osg::Vec3(ori_x, ori_y, ori_z));

    v->push_back(osg::Vec3(ori_x, ori_y, ori_z + a_z));

    //��������
    for (int i = 1; i <= 10; i++) {
        v->push_back(osg::Vec3(2*i, 0, 0));
        v->push_back(osg::Vec3(2 * i, 21, 0));
        v->push_back(osg::Vec3(2 * i, 0, 0));
        v->push_back(osg::Vec3(2 * i, 0, 21));
    }

    for (int i = 1; i <= 10; i++) {
        v->push_back(osg::Vec3(0, 2 * i, 0));
        v->push_back(osg::Vec3(21, 2 * i, 0));
        v->push_back(osg::Vec3(0, 2 * i, 0));
        v->push_back(osg::Vec3(0, 2 * i, 21));
    }
    for (int i = 1; i <= 10; i++) {
        v->push_back(osg::Vec3(0, 0, 2 * i));
        v->push_back(osg::Vec3(21, 0, 2 * i));
        v->push_back(osg::Vec3(0, 0, 2 * i));
        v->push_back(osg::Vec3(0, 21, 2 * i));
    }



    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    c->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    for (int i = 0; i < 3; i++) {
        for (int j = 1; j <= 10; j++) {
            c->push_back(osg::Vec4(0.62f, 0.625f, 0.628f, 0.05f));
            c->push_back(osg::Vec4(0.62f, 0.625f, 0.628f, 0.05f));
            c->push_back(osg::Vec4(0.62f, 0.625f, 0.628f, 0.05f));
            c->push_back(osg::Vec4(0.62f, 0.625f, 0.628f,0.05f));
        }
    }
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    //xyz
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 2, 2));
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 4, 2));

    for (int i = 0; i < 60; i++) {
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 6 + 2 * i, 2));
    }

    osg::ref_ptr<osgText::Text> pTextXAuxis1 = new osgText::Text;
    pTextXAuxis1->setText(L"X");
    pTextXAuxis1->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.8f));
    // pTextXAuxis1->setFont("Fonts/simhei.ttf");

    pTextXAuxis1->setAxisAlignment(osgText::Text::SCREEN);
    pTextXAuxis1->setCharacterSize(font_size);
    pTextXAuxis1->setPosition(osg::Vec3(ori_x + a_x, ori_y, ori_z));

    osg::ref_ptr<osgText::Text> pTextYAuxis1 = new osgText::Text;
    pTextYAuxis1->setText(L"Y");
    pTextYAuxis1->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.8f));
    //pTextYAuxis1->setFont("Fonts/simhei.ttf");

    pTextYAuxis1->setAxisAlignment(osgText::Text::SCREEN);
    pTextYAuxis1->setCharacterSize(font_size);
    pTextYAuxis1->setPosition(osg::Vec3(ori_x, ori_y + a_y, ori_z));
    osg::ref_ptr<osgText::Text> pTextZAuxis1 = new osgText::Text;
    pTextZAuxis1->setText(L"Z");
    pTextZAuxis1->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.8f));
    //pTextZAuxis1->setFont("Fonts/simhei.ttf");

    pTextZAuxis1->setAxisAlignment(osgText::Text::SCREEN);
    pTextZAuxis1->setCharacterSize(font_size);
    pTextZAuxis1->setPosition(osg::Vec3(ori_x, ori_y, ori_z + a_z));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    for (int i = 1; i <= 10; i++) {
        osg::ref_ptr<osgText::Text> pText = new osgText::Text;
        string str = to_string(2 * i);
        pText->setText(str);
        pText->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        // pTextXAuxis1->setFont("Fonts/simhei.ttf");
        pText->setAxisAlignment(osgText::Text::SCREEN);
        pText->setCharacterSize(0.6);
        pText->setPosition(osg::Vec3(ori_x + 2 * i, ori_y, ori_z));
        geode->addDrawable(pText.get());
    }

    for (int i = 1; i <= 10; i++) {
        osg::ref_ptr<osgText::Text> pText = new osgText::Text;
        string str = to_string(2 * i);
        pText->setText(str);
        pText->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        // pTextXAuxis1->setFont("Fonts/simhei.ttf");
        pText->setAxisAlignment(osgText::Text::SCREEN);
        pText->setCharacterSize(0.6);
        pText->setPosition(osg::Vec3(ori_x, ori_y + 2 * i, ori_z));
        geode->addDrawable(pText.get());
    }

    for (int i = 1; i <= 10; i++) {
        osg::ref_ptr<osgText::Text> pText = new osgText::Text;
        string str = to_string(2 * i);
        pText->setText(str);
        pText->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        // pTextXAuxis1->setFont("Fonts/simhei.ttf");
        pText->setAxisAlignment(osgText::Text::SCREEN);
        pText->setCharacterSize(0.6);
        pText->setPosition(osg::Vec3(ori_x, ori_y, ori_z + 2 * i));
        geode->addDrawable(pText.get());
    }






    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.0), osg::StateAttribute::ON);

    geode->addDrawable(pShapeDrawable.get());
    geode->addDrawable(geom.get());
    geode->addDrawable(pTextXAuxis1.get());

    geode->addDrawable(pTextYAuxis1.get());
    geode->addDrawable(pTextZAuxis1.get());

    return geode;
}