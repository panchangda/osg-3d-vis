#include "DotChart.h"


float DotChart::generateRandom() {
    float random;
    random = rand() % 100 / 100.f;
    while (random < 0.2) random = rand() % 100 / 100.f;
    return random;
}



osg::ShapeDrawable* DotChart::createShpe(osg::Vec3 location, float radius, osg::Vec4 color)
{

    //精细度
    osg::TessellationHints* hints1 = new osg::TessellationHints();
    //设置精细度
    hints1->setDetailRatio(1.1f);

    //创建球体
    osg::Sphere* sphere = new osg::Sphere(location, radius);
    osg::ShapeDrawable* draw1 = new osg::ShapeDrawable(sphere, hints1);
    draw1->setColor(color);

    //geode->addDrawable(draw1);

    return draw1;
}



osg::ref_ptr<osg::Geode> DotChart::generateDot() {

    osg::Geode* geode = new osg::Geode();

    for (int i = 0; i < 40; i++) {
        float f1 = rand() % 20 / 1.f;
        float f2 = rand() % 20 / 1.f;
        float f3 = rand() % 20 / 1.f;
        geode->addDrawable(createShpe(osg::Vec3(f1, f2, f3), generateRandom(), osg::Vec4(f1 / 20.f, f2 / 20.f, f3 / 20.f,0.6f)));
    }
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    return geode;
}

