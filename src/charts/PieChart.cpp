#include "PieChart.h"

osg::ref_ptr<osg::Geode> PieChart::generatePieChart()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    int num[5] = { 0 };
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        num[i] = rand() % 100 + 10;
        sum += num[i];
    }

    int leiji = 0;

    for (int p = 0; p < 5; p++) {

        //生成一个随机的颜色
        float r = rand() % 100 / 100.f;
        float g = rand() % 100 / 100.f;
        float b = rand() % 100 / 100.f;
        osg::Vec4 color = osg::Vec4(r, g, b, 0.7f);
      

        int perDegree = round(num[p] * 360 / sum);
        float x0 = 10.0, y0 = 10.0, height = rand() % 7, raduis = 7.0;
        int startDegree = leiji, endDegree = leiji + perDegree;
        leiji += perDegree;
        float prex = x0 + raduis * cos(startDegree * 3.14159 / 180);
        float prey = y0 + raduis * sin(startDegree * 3.14159 / 180);
        float endx = x0 + raduis * cos(endDegree * 3.14159 / 180);
        float endy = y0 + raduis * sin(endDegree * 3.14159 / 180);

        osg::ref_ptr<osg::Geometry> ce1 = new osg::Geometry();
        osg::ref_ptr<osg::Vec3Array> ve1 = new osg::Vec3Array();
        ve1->push_back(osg::Vec3(x0, y0, 0));
        ve1->push_back(osg::Vec3(prex, prey, 0));
        ve1->push_back(osg::Vec3(prex, prey, height));
        ve1->push_back(osg::Vec3(x0, y0, height));
        ce1->setVertexArray(ve1);
        osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
        c->push_back(color);
        ce1->setColorArray(c.get());
        ce1->setColorBinding(osg::Geometry::BIND_OVERALL);
        ce1->addPrimitiveSet(
            new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
        geode->addDrawable(ce1);

        osg::ref_ptr<osg::Geometry> ce2 = new osg::Geometry();
        osg::ref_ptr<osg::Vec3Array> ve2 = new osg::Vec3Array();
        ve2->push_back(osg::Vec3(x0, y0, 0));
        ve2->push_back(osg::Vec3(endx, endy, 0));
        ve2->push_back(osg::Vec3(endx, endy, height));
        ve2->push_back(osg::Vec3(x0, y0, height));
        ce2->setVertexArray(ve2);
        osg::ref_ptr<osg::Vec4Array> c2 = new osg::Vec4Array();
        c2->push_back(color);
        ce2->setColorArray(c2.get());
        ce2->setColorBinding(osg::Geometry::BIND_OVERALL);
        ce2->addPrimitiveSet(
            new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
        geode->addDrawable(ce2);

        for (int i = startDegree + 1; i <= endDegree; i++) {
            float degree = i * 3.14159 / 180;
            float x = x0 + raduis * cos(degree);
            float y = y0 + raduis * sin(degree);
            osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
            osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
            v->push_back(osg::Vec3(x0, y0, 0));
            v->push_back(osg::Vec3(prex, prey, 0));
            v->push_back(osg::Vec3(x, y, 0));
            geom->setVertexArray(v);
            geom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));
            geode->addDrawable(geom);
            osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry();
            osg::ref_ptr<osg::Vec3Array> v1 = new osg::Vec3Array();
            v1->push_back(osg::Vec3(x0, y0, height));
            v1->push_back(osg::Vec3(prex, prey, height));
            v1->push_back(osg::Vec3(x, y, height));
            geom1->setVertexArray(v1);
            geom1->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));
            geode->addDrawable(geom1);

            osg::ref_ptr<osg::Geometry> geom2 = new osg::Geometry();
            osg::ref_ptr<osg::Vec3Array> v2 = new osg::Vec3Array();
            v2->push_back(osg::Vec3(x, y, 0));
            v2->push_back(osg::Vec3(prex, prey, 0));
            v2->push_back(osg::Vec3(prex, prey, height));
            v2->push_back(osg::Vec3(x, y, height));
            geom2->setVertexArray(v2);
            geom2->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
            geode->addDrawable(geom2);
            prex = x;
            prey = y;

        }

    }

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    return geode;
}
