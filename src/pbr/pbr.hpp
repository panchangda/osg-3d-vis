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
	const std::string TankerObj = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/IS4.obj";
	const std::string texAlbedo = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/IS_4M.dds";
	const std::string texNormal = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/IS_4M_NM.dds";
	const std::string texRoughness = std::string(OSG_3D_VIS_DATA_PREFIX) + "Tanker/IS_4M_SM.dds";
	osg::ref_ptr<osg::Texture2D> tex1 = loadTexture(texAlbedo);
	osg::ref_ptr<osg::Texture2D> tex2 = loadTexture(texNormal);
	osg::ref_ptr<osg::Texture2D> tex3 = loadTexture(texRoughness);
	osg::ref_ptr<osg::Geode> Geos;
	osg::ref_ptr<osg::Geometry> CreareSingleGeode(const osg_3d_vis::GeometryExtractor& data, int index)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		geom->setVertexAttribArray(0, data.geometries[index].vertices, osg::Array::BIND_PER_VERTEX);
		geom->setVertexAttribArray(1, data.geometries[index].uvs, osg::Array::BIND_PER_VERTEX);

		auto Primitives = data.geometries[index].shortIndices;
		geom->addPrimitiveSet(Primitives);
		auto stateset = geom->getOrCreateStateSet();
		stateset->addUniform(new osg::Uniform("kd", osg::Vec3(data.geometries[index].materials.kd.x(),
			data.geometries[index].materials.kd.y(), data.geometries[index].materials.kd.z())));
		stateset->addUniform(new osg::Uniform("ks", osg::Vec3(data.geometries[index].materials.ks.x(),
			data.geometries[index].materials.ks.y(), data.geometries[index].materials.ks.z())));
		stateset->addUniform(new osg::Uniform("F0", data.geometries[index].materials.F0));
		stateset->addUniform(new osg::Uniform("ao", osg::Vec3(data.geometries[index].materials.Ao.x(),
			data.geometries[index].materials.Ao.y(), data.geometries[index].materials.Ao.z())));

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
		auto tex1 = loadTexture(texAlbedo);
		osg::ref_ptr<osg::Uniform> albedoUniform = new osg::Uniform("alb", 0);
		auto tex2 = loadTexture(texNormal);
		osg::ref_ptr<osg::Uniform> NorUniform = new osg::Uniform("Nor", 1);
		auto tex3 = loadTexture(texRoughness);
		osg::ref_ptr<osg::Uniform> RouUniform = new osg::Uniform("Rou", 2);
		stateSet->setTextureAttributeAndModes(0, tex1.get());
		stateSet->setTextureAttributeAndModes(1, tex2.get());
		stateSet->setTextureAttributeAndModes(2, tex3.get());
		stateSet->addUniform(albedoUniform);
		stateSet->addUniform(NorUniform);
		stateSet->addUniform(RouUniform);

		// other com uniform
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
		mvpUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(camera));
		stateSet->addUniform(mvpUniform);
		auto cpos = new osg::Uniform( osg::Uniform::FLOAT_VEC3, "camPos");
		cpos->setUpdateCallback(new CameraPosUniformCallback(camera));
		stateSet->addUniform(cpos);
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
		transform->setPosition(osg_3d_vis::HangZhouPos);
		transform->setScale({ 20000,20000,20000 });
		transform->addChild(Geos);
		_root->addChild(transform);
	}
};




#endif //PBR_HPP
