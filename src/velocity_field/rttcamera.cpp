//
// Created by pcd on 2024/10/28.
//

#include "rttcamera.h"
#include "../util.h"
#include <osg/Texture2D>
#include <osg/Depth>


inline 	osg::ref_ptr<osg::Geometry>createScreenQuad() {
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
    v->push_back(osg::Vec3(-1, -1, 0.0));
    v->push_back(osg::Vec3(1, -1, 0.0));
    v->push_back(osg::Vec3(1, 1, 0.0));
    v->push_back(osg::Vec3(-1, 1, 0.0));

    vt->push_back(osg::Vec2(0, 0));
    vt->push_back(osg::Vec2(1, 0));
    vt->push_back(osg::Vec2(1, 1));
    vt->push_back(osg::Vec2(0, 1));
    geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
    geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
    osg::ref_ptr<osg::DrawArrays> quad = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
    geometry->addPrimitiveSet(quad.get());
    return geometry;
}

class DepthCopyCallback : public osg::Camera::DrawCallback {
public:
    DepthCopyCallback(osg::Texture2D* depthTexture) : _depthTexture(depthTexture) {}

    virtual void operator()(osg::RenderInfo& renderInfo) const override {
        // 使用 glCopyTexSubImage2D 复制深度缓冲到深度纹理
        glBindTexture(GL_TEXTURE_2D, _depthTexture->getTextureObject(renderInfo.getState()->getContextID())->id());
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _depthTexture->getTextureWidth(), _depthTexture->getTextureHeight());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    osg::ref_ptr<osg::Texture2D> _depthTexture;
};

namespace osg_3d_vis {
    rttcamera::rttcamera(osgViewer::Viewer& viewer) {

        osg::Camera* mainCamera( viewer.getCamera() );


        int rttTexSize = 128;


        screenColorTexture = new osg::Texture2D;
        screenColorTexture->setSourceFormat(GL_RGBA);
        screenColorTexture->setInternalFormat(GL_RGBA);
        screenColorTexture->setSourceType(GL_FLOAT);
        screenColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        screenColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
        // 不设置长宽，这样子图像会自动调整为和framebuffer长宽一样，否则会有黑边拉伸
        // screenColorTexture->setTextureWidth(mainCamera->);
        // screenColorTexture->setTextureHeight(rttTextureSize);

        screenDepthTexture = new osg::Texture2D;
        screenDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
        screenDepthTexture->setSourceType(GL_FLOAT);
        screenDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
        screenDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        screenDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
        // screenDepthTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        // screenDepthTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        screenDepthTexture->setTextureWidth(mainCamera->getViewport()->width());
        screenDepthTexture->setTextureHeight(mainCamera->getViewport()->height());
        // depthTexture->setTextureSize(1024, 768);


        testColorTexture = new osg::Texture2D;
        testColorTexture->setSourceFormat(GL_RGBA);
        testColorTexture->setInternalFormat(GL_RGBA);
        testColorTexture->setSourceType(GL_FLOAT);
        testColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        testColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
        // testColorTexture->setTextureSize(rttTexSize, rttTexSize);

        mainCamera->setPostDrawCallback(new DepthCopyCallback(screenDepthTexture));

        // enable depth write?
        // mainCamera->getStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS));
        //
        // mainCamera->attach(osg::Camera::COLOR_BUFFER, screenColorTexture);
        // mainCamera->attach(osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT24);
        // mainCamera->attach(osg::Camera::DEPTH_BUFFER, screenDepthTexture);

        // mainCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
        // mainCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // mainCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);

        //
        // osg::ref_ptr<osg::Camera> slaveCamera = new osg::Camera;
        // slaveCamera->setGraphicsContext(mainCamera->getGraphicsContext());
        // slaveCamera->setViewport(0, 0, 1920, 1080);
        // slaveCamera->setViewMatrix(mainCamera->getViewMatrix());
        // slaveCamera->setProjectionMatrix(mainCamera->getProjectionMatrix());
        // viewer.addSlave(slaveCamera, osg::Matrixd::identity(), osg::Matrixd::identity(), true);
        // slaveCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        // slaveCamera->attach(osg::Camera::COLOR_BUFFER, screenColorTexture);
        // slaveCamera->attach(osg::Camera::DEPTH_BUFFER, screenDepthTexture);

        //
        osg::ref_ptr< osg::Camera > postRenderCamera( new osg::Camera );
        // postRenderCamera->setClearMask( 0 );
        // FB: Final RT
        // FBO: Based on Textures
        postRenderCamera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER);
        postRenderCamera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
        postRenderCamera->setRenderOrder( osg::Camera::POST_RENDER, 1 );
        // postRenderCamera->setViewMatrix( osg::Matrixd::identity() );
        // postRenderCamera->setProjectionMatrix( osg::Matrixd::identity() );


        osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
        osg::ref_ptr<osg::StateSet> stateset = renderPassGeometry->getOrCreateStateSet();
        // Create vertex and fragment shaders
        osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
        osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source


        std::string shaderPath = std::string(OSG_3D_VIS_SHADER_PREFIX) + "rtt/";
        vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
        fragmentShader->loadShaderSourceFromFile(shaderPath + "rttScreenDraw.fs");

        // Create a shader program and attach the shaders
        osg::ref_ptr<osg::Program> program = new osg::Program;
        program->addBindAttribLocation("Vertex", 0);
        program->addBindAttribLocation("TexCoord", 1);
        program->addShader(vertexShader.get());
        program->addShader(fragmentShader.get());
        stateset->setAttributeAndModes(program);


//        osg::ref_ptr<osg::Uniform> uniformScreenColor = new osg::Uniform("screenColorTexture", 0);
//        stateset->setTextureAttributeAndModes(0, screenColorTexture.get(), osg::StateAttribute::ON);
//        stateset->addUniform(uniformScreenColor);

        osg::ref_ptr<osg::Uniform> uniformScreenDepth = new osg::Uniform("screenDepthTexture", 0);
        stateset->setTextureAttributeAndModes(0, screenDepthTexture.get(), osg::StateAttribute::ON);
        stateset->addUniform(uniformScreenDepth);

        osg::ref_ptr<osg::Uniform> uniformTestColor = new osg::Uniform("testColorTexture", 1);
        stateset->setTextureAttributeAndModes(1, testColorTexture.get(), osg::StateAttribute::ON);
        stateset->addUniform(uniformTestColor);


        renderPassGeometry->setStateSet(stateset);


        osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
        tmpGeode->addDrawable(renderPassGeometry);
//        osg::ref_ptr<osg::Transform> tmpNode = new osg::Transform;
//        tmpNode->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//        tmpNode->addChild(tmpGeode);

        postRenderCamera->addChild(tmpGeode);

        cameraNode = postRenderCamera.release();

        fullscreenQuadGeode = tmpGeode;



        {
            osg::ref_ptr< osg::Camera > TestRenderCamera( new osg::Camera );


            TestRenderCamera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT);
            TestRenderCamera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
            TestRenderCamera->setRenderOrder( osg::Camera::POST_RENDER, 0 );
            TestRenderCamera->attach(osg::Camera::COLOR_BUFFER, testColorTexture);
            // TestRenderCamera->setViewport(0, 0, rttTexSize, rttTexSize);

            osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
            osg::ref_ptr<osg::StateSet> stateset = renderPassGeometry->getOrCreateStateSet();
            // Create vertex and fragment shaders
            osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
            osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source


            std::string shaderPath = std::string(OSG_3D_VIS_SHADER_PREFIX) + "rtt/";
            vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
            fragmentShader->loadShaderSourceFromFile(shaderPath + "fullscreenColor.fs");

            // Create a shader program and attach the shaders
            osg::ref_ptr<osg::Program> program = new osg::Program;
            program->addBindAttribLocation("Vertex", 0);
            program->addBindAttribLocation("TexCoord", 1);
            program->addShader(vertexShader.get());
            program->addShader(fragmentShader.get());
            stateset->setAttributeAndModes(program);

            renderPassGeometry->setStateSet(stateset);


            osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
            tmpGeode->addDrawable(renderPassGeometry);
            TestRenderCamera->addChild(tmpGeode);



            fullscreenCameraNode = TestRenderCamera.release();
        }


    }

}


