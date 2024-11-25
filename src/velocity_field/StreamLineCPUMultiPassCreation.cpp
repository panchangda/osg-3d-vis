//
// Created by pcd on 2024/11/12.
//

#include <osg/BlendFunc>
#include <osgText/Text>

#include "StreamlineCPU.h"

namespace osg_3d_vis{


	osg::ref_ptr<osg::Camera> StreamLineCPU::createSegmentDrawPass(osg::ref_ptr<osg::Camera> mainCamera) {
		segmentDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::StateSet> stateset = this->geometry->getOrCreateStateSet();
		//  Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "segmentDraw.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "segmentDraw.fs");

		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("VertexColor", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);

		// �� mvp
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
		mvpUniform->setUpdateCallback(new MVPRedrawCallback(mainCamera, this));
		stateset->addUniform(mvpUniform);



		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(this->geometry);
		updateNodeGeometryCallbackPtr = new updateNodeGeometryCallback(this);
		tmpGeode->addUpdateCallback(updateNodeGeometryCallbackPtr);

//        stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//        stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        //segmentDrawCamera->setStateSet(stateset);

		segmentDrawCamera->addChild(tmpGeode);

		segmentDrawCamera->setClearColor(osg::Vec4());
		segmentDrawCamera->setClearDepth(1.0f);
		segmentDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		segmentDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        segmentDrawCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        segmentDrawCamera->setRenderOrder(osg::Camera::POST_RENDER, 0);
        segmentDrawCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
		segmentDrawCamera->attach(osg::Camera::COLOR_BUFFER, segmentColorTexture);
		segmentDrawCamera->attach(osg::Camera::DEPTH_BUFFER, segmentDepthTexture);
		return segmentDrawCamera;
	}
	class updateTrailDrawPassCallback : public osg::Camera::DrawCallback {
	public:
		StreamLineCPU* sl;
		osg::Camera* camera;
		updateTrailDrawPassCallback(StreamLineCPU* _sl, osg::Camera* _camera) {
			sl = _sl;
			camera = _camera;
		}
		virtual void operator()(osg::RenderInfo& renderInfo) const
		{
			//osg::State* state = renderInfo.getState();
			//state->applyAttribute(sl->nextTrailColorTexture.get());
			//osg::Image* image = new osg::Image();
			//image->readImageFromCurrentTexture(renderInfo.getContextID(), false, GL_FLOAT);
			//osgDB::writeImageFile(*image, "snap.png");

			//osg::State* state = renderInfo.getState();
			//state->apply(renderInfo.getCurrentCamera()->getStateSet());
			//state->applyTextureAttribute(0, sl->currTrailColorTexture);
			//osg::Image* image = new osg::Image();
			//sl->currTrailColorTexture->apply(*state);
			//image->readImageFromCurrentTexture(renderInfo.getContextID(), true,
			//	GL_FLOAT); //change the GL_FLOAT to your needs
			//
			//osgDB::writeImageFile(*image, "snap.png");
			//sl->currTrailColorTexture->setImage(image);
			//sl->currTrailColorTexture->dirtyTextureObject();


			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (camera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
            if(sl->cameraMoving){
                stateset->getUniform("first")->set(bool(true));
            }else{
                stateset->getUniform("first")->set(bool(false));
            }


		}
	};
	osg::ref_ptr<osg::Camera> StreamLineCPU::createTrailDrawPass() {
		trailDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::Geometry> trailDrawPassGometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = trailDrawPassGometry->getOrCreateStateSet();
		//  Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "trailDraw.fs");


		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);


		osg::ref_ptr<osg::Uniform> uniformTrailColor = new osg::Uniform("trailColorTexture", 0);
		stateset->setTextureAttributeAndModes(0, currTrailColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformTrailColor);
		osg::ref_ptr<osg::Uniform> uniformTrailDepth = new osg::Uniform("trailDepthTexture", 1);
		stateset->setTextureAttributeAndModes(1, currTrailDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformTrailDepth);
		osg::ref_ptr<osg::Uniform> uniformSegmentColor = new osg::Uniform("segmentColorTexture", 2);
		stateset->setTextureAttributeAndModes(2, segmentColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentColor);
		osg::ref_ptr<osg::Uniform> uniformSegmentDepth = new osg::Uniform("segmentDepthTexture", 3);
		stateset->setTextureAttributeAndModes(3, segmentDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentDepth);

        osg::ref_ptr<osg::Uniform> firstUniform = new osg::Uniform("first", bool(true));
        stateset->addUniform(firstUniform);

        osg::ref_ptr<osg::Uniform> fadeOpacityUniform = new osg::Uniform("fadeOpacity", (float)0.95);
		stateset->addUniform(fadeOpacityUniform);

//        stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
//        blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        stateset->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
//        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(trailDrawPassGometry);
		trailDrawCamera->addChild(tmpGeode);


        trailDrawCamera->setClearColor(osg::Vec4());
        trailDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		trailDrawCamera->setClearDepth(1.0f);
		trailDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        trailDrawCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
        trailDrawCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        trailDrawCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
         trailDrawCamera->attach(osg::Camera::COLOR_BUFFER, nextTrailColorTexture);
         trailDrawCamera->attach(osg::Camera::DEPTH_BUFFER, nextTrailDepthTexture);

		trailDrawCamera->setPostDrawCallback(new updateTrailDrawPassCallback(this, trailDrawCamera));

		return trailDrawCamera;
	}


	osg::ref_ptr<osg::Camera> StreamLineCPU::createScreenDrawPass() {
		osg::ref_ptr<osg::Camera> screenDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
		//osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = renderPassGeometry->getOrCreateStateSet();
		// Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "screenDraw.fs");

		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);


        osg::ref_ptr<osg::Uniform> uniformScreenColor = new osg::Uniform("screenColorTexture", 0);
        stateset->setTextureAttributeAndModes(0, screenColorTexture.get(), osg::StateAttribute::ON);
        stateset->addUniform(uniformScreenColor);

        osg::ref_ptr<osg::Uniform> uniformScreenDepth = new osg::Uniform("screenDepthTexture", 1);
        stateset->setTextureAttributeAndModes(1, screenDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformScreenDepth);

        osg::ref_ptr<osg::Uniform> uniformSegmentColor = new osg::Uniform("streamlineColorTexture", 2);
        stateset->setTextureAttributeAndModes(2, nextTrailColorTexture.get(), osg::StateAttribute::ON);
        stateset->addUniform(uniformSegmentColor);

        osg::ref_ptr<osg::Uniform> uniformSegmentDepth = new osg::Uniform("streamlineDepthTexture", 3);
        stateset->setTextureAttributeAndModes(3, nextTrailDepthTexture.get(), osg::StateAttribute::ON);
        stateset->addUniform(uniformSegmentDepth);


//         osg::ref_ptr<osg::Uniform> uniformSegmentColor = new osg::Uniform("streamlineColorTexture", 2);
//         stateset->setTextureAttributeAndModes(2, segmentColorTexture.get(), osg::StateAttribute::ON);
//         stateset->addUniform(uniformSegmentColor);

//         osg::ref_ptr<osg::Uniform> uniformSegmentDepth = new osg::Uniform("streamlineDepthTexture", 3);
//         stateset->setTextureAttributeAndModes(3, segmentDepthTexture.get(), osg::StateAttribute::ON);
//         stateset->addUniform(uniformSegmentDepth);


		// Blend Rendering Related ʹ����ɫ��ALPHAͨ������͸��������Ⱦ
//         stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//         stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		// stateset->setRenderBinDetails(11, "RenderBin");

//         stateset->setMode(GL_DEPTH_WRITEMASK, GL_FALSE);
		//
//        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
//        blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        stateset->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);


		renderPassGeometry->setStateSet(stateset);


		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(renderPassGeometry);
		osg::ref_ptr<osg::Transform> tmpNode = new osg::Transform;
		tmpNode->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		tmpNode->addChild(tmpGeode);

		screenDrawCamera->addChild(tmpNode);

		 // render to screen frame buffer
		screenDrawCamera->setClearMask(0);
		 screenDrawCamera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER);
		 screenDrawCamera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
		 screenDrawCamera->setRenderOrder( osg::Camera::POST_RENDER , 3);

		// screenDrawCamera->addChild(tmpGeode);

		// screenDrawCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		// screenDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//screenDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//screenDrawCamera->setRenderOrder(osg::Camera::PRE_RENDER, 2);

		//screenDrawCamera->setViewport(0, 0, 1024, 768);

		return screenDrawCamera;
	}

	// Copy next trail -> curr trail
	osg::ref_ptr<osg::Camera> StreamLineCPU::createCopyPass() {
		osg::ref_ptr<osg::Camera> copyCamera = new osg::Camera;
		osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "copyColorAndDepth.fs");
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);

		osg::ref_ptr<osg::Uniform> nextTrailColorTextureUniform = new osg::Uniform("colorTexture", 0);
		stateset->setTextureAttributeAndModes(0, nextTrailColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(nextTrailColorTextureUniform);

		osg::ref_ptr<osg::Uniform> nextTrailDepthTextureUniform = new osg::Uniform("depthTexture", 1);
		stateset->setTextureAttributeAndModes(1, nextTrailDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(nextTrailDepthTextureUniform);

		osg::ref_ptr<osg::Uniform> firstUniform = new osg::Uniform("first", bool(true));
		stateset->addUniform(firstUniform);

		osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
		tmpGeo->addDrawable(copyPassGeometry.get());
		copyCamera->addChild(tmpGeo);

        copyCamera->setRenderOrder(osg::Camera::POST_RENDER, 4);
        // copyCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		// copyCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		copyCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		copyCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        copyCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
		copyCamera->attach(osg::Camera::COLOR_BUFFER, currTrailColorTexture);
		copyCamera->attach(osg::Camera::DEPTH_BUFFER, currTrailDepthTexture);

		// reset first uniform
		copyCamera->setPostDrawCallback(new updateTrailDrawPassCallback(this, copyCamera));

		return copyCamera;
	}

	osg::ref_ptr<osg::Camera> StreamLineCPU::createTextPass() {
		osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera;

		// 设置为正交投影
		hudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 720));  // 假设屏幕分辨率为 1280x720
		hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		hudCamera->setViewMatrix(osg::Matrix::identity());
		hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

		osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
		streamlineText = new osgText::Text;

		streamlineText->setFont("fonts/arial.ttf");
		streamlineText->setText(LineCountText + LineAvgSpeedText);
		streamlineText->setPosition(osg::Vec3(1.0f, 1.0f, 0));  // 屏幕中心
		streamlineText->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));  // 黄色
		streamlineText->setCharacterSize(30.0f);

		textGeode->addDrawable(streamlineText);

		perStreamlineTextGeode = new osg::Geode;

		hudCamera->addChild(perStreamlineTextGeode);
		hudCamera->addChild(textGeode);

		hudCamera->setRenderOrder(osg::Camera::POST_RENDER, 999);

		return hudCamera;
	}
};
