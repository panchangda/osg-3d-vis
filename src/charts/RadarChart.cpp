#include "RadarChart.h"

osg::ref_ptr<osg::Geode> RadarChart::generateRadar()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    for (int j = 1; j <= 5; j++) {
        osg::ref_ptr<osg::Geometry> clockGeometry = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> allPoints = new osg::Vec3Array;
        //
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        for (double i = 0.0; i < 6.28; i += 0.02) {
            colors->push_back(osg::Vec4f(1, 1, 1, 1.0));
            allPoints->push_back(osg::Vec3(4 * j * sin(i), -0.0, 4 * j * cos(i)));
        }
        clockGeometry->setVertexArray(allPoints);
        clockGeometry->setColorArray(colors);
        clockGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        clockGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, allPoints->size()));
        geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(4.0), osg::StateAttribute::ON);
        geode->addDrawable(clockGeometry.get());
    }
    //
    osg::ref_ptr<osg::Geometry> axisGeometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> axisPoints = new osg::Vec3Array;
    axisPoints->push_back(osg::Vec3(-20, 0, 0));
    axisPoints->push_back(osg::Vec3(20, 0, 0));
    axisPoints->push_back(osg::Vec3(0, 0, 20));
    axisPoints->push_back(osg::Vec3(0, 0, -20));
    axisPoints->push_back(osg::Vec3(20 * sin(3.1415 / 4), 0, 20 * sin(3.1415 / 4)));
    axisPoints->push_back(osg::Vec3(-20 * sin(3.1415 / 4), 0, -20 * sin(3.1415 / 4)));
    axisPoints->push_back(osg::Vec3(-20 * sin(3.1415 / 4), 0, 20 * sin(3.1415 / 4)));
    axisPoints->push_back(osg::Vec3(20 * sin(3.1415 / 4), 0, -20 * sin(3.1415 / 4)));
    axisGeometry->setVertexArray(axisPoints);
    for (int i = 1; i <= 4; i++) {
        axisGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2 * i));
    }
    geode->addDrawable(axisGeometry.get());

    for (int j = 0; j < 2; j++) {
        osg::Vec4 color;
        if (j == 0)
            color = osg::Vec4(0.12, 0.56, 0.67, 0.7f);
        else if (j == 1)
            color = osg::Vec4(0.52, 0.78, 0.17, 0.7f);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

        //generete 
        for (int i = 0; i < 8; i++) {
            float x = 0, y = 0, z;
            float r1 = (rand() % 14 + 1) / 1.0f;
            y = (rand() % 14 + 1) / 1.0f;
            switch (i)
            {
            case 0:x = r1; z = 0; break;
            case 1:x = r1; z = r1; break;
            case 2:x = 0; z = r1; break;
            case 3:x = -r1; z = r1; break;
            case 4:x = -r1; z = 0; break;
            case 5:x = -r1; z = -r1; break;
            case 6:x = 0; z = -r1; break;
            case 7:x = r1; z = -r1; break;
            default:
                break;
            }


            //��ϸ��
            osg::TessellationHints* hints1 = new osg::TessellationHints();
            //���þ�ϸ��
            hints1->setDetailRatio(1.1f);

            //��������
            osg::Sphere* sphere = new osg::Sphere(osg::Vec3(x, y, z), 0.3f);
            osg::ShapeDrawable* draw1 = new osg::ShapeDrawable(sphere, hints1);
            draw1->setColor(color);
            geode->addDrawable(draw1);

            v->push_back(osg::Vec3(x, y, z));
            c->push_back(color);
        }
        geom->setVertexArray(v);

        geom->setColorArray(c.get());
        geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, v->size()));
        geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(4.0), osg::StateAttribute::ON);
        geode->addDrawable(geom.get());

    }



    return geode;
}
