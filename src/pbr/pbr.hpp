//
// Created by pcd on 2024/9/16.
//

#ifndef PBR_HPP
#define PBR_HPP

#include <numbers>
#include <osgViewer/Viewer>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>
#include <pcl/pcl_macros.h>

class PBR {
public:
	const std::string TankerObj = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/Tank.obj";
	const std::string texAlbedo1 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_D_Forest.PNG";
	const std::string texArm1 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_ARM.PNG"; 
	const std::string texNormal1 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_N.PNG";
	const std::string texAlbedo2 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_Track_D.PNG";
	const std::string texArm2 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_Track_ARM.PNG";
	const std::string texNormal2 = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/OBJ/T_West_Tank_M1A1Abrams_Track_N.PNG";
	osg::ref_ptr<osg::Geode> Geos;
	osg::ref_ptr<osg::Geometry> CreareSingleGeode(const osg_3d_vis::GeometryExtractor& data, int index)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		geom->setVertexAttribArray(0, data.geometries[index].vertices, osg::Array::BIND_PER_VERTEX);
		geom->setVertexAttribArray(1, data.geometries[index].uvs, osg::Array::BIND_PER_VERTEX);
		geom->setVertexAttribArray(2, data.geometries[index].noramls, osg::Array::BIND_PER_VERTEX);
		auto Primitives = data.geometries[index].shortIndices;
		geom->addPrimitiveSet(Primitives);
		if( index <2736)
		{
			geom->getOrCreateStateSet()->addUniform(new osg::Uniform("index", 1));
		}else
		{
			geom->getOrCreateStateSet()->addUniform(new osg::Uniform("index", 2));
		}
		return geom;
	}

	osg::ref_ptr<osg::Texture2D> loadTexture(const std::string& textureFilePath) {
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile(textureFilePath); // 读取图像文件
		if (!image) {
			std::cerr << "Failed to load texture: " << textureFilePath << std::endl;
			return nullptr;
		}

		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image); // 设置图像为纹理    texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
		texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
		//texture->setTextureWidth(image->computeRowWidthInBytes());
		texture->setTextureWidth(image->s());
		texture->setTextureHeight(image->t());
		return texture;
	}
	class CameraPosUniformCallback : public osg::Uniform::Callback {
	public:
		CameraPosUniformCallback(osg::Camera* camera): Cam(camera) {}
		virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
			
			uniform->set(osg::Vec3(Cam->getViewMatrix().getTrans()));
		}
		osg::Camera* Cam;
	};

	class thetaUniformCallback : public osg::Uniform::Callback {
	public:

		virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
			theta += 0.0005;
			theta = theta > osg::PI_2 ? theta-osg::PI_2 : theta;
			uniform->set(theta);
		}
		float theta = 0;;
	};

	void createLightModel(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> camera ) {
		Geos = new osg::Geode;
		osg::ref_ptr<osg::Node> objModel = osgDB::readNodeFile(TankerObj.c_str());

		if (!objModel) {
			std::cerr << "Failed to load model.obj" << std::endl;
			exit(1);
		}

		osg_3d_vis::GeometryExtractor geoExtractor;
		objModel->accept(geoExtractor);

		for (int i = 0; i < geoExtractor.geometries.size(); ++i) {
			Geos->addChild(CreareSingleGeode(geoExtractor, i));
		}
		
		auto stateSet = Geos->getOrCreateStateSet();
		// shaders
		osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "Pbr/lightvs.glsl");
		FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "Pbr/lightps.glsl");
		osg::ref_ptr<osg::Program> Program = new osg::Program;
		Program->addShader(VertexShader);
		Program->addShader(FragmentShader);

		stateSet->setAttributeAndModes(Program);
		// textures
		auto tex1 = loadTexture(texAlbedo1);
		osg::ref_ptr<osg::Uniform> albedoUniform = new osg::Uniform("alb1", 0);
		auto tex2 = loadTexture(texNormal1);
		osg::ref_ptr<osg::Uniform> NorUniform = new osg::Uniform("Nor1", 1);
		auto tex3 = loadTexture(texArm1);
		osg::ref_ptr<osg::Uniform> RouUniform = new osg::Uniform("Rou1", 2);
		stateSet->setTextureAttributeAndModes(0, tex1.get());
		stateSet->setTextureAttributeAndModes(1, tex2.get());
		stateSet->setTextureAttributeAndModes(2, tex3.get());
		stateSet->addUniform(albedoUniform);
		stateSet->addUniform(NorUniform);
		stateSet->addUniform(RouUniform);
		// textures
		auto tex4 = loadTexture(texAlbedo2);
		osg::ref_ptr<osg::Uniform> albedoUniform1 = new osg::Uniform("alb2", 3);
		auto tex5 = loadTexture(texNormal2);
		osg::ref_ptr<osg::Uniform> NorUniform1 = new osg::Uniform("Nor2", 4);
		auto tex6 = loadTexture(texArm2);
		osg::ref_ptr<osg::Uniform> RouUniform1 = new osg::Uniform("Rou2", 5);
		stateSet->setTextureAttributeAndModes(3, tex4.get());
		stateSet->setTextureAttributeAndModes(4, tex5.get());
		stateSet->setTextureAttributeAndModes(5, tex6.get());
		stateSet->addUniform(albedoUniform1);
		stateSet->addUniform(NorUniform1);
		stateSet->addUniform(RouUniform1);

		// other com uniform
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
		mvpUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(camera));
		stateSet->addUniform(mvpUniform);
		auto cpos = new osg::Uniform( osg::Uniform::FLOAT_VEC3, "camPos");
		cpos->setUpdateCallback(new CameraPosUniformCallback(camera));
		stateSet->addUniform(cpos);
		auto theta = new osg::Uniform(osg::Uniform::FLOAT, "theta");
		theta->setUpdateCallback( new thetaUniformCallback());
		stateSet->addUniform(theta);
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
		transform->setPosition(osg_3d_vis::HangZhouPos);
		transform->setScale({ 200,200,200 });

		auto  calculateOrientation = [&](const osg::Vec3& position) {
			osg::Vec3 normal = position; // 法线
			normal.normalize();

			// 初始向上的参考向量
			osg::Vec3 up(0, 0, 1);

			// 计算旋转四元数
			osg::Quat quat;
			quat.makeRotate(up, normal); // 将 "up" 对齐到 "normal"

			return quat;
			};
		transform->setAttitude( calculateOrientation(osg_3d_vis::HangZhouPos));
		transform->addChild(Geos);
		_root->addChild(transform);
	}
};




#endif //PBR_HPP
