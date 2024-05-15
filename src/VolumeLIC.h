#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include "Util.h"

namespace VolumeLIC {
	int row = 128;
	int col = 128;
	int lyr = 128;
	int sliceNum = 1000;

	void Generate3DVertices(std::vector<osg::Vec3f> &vertices, std::vector<osg::Vec3f> &texCoord3D)
	{
		vertices.resize(0);
		texCoord3D.resize(0);

		// Calculate vertex positions
		for (int i = 0; i < sliceNum; i++)
		{
			float tempZ = (float)i / (sliceNum - 1);

			vertices.push_back(osg::Vec3(0.0f, 1.0f, tempZ));
			vertices.push_back(osg::Vec3(1.0f, 1.0f, tempZ));
			vertices.push_back(osg::Vec3(1.0f, 0.0f, tempZ));
			vertices.push_back(osg::Vec3(0.0f, 0.0f, tempZ));

			texCoord3D.push_back(osg::Vec3(0.0f, 0.0f, tempZ));
			texCoord3D.push_back(osg::Vec3(1.0f, 0.0f, tempZ));
			texCoord3D.push_back(osg::Vec3(1.0f, 1.0f, tempZ));
			texCoord3D.push_back(osg::Vec3(0.0f, 1.0f, tempZ));
		}
	}
	osg::ref_ptr<osg::Texture3D> Generate3DTexture() {
		// Define the size of the 3D texture
		int size = row * col * lyr;

		// 1. Read vector data
		float* pVecData = new float[size * 3];
		FILE* fp = fopen( (std::string(OSG_3D_VIS_DATA_PREFIX) + "tornado0.dat").c_str(), "rb");
		fread(pVecData, sizeof(float), size * 3, fp);
		fclose(fp);

		// 2. Read output data
		unsigned char* pData = new unsigned char[size];
		fp = fopen((std::string(OSG_3D_VIS_DATA_PREFIX) + "output.dat").c_str(), "rb");
		fread(pData, sizeof(unsigned char), size, fp);
		fclose(fp);

		// Create a 3D image for the texture
		osg::ref_ptr<osg::Image> image3d = new osg::Image();
		image3d->allocateImage(row, col, lyr, GL_RGBA, GL_UNSIGNED_BYTE);

		// 4. Fill 3D texture data
		unsigned char* imgData = new unsigned char[size * 4];
		for (int i = 0; i < size; i++)
		{
			imgData[i * 4 + 0] = pData[i];
			imgData[i * 4 + 1] = pData[i];
			imgData[i * 4 + 2] = pData[i];
			float x = pVecData[i * 3 + 0];
			float y = pVecData[i * 3 + 1];
			float z = pVecData[i * 3 + 2];
			double vectMag = sqrt(x * x + y * y + z * z);
			if (vectMag < 0.85f && vectMag > 0.75f)
			{
				imgData[i * 4 + 3] = 200;
			}
			else if (vectMag < 0.5f && vectMag > 0.45f)
			{
				imgData[i * 4 + 3] = 0;
			}
			else
			{
				imgData[i * 4 + 3] = 0;
			}
		}

		//for (int i = 0; i < size; i++)
		//{
		//	imgData[i * 4 + 0] = static_cast<float>(rand()) / RAND_MAX * 255;
		//	imgData[i * 4 + 1] = static_cast<float>(rand()) / RAND_MAX * 255;
		//	imgData[i * 4 + 2] = static_cast<float>(rand()) / RAND_MAX * 255;
		//	imgData[i * 4 + 3] = 20;
		//	imgData[i * 4 + 0] = unsigned int(imgData[i * 4 + 0]);
		//	imgData[i * 4 + 1] = unsigned int(imgData[i * 4 + 1]);
		//	imgData[i * 4 + 2] = unsigned int(imgData[i * 4 + 2]);
		//	imgData[i * 4 + 3] = unsigned int(imgData[i * 4 + 3]);
		//}
		//std::cout << (int)imgData[0] << (int)imgData[1] << (int)imgData[2];

		image3d->setImage(row, col, lyr, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, imgData, osg::Image::AllocationMode::USE_NEW_DELETE);
		osg::ref_ptr<osg::Texture3D> texture3D = new osg::Texture3D;
		texture3D->setImage(image3d.get());
		texture3D->setFilter(osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR);
		texture3D->setFilter(osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR);
		texture3D->setWrap(osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_EDGE);
		texture3D->setWrap(osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_EDGE);
		texture3D->setWrap(osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_EDGE);


		return texture3D.get();
	}

	osg::ref_ptr<osg::Geode> Generate(osg::Camera* camera, llhRange range = llhRange())
	{

		// generate 3DVertices
		std::vector<osg::Vec3f> vertices, texCoord3D;
		Generate3DVertices(vertices, texCoord3D);

		osg::ref_ptr<osg::Texture3D> texture3D = Generate3DTexture();

		// Create a Geode to hold the 3D texture
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

		// Set vertex array in the geometry
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec3Array> vt = new osg::Vec3Array();
		for (int i = 0; i < 4 * sliceNum; i++) {
			double x = vertices[i].x();
			double y = vertices[i].y();
			double z = vertices[i].z();
			double tx, ty, tz;
			llh2xyz_Ellipsoid(range,
				x, y, z,
				tx, ty,tz);
			v->push_back(osg::Vec3(tx, ty, tz));
			vt->push_back(texCoord3D[i]);
		}
		geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
		osg::ref_ptr<osg::DrawArrays> quadStrip = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, 4 * sliceNum);

		//osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		//osg::ref_ptr<osg::Vec3Array> vt = new osg::Vec3Array();
		//for (int i = 0; i < SliceNum; i++) {
		//	float x = static_cast<float>(rand()) / RAND_MAX;
		//	float y = static_cast<float>(rand()) / RAND_MAX;
		//	float z = static_cast<float>(rand()) / RAND_MAX;

		//	//float tx, ty, tz;
		//	//vertShader(osg::DegreesToRadians(-90.f),
		//	//	osg::DegreesToRadians(90.f),
		//	//	osg::DegreesToRadians(-180.f),
		//	//	osg::DegreesToRadians(180.f),
		//	//	static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) * 1.005f,
		//	//	static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) * 1.205f,
		//	//	x,
		//	//	y,
		//	//	z,
		//	//	tx,
		//	//	ty,
		//	//	tz);

		//	double tx, ty, tz;
		//	
		//	llh2xyz(osg::DegreesToRadians(-90.f),
		//		osg::DegreesToRadians(90.f),
		//		osg::DegreesToRadians(-180.f),
		//		osg::DegreesToRadians(180.f),
		//		0.f,
		//		2000000.f,
		//		x,
		//		y,
		//		z,
		//		tx,
		//		ty,
		//		tz);

		//	v->push_back(osg::Vec3(tx, ty, tz));


		//	//v->push_back(osg::Vec3(x, y, z));
		//	vt->push_back(osg::Vec3(x, y, z));
		//}

		//geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
		//geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
		//osg::ref_ptr<osg::DrawArrays> quadStrip = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, SliceNum);



		// Add the primitive set to the geometry
		geometry->addPrimitiveSet(quadStrip.get());

		//// Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX)+"pass.vs");
		fragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) +"frag.fs");

		//// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);

		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());

		// Set stateset for the quad
		osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();

		osg::Uniform* modelViewProjectionMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
		modelViewProjectionMatrix->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
		stateset->addUniform(modelViewProjectionMatrix);

		osg::Uniform* cameraEye = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "CameraEye");
		cameraEye->setUpdateCallback(new CameraEyeCallback(camera));
		stateset->addUniform(cameraEye);

		// 示例：如何绑定 3DTexture 到 Uniform 
		// 设置 Unfirom 名称，设置 ID 为0，接着调用 setTextureAttributeAndModes 进行 texture 到 Uniform 的绑定
		osg::ref_ptr<osg::Uniform> volumeTexture = new osg::Uniform("VolumeTexture", 0);
		stateset->setTextureAttributeAndModes(0, texture3D.get(), osg::StateAttribute::ON);
		stateset->addUniform(volumeTexture);

		stateset->setAttributeAndModes(program);

		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);//管理深度测试
		//stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
		// Blend Rendering Related 使用颜色的ALPHA通道进行透明材质渲染
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		//osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
		//blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		//stateset->setAttributeAndModes(blendFunc);

		// Create a geode to hold the geometry,	add the quad to the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(geometry.get());
		geode->setStateSet(stateset);

		return  geode.get();

	}
};


