//
// Created by pcd on 2024/9/16.
//

#include "Particle.h"
#include "../globals.h"

osg::ref_ptr<osg::Node> osg_3d_vs::Particle::createExplode() {
        osg::ref_ptr<osg::Group> explode = new osg::Group();
        // for the earth scale
        constexpr float scale = osg_3d_vis::drawEarth ? 10000000.0f : 1.0f;
        //风向
        osg::Vec3 wind(1.0f, 0.0f, 0.0f);
        osg::Vec3 position(0.0f, 0.0f, -1.0f);
        //爆炸模拟，10.0f为缩放比，默认为1.0f,不缩放
        osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(position, scale);
        osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebri =
            new osgParticle::ExplosionDebrisEffect(position, scale);
        //烟模拟
        osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(position, scale);
        //火焰模拟
        osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, scale);
        //设置风向
        explosion->setWind(wind);
        explosionDebri->setWind(wind);
        smoke->setWind(wind);
        fire->setWind(wind);

        //添加子节点
        explode->addChild(explosion.get());
        explode->addChild(explosionDebri.get());
        explode->addChild(smoke.get());
        explode->addChild(fire.get());
        return explode.get();
}

osg::ref_ptr<osg::Node> osg_3d_vs::Particle::createWeather() {

        //设置雪花类
        osg::ref_ptr<osgParticle::PrecipitationEffect> precipitationEffect = new osgParticle::PrecipitationEffect;

        //设置雪花浓度
        precipitationEffect->snow(0.5);
        // 设置雨
        precipitationEffect->rain(0.5);

        //设置雪花颜色
        precipitationEffect->setParticleColor(osg::Vec4(1, 1, 1, 1));

        //设置风向
        precipitationEffect->setWind(osg::Vec3(2, 0, 0));
        return precipitationEffect.get();

}

osg::ref_ptr<osg::Fog> osg_3d_vs::Particle::createFog(bool linear) {
    // 创建Fog对象
    osg::ref_ptr<osg::Fog> fog = new osg::Fog();

    // 设置颜色
    fog->setColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));

    // 设置浓度
    fog->setDensity(0.01);

    // 设置雾效模式为线性雾
    if (!linear)
    {
        fog->setMode(osg::Fog::LINEAR);
    }
    else// 设置雾效模式为全局零
    {
        fog->setMode(osg::Fog::EXP);
    }

    // 设置雾效近点浓度
    fog->setStart(5.0);

    constexpr float fogEnd = osg_3d_vis::drawEarth ? 10000000.0f : 2000.0f;
    // 设置雾效远点浓度
    fog->setEnd(fogEnd);

    return fog.get();

}
