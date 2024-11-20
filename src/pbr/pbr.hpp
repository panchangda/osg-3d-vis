//
// Created by pcd on 2024/9/16.
//

#ifndef PBR_HPP
#define PBR_HPP

#include <osgViewer/Viewer>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>

class PBR {
public:


	osg::ref_ptr<osg::Geometry> createSphereGeometry(float radius, unsigned int rings, unsigned int sectors,  float i, float j)
	{
	    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

	    float const R = 1.0 / (float)(rings - 1);
	    float const S = 1.0 / (float)(sectors - 1);

	    for (unsigned int r = 0; r < rings; ++r)
	    {
	        for (unsigned int s = 0; s < sectors; ++s)
	        {
	            float const y = std::sin(-M_PI_2 + M_PI * r * R);
	            float const x = std::cos(2 * M_PI * s * S) * std::sin(M_PI * r * R);
	            float const z = std::sin(2 * M_PI * s * S) * std::sin(M_PI * r * R);

	            vertices->push_back(osg::Vec3(x * radius+i/2, y * radius+j/2, z * radius-1));
	            normals->push_back(osg::Vec3(x, y, z));
	        }
	    }

	    for (unsigned int r = 0; r < rings - 1; ++r)
	    {
	        for (unsigned int s = 0; s < sectors - 1; ++s)
	        {
	            indices->push_back(r * sectors + s);
	            indices->push_back((r + 1) * sectors + (s + 1));
	            indices->push_back(r * sectors + (s + 1));

	            indices->push_back(r * sectors + s);
	            indices->push_back((r + 1) * sectors + s);
	            indices->push_back((r + 1) * sectors + (s + 1));
	        }
	    }

	    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		geometry->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(1, normals, osg::Array::BIND_PER_VERTEX);

	    geometry->addPrimitiveSet(indices);

	    return geometry;
	}

	auto createLightModel(osg::ref_ptr<osg::Camera> camera ) {
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;

		//金属的
		osg::ref_ptr<osg::Vec3Array> metalColors = new osg::Vec3Array;
		{
			
			osg::Vec3 aluminumColor(0.77f, 0.77f, 0.77f);   // ��
			osg::Vec3 goldColor(1.0f, 0.84f, 0.0f);         // ��
			osg::Vec3 copperColor(0.72f, 0.45f, 0.2f);      // ͭ
			osg::Vec3 silverColor(0.75f, 0.75f, 0.75f);     // ��
			osg::Vec3 ironColor(0.56f, 0.57f, 0.58f);       // ��
			osg::Vec3 bronzeColor(0.55f, 0.47f, 0.14f);     // ��ͭ
			osg::Vec3 titaniumColor(0.54f, 0.57f, 0.62f);   // ��
			osg::Vec3 platinumColor(0.81f, 0.78f, 0.75f);   // ��

			// �����ɫ������
			metalColors->push_back(aluminumColor);
			metalColors->push_back(goldColor);
			metalColors->push_back(copperColor);
			metalColors->push_back(silverColor);
			metalColors->push_back(ironColor);
			metalColors->push_back(bronzeColor);
			metalColors->push_back(titaniumColor);
			metalColors->push_back(platinumColor);
		}

		osg::ref_ptr<osg::Vec3Array> metalF0Values = new osg::Vec3Array;

		{
			
			osg::Vec3 aluminumF0(0.913f, 0.921f, 0.924f);  // ��
			osg::Vec3 goldF0(1.000f, 0.782, 0.344);      // ��
			osg::Vec3 copperF0(0.955f, 0.637f, 0.538f);    // ͭ
			osg::Vec3 silverF0(0.972f, 0.960f, 0.915f);    // ��
			osg::Vec3 ironF0(0.562f, 0.565f, 0.578f);      // ��
			osg::Vec3 bronzeF0(0.495f, 0.424f, 0.275f);    // ��ͭ
			osg::Vec3 titaniumF0(0.542f, 0.497f, 0.449f);  // ��
			osg::Vec3 platinumF0(0.673f, 0.637f, 0.585f);  // ��

			
			metalF0Values->push_back(aluminumF0);
			metalF0Values->push_back(goldF0);
			metalF0Values->push_back(copperF0);
			metalF0Values->push_back(silverF0);
			metalF0Values->push_back(ironF0);
			metalF0Values->push_back(bronzeF0);
			metalF0Values->push_back(titaniumF0);
			metalF0Values->push_back(platinumF0);
		}

		// ����һ�� Vec3Array �����һЩ������ɫֵ
		osg::ref_ptr<osg::Vec3Array> paintColors = new osg::Vec3Array;
		{
			// ������ɫʾ�� (RGB ��Χ 0.0 - 1.0)
			osg::Vec3 redPaint(1.0f, 0.0f, 0.0f);       // ��ɫ
			osg::Vec3 greenPaint(0.0f, 1.0f, 0.0f);     // ��ɫ
			osg::Vec3 bluePaint(0.0f, 0.0f, 1.0f);      // ��ɫ
			osg::Vec3 whitePaint(1.0f, 1.0f, 1.0f);     // ��ɫ
			osg::Vec3 blackPaint(0.0f, 0.0f, 0.0f);     // ��ɫ
			osg::Vec3 yellowPaint(1.0f, 1.0f, 0.0f);    // ��ɫ
			osg::Vec3 cyanPaint(0.0f, 1.0f, 1.0f);      // ��ɫ
			osg::Vec3 magentaPaint(1.0f, 0.0f, 1.0f);   // Ʒ��ɫ

			// �����ɫ������
			paintColors->push_back(redPaint);
			paintColors->push_back(greenPaint);
			paintColors->push_back(bluePaint);
			paintColors->push_back(whitePaint);
			paintColors->push_back(blackPaint);
			paintColors->push_back(yellowPaint);
			paintColors->push_back(cyanPaint);
			paintColors->push_back(magentaPaint);
		}

		// ����һ�� Vec3Array ����Ӷ�Ӧ�ķ����� F0 ֵ
		osg::ref_ptr<osg::Vec3Array> paintF0Values = new osg::Vec3Array;

		{
			
			osg::Vec3 redF0(0.04f, 0.04f, 0.04f);       // ��ɫ
			osg::Vec3 greenF0(0.04f, 0.04f, 0.04f);     // ��ɫ
			osg::Vec3 blueF0(0.04f, 0.04f, 0.04f);      // ��ɫ
			osg::Vec3 whiteF0(0.04f, 0.04f, 0.04f);     // ��ɫ
			osg::Vec3 blackF0(0.04f, 0.04f, 0.04f);     // ��ɫ
			osg::Vec3 yellowF0(0.04f, 0.04f, 0.04f);    // ��ɫ
			osg::Vec3 cyanF0(0.04f, 0.04f, 0.04f);      // ��ɫ
			osg::Vec3 magentaF0(0.04f, 0.04f, 0.04f);   // Ʒ��ɫ

			
			paintF0Values->push_back(redF0);
			paintF0Values->push_back(greenF0);
			paintF0Values->push_back(blueF0);
			paintF0Values->push_back(whiteF0);
			paintF0Values->push_back(blackF0);
			paintF0Values->push_back(yellowF0);
			paintF0Values->push_back(cyanF0);
			paintF0Values->push_back(magentaF0);
		}

		osg::Vec3 lightPositions[4] = {
		osg::Vec3(20.0f, 20.0f, 8.0f),
		osg::Vec3(-20.0f, 20.0f, 8.0f),
		osg::Vec3(20.0f, -20.0f, 8.0f),
		osg::Vec3(-20.0f, -20.0f, 8.0f)
		};

		osg::Vec3 lightColors[4] = {
			osg::Vec3(2200.0f, 2200.0f, 2200.0f),
			osg::Vec3(2200.0f, 2200.0f, 2200.0f),
			osg::Vec3(2200.0f, 2200.0f, 2200.0f),
			osg::Vec3(2200.0f, 2200.0f, 2200.0f)
		};


		for (int i = -2; i <= 2; ++i) {

			osg::ref_ptr<osg::Geometry> geometry = createSphereGeometry(0.1, 20, 20, i*0.7, 1*0.6);
			geode->addDrawable(geometry);

			auto stateset = geometry->getOrCreateStateSet();
			


			stateset->addUniform(new osg::Uniform("albedo", metalColors->at(i + 3)));
			stateset->addUniform(new osg::Uniform("F0", metalF0Values->at(i + 3)));
			stateset->addUniform(new osg::Uniform("roughness", float(0.33)));
			stateset->addUniform(new osg::Uniform("metallic", float(0.96)));
		}
		for (int i = -2; i <= 2; ++i) {

			osg::ref_ptr<osg::Geometry> geometry = createSphereGeometry(0.1, 20, 20, i*0.7, -1*0.6);
			geode->addDrawable(geometry);

			auto stateset = geometry->getOrCreateStateSet();
			


			stateset->addUniform(new osg::Uniform("albedo", paintColors->at(i + 3)));
			stateset->addUniform(new osg::Uniform("F0", paintF0Values->at(i + 3)));
			stateset->addUniform(new osg::Uniform("roughness", float(0)));
			stateset->addUniform(new osg::Uniform("metallic", float(0.0)));
		}

		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		// bind shader
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		vertexShader->loadShaderSourceFromFile("../shader/lightvs.glsl");
		fragmentShader->loadShaderSourceFromFile("../shader/lightps.glsl");
		osg::ref_ptr<osg::Program> program1 = new osg::Program;
		program1->addShader(vertexShader.get());
		program1->addShader(fragmentShader.get());

		stateset->setAttributeAndModes(program1, osg::StateAttribute::ON);
		geode->setStateSet(stateset);


		stateset->addUniform(new osg::Uniform("view", osg::Matrixf(camera->getViewMatrix())));
		stateset->addUniform(new osg::Uniform("projection", osg::Matrixf(camera->getProjectionMatrix())));

		stateset->addUniform(new osg::Uniform("ao", float(0.5)));
		osg::ref_ptr<osg::Uniform> lightColorsUniform = new osg::Uniform(osg::Uniform::Type::FLOAT_VEC3, "lightColors", 4);
		for (int i = 0; i < 4; ++i) {
			lightColorsUniform->setElement(i, lightColors[i]);
		}
		stateset->addUniform(lightColorsUniform.get());

		osg::ref_ptr<osg::Uniform> lightPositionsUniform = new osg::Uniform(osg::Uniform::Type::FLOAT_VEC3, "lightPositions", 4);
		for (int i = 0; i < 4; ++i) {
			lightPositionsUniform->setElement(i, lightPositions[i]);
		}
		stateset->addUniform(lightPositionsUniform.get());
		stateset->addUniform(new osg::Uniform("camPos", osg::Vec3(0, 0, 1)));
		return geode;
	}
};




#endif //PBR_HPP
