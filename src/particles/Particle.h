//
// Created by pcd on 2024/9/16.
//

#ifndef EXPLODE_H
#define EXPLODE_H

#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/PrecipitationEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgUtil/Optimizer>

#include "../globals.h"


namespace osg_3d_vs{

    class Particle {
    public:
        Particle(osg::ref_ptr<osg::Group> _root): root(_root) {
            if(osg_3d_vis::drawExplode) {
                root->addChild(createExplode());
            }
            if(osg_3d_vis::drawWeather) {
                root->addChild(createWeather());
            }


            // root->addChild(createFog(false));
        }

        //创建爆炸效果
        static osg::ref_ptr<osg::Node> createExplode();
        // 创建天气
        static osg::ref_ptr<osg::Node> createWeather();
        // 创建云雾
        static osg::ref_ptr<osg::Fog> createFog(bool linear);
    private:
        osg::ref_ptr<osg::Group> root;
    };

}



#endif //EXPLODE_H