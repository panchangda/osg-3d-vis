#include "PolylineChart.h"

osg::ref_ptr<osg::Geode> PolylineChart::generateLineChart()
{
    float prex, prey, prez;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    for (int j = 0; j < 5; j++) {
        //生成一个随机的颜色
        float r = rand() % 100 / 100.f;
        float g = rand() % 100 / 100.f;
        float b = rand() % 100 / 100.f;
        osg::Vec4 color = osg::Vec4(r, g, b, 0.5f);

        for (int i = 0; i < 10; i++) {
            float x = i * 2;
            float y = j * 4;
            float z = rand() % 20 / 1.f;

            //精细度
            osg::TessellationHints* hints1 = new osg::TessellationHints();
            //设置精细度
            hints1->setDetailRatio(1.1f);

            //创建球体
            osg::Sphere* sphere = new osg::Sphere(osg::Vec3(x, y, z), 0.3f);
            osg::ShapeDrawable* draw1 = new osg::ShapeDrawable(sphere, hints1);
            draw1->setColor(color);
            geode->addDrawable(draw1);

           

            //画线
            if (i > 0) {
                osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
                osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
                v->push_back(osg::Vec3(x, y, z));
                v->push_back(osg::Vec3(prex, prey, prez));
                osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
                c->push_back(color);
                c->push_back(color);
                geom->setColorArray(c.get());
                geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
                geom->setVertexArray(v);
                geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
                geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
                geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(4.0), osg::StateAttribute::ON);
                geode->addDrawable(geom.get());

            }
            prex = x;
            prey = y;
            prez = z;

        }
    }
    return geode;
}
