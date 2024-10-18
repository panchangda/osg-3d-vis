#include "ParallelChart.h"

osg::ref_ptr<osg::Geode> ParallelChart::generateParallel()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    //��
    osg::ref_ptr<osg::Geometry> axisGeometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> axisPoints = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

    for (int j = 1; j <= 5; j++) {
        axisPoints->push_back(osg::Vec3(4 * j, 0, 0));
        axisPoints->push_back(osg::Vec3(4 * j, 0, 21));
        colors->push_back(osg::Vec4(1, 0, 0, 1));
        colors->push_back(osg::Vec4(1, 0, 0, 1));
    }

    axisGeometry->setVertexArray(axisPoints);
    axisGeometry->setColorArray(colors);
    axisGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);


    for (int i = 1; i <= 5; i++) {
        axisGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 2 * (i - 1), 2));
    }
    geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(4.0), osg::StateAttribute::ON);
    geode->addDrawable(axisGeometry.get());

    osg::Vec3 a1 = osg::Vec3(0, 0, 1);
    osg::Vec3 a2 = osg::Vec3(4, 0, 9);
    osg::Vec3 a3 = osg::Vec3(8, 0, 3);
    osg::Vec3 a4 = osg::Vec3(12, 0, 19);
    osg::Vec3 a5 = osg::Vec3(16, 0, 10);
    osg::Vec3 a6 = osg::Vec3(20, 0, 8);

    for (int i = 0; i < 3; i++) {
        osg::Vec4 color;
        if (i == 0)
            color = osg::Vec4(0.12, 0.56, 0.67, 0.7f);
        else if (i == 1)
            color = osg::Vec4(0.52, 0.78, 0.17, 0.7f);
        else if (i == 2)
            color = osg::Vec4(0.82, 0.34, 0.78, 0.7f);
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
        v->push_back(a1 + osg::Vec3(0, 0, i));
        v->push_back(a2 + osg::Vec3(0, 0, i));
        v->push_back(a3 + osg::Vec3(0, 0, i));
        v->push_back(a4 + osg::Vec3(0, 0, i));
        v->push_back(a5 + osg::Vec3(0, 0, i));
        v->push_back(a6 + osg::Vec3(0, 0, i));
        geom->setVertexArray(v);
        c->push_back(color);

        geom->setColorArray(c.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, v->size()));


        geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(4.0), osg::StateAttribute::ON);
        geode->addDrawable(geom.get());



    }




    return geode;
}
