#include "CurveChart.h"

#include <osg/LineWidth>

osg::ref_ptr<osg::Geode> CurveChart::generateCurveChart()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
   
    
    for (float t = 0; t < 20; t += 0.001) {
        float x = 10 + (1 + 10 * cos(55 * t)) * cos(t);
        float y = 10 + (1 + 10 * cos(55 * t)) * sin(t);
        float z = t + sin(55 * t);
        v->push_back(osg::Vec3(x, y, z));
        c->push_back(osg::Vec4(x/20.f, y/20.f, z/20.f,0.6f));
    }
    geom->setVertexArray(v);
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, v->size()));
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.0), osg::StateAttribute::ON);
    geode->addDrawable(geom.get());
    return geode;
}
