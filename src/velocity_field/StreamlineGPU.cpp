//
// Created by pcd on 2024/9/16.
//
#include "StreamlineGPU.h"
#include "StreamLineMainWindow.h"

using namespace VelocityFieldGPU;

StreamLineGPU * VelocityFieldGPU::Generate(osg::Group *root, osg::Camera *camera, osg_3d_vis::llhRange range) {
    // return nullptr;
    // 1. sample points
    // 2. generate wind speed
    // 3. Pos = points + wind speed
    // 4. Randomize points to avoid degeneration
    // 5. transform llh2xyz & coloring
    // 6. update every frame
    // camera->getGraphicsContext()->getState()->setUseModelViewAndProjectionUniforms(true);
    srand(time(NULL));
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    VelocityFieldGPU::StreamLineGPU* sl = new VelocityFieldGPU::StreamLineGPU;
    sl->setGeode(geode);
    sl->setRoot(root);
    sl->setMainCamera(camera);
    sl->setLlhRange(range);
#if defined(_WIN32)
    sl->setShaderPath(std::string(OSG_3D_VIS_SHADER_PREFIX));
    sl->initFromDatFile(std::string(OSG_3D_VIS_DATA_PREFIX) + "U.DAT", std::string(OSG_3D_VIS_DATA_PREFIX)+"V.DAT", range);
    //sl->initFromConfigFile("E:/osg-vis/config");
#else
    sl->setShaderPath("/home/user/temp/gis3/kylin64_desk_arm_ft_g++_private/bin/gtconfig/modules/gis3_ztbh/Mgs3D_Data/MoeData/spaceDivide/Streamline/shaders/");
    sl->initFromDatFile("/home/user/temp/gis3/kylin64_desk_arm_ft_g++_private/bin/gtconfig/modules/gis3_ztbh/Mgs3D_Data/MoeData/spaceDivide/Streamline/data/U.DAT",
                        "/home/user/temp/gis3/kylin64_desk_arm_ft_g++_private/bin/gtconfig/modules/gis3_ztbh/Mgs3D_Data/MoeData/spaceDivide/Streamline/data/V.DAT", range);
#endif

    sl->createCalculateSpeedPass();
    sl->createUpdatePositionPass();
    sl->createDrawLinesPass();
    //sl->createDrawArrowsPass();
    sl->createCopyCurrentParticleToPrevParticlePass();
    //      sl->createTestPass();
    sl->createCopyNextParticleToCurrentParticlePass();
    sl->createBlendPass();
    sl->createCopyblendTexToLastFrameTexPass();

    sl->createRenderPass();

    root->addChild(geode.get());


    // ui connection
    auto uiWindow = new StreamLineMainWindow();
    uiWindow->SetStreamLine(sl);
    uiWindow->show();

    return sl;
}
