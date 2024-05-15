#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include "Util.h"
#include "APMRadarRender.h"
#include "volumeRender.cpp"

namespace Radar {
	RadarRender* _radarrender;
	int voxels[256][256][256];
	unsigned char* voxelsData = new unsigned char[sizeof(float) * 256 * 256 * 256];
	llhRange _range;

	void initRadar(llhRange range = llhRange())
	{
		for (int i = 0; i < 256; i++)
			for (int j = 0; j < 256; j++)
				for (int k = 0; k < 256; k++)
					voxels[i][j][k] = 0;
		_range = range;
	}

	void transPointsToVoxels(osg::ref_ptr<osg::Vec3Array> points, float curd, llhRange smallRange)
	{
		int mid = 128;
		std::vector<std::vector<std::vector<int>>> localVoxels(256, std::vector<std::vector<int>>(256, std::vector<int>(256, 0)));

		// 求smallRange在_range中的比例，得到中心与半径
		int centerX = ((smallRange.maxLatitude + smallRange.minLatitude) / 2 - _range.minLatitude) / (_range.maxLatitude - _range.minLatitude) * 256;
		int centerY = ((smallRange.maxLongtitude + smallRange.minLongtitude) / 2 - _range.minLongtitude) / (_range.maxLongtitude - _range.minLongtitude) * 256;
		float rRatio = (smallRange.maxLatitude - smallRange.minLatitude) / (_range.maxLatitude - _range.minLatitude);

		// 清空localVoxels为false
		for (int i = 0; i < 256; i++)
			for (int j = 0; j < 256; j++)
				for (int k = 0; k < 256; k++)
					localVoxels[i][j][k] = 0;

		for (int i = 0; i < points->size(); i++)
		{
			if (points->at(i).z() < curd)
				continue;
			int x = points->at(i).x() / 5 * rRatio;
			int y = points->at(i).y();
			if (localVoxels[centerX + x][centerY][y] == 1)
				continue;
			localVoxels[centerX + x][centerY][y] = 1;
			float stepAngle = (360.0 / 180.0) * M_PI / 3600.0;
			for (int i = 1; i < 3600; i++)
			{
				float curangle = stepAngle * i;
				float sinthita = sin(curangle);
				float costhita = cos(curangle);
				localVoxels[centerX + int(x * costhita)][centerY + int(x * sinthita)][y] = 1;
			}
		}

		for (int i = 0; i < 256; i++)
			for (int j = 0; j < 256; j++)
				for (int k = 0; k < 256; k++)
					voxels[i][j][k] += localVoxels[i][j][k];

	}

	void submitRadar(osg::Group* grp)
	{
		osg::ref_ptr<osg::Texture3D> voxelsTexture = new osg::Texture3D;
		osg::ref_ptr<osg::Image> voxelsImage = new osg::Image;
		voxelsImage->setImage(256, 256, 256, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, voxelsData, osg::Image::USE_NEW_DELETE);
		//voxelsTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		//voxelsTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		voxelsTexture->setImage(voxelsImage);

		static const std::array<uint32_t, 3> dim = { 256, 256, 256 };
		static const std::array<float, 2> lonRng = { float (_range.minLongtitude / M_PI * 180.f - 360.f), float (_range.maxLongtitude / M_PI * 180.f - 360.f) };
		static const std::array<float, 2> latRng = { float(_range.minLatitude / M_PI * 180.f), float (_range.maxLatitude / M_PI * 180.f) };
		static const std::array<float, 2> hRng = { float (_range.minHeight), float(_range.maxHeight) };
		static const float hScale = 150.f;

		auto dvr = std::make_shared<SciVis::ScalarViser::DirectVolumeRenderer>();
		dvr->SetDeltaT(hScale * (hRng[1] - hRng[0]) / dim[2] * .3f);
		dvr->SetMaxStepCount(8000);
		dvr->AddVolume("name1", voxelsTexture, nullptr, dim, false);
		auto vol = dvr->GetVolume("name1");
		vol->SetLongtituteRange(lonRng[0], lonRng[1]);
		vol->SetLatituteRange(latRng[0], latRng[1]);
		vol->SetHeightFromCenterRange(
			static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) + hScale * hRng[0],
			static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) + hScale * hRng[1]);

		dvr->DisplayVolume("name1");
		grp->addChild(dvr->GetGroup());
	}

	void addRadar(llhRange range = llhRange())
	{
		float maxAngle = 91;
		float maxLength = 20;
		float maxSearchRange = 1;
		float minSearchRange = 1;
		float DetectionProbability = 99.9;
		_radarrender = new RadarRender();
		_radarrender->radar->MakeRGridFromFile(std::string(OSG_3D_VIS_DATA_PREFIX) + "cosec2.out");
		_radarrender->radar->SetMaxAngle_length_box(maxAngle, maxLength, maxSearchRange, minSearchRange);
		_radarrender->radar->UpdatePara();
		_radarrender->radar->UpdateDValue();
		_radarrender->SetNewPd(DetectionProbability * 0.01);

		// 将point信息放到空间体素中	
		transPointsToVoxels(_radarrender->m_grid_points, _radarrender->radar->m_curd, range);
		// 将空间体素信息放到Image3D中
		for (int i = 0; i < 256; i++)
			for (int j = 0; j < 256; j++)
				for (int k = 0; k < 256; k++)
					voxelsData[i * 256 * 256 + j * 256 + k] = voxels[j][k][i];
		
	}

	osg::ref_ptr<osg::Geode> OldGenerate(osg::Group* grp, osg::Camera* camera, llhRange range = llhRange())
	{

		float maxAngle = 91;
		float maxLength = 20;
		float maxSearchRange = 1;
		float minSearchRange = 1;
		float DetectionProbability = 99.9;
		_radarrender = new RadarRender();
		_radarrender->radar->MakeRGridFromFile("cosec2.out");
		_radarrender->radar->SetMaxAngle_length_box(maxAngle, maxLength, maxSearchRange, minSearchRange);
		_radarrender->radar->UpdatePara();
		_radarrender->radar->UpdateDValue();
		_radarrender->SetNewPd(DetectionProbability * 0.01);

		// 点绘制模式
		osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> pointGeometry = new osg::Geometry;
		pointGeometry->setVertexAttribArray(0, _radarrender->m_grid_points, osg::Array::BIND_PER_VERTEX);

		osg::ref_ptr<osg::Shader> pointVertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> pointFragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		pointVertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX)+"RadarGridPointsVS.glsl");
		pointFragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarGridPointsPS.glsl");
		osg::ref_ptr<osg::Program> pointProgram = new osg::Program;
		pointProgram->addShader(pointVertexShader);
		pointProgram->addShader(pointFragmentShader);
		osg::ref_ptr<osg::StateSet> pointStateSet = pointGeode->getOrCreateStateSet();
		pointStateSet->setAttributeAndModes(pointProgram);

		// 设置curd
		osg::ref_ptr<osg::Uniform> curdUniform = new osg::Uniform("curd", float(_radarrender->radar->m_curd));
		pointStateSet->addUniform(curdUniform);

		// 绑定mvp
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
		mvpUniform->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
		pointStateSet->addUniform(mvpUniform);

		osg::ref_ptr<osg::DrawArrays> radar_points = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, _radarrender->m_grid_points->size());
		pointGeometry->addPrimitiveSet(radar_points);
		pointGeode->addDrawable(pointGeometry);
		
		// 线绘制模式
		osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> lineGeometry = new osg::Geometry;

		osg::ref_ptr<osg::Vec2Array> m_lines_normals_pos = new osg::Vec2Array();
		osg::ref_ptr<osg::Vec3Array> m_lines_normals_pos3 = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> m_lines_normals_nor = new osg::Vec2Array();


		// 求不同i的情况下，_radarrender->m_lines_normals->at(i).x()中最大值
		double maxX = 0;
		for (int i = 0; i < _radarrender->m_lines_normals->size(); i++)
			if (maxX < _radarrender->m_lines_normals->at(i).x())
				maxX = _radarrender->m_lines_normals->at(i).x();

		for (int i = 0; i < _radarrender->m_lines_normals->size(); i++)
		{
			double transX, transY, transZ;
			llh2xyz_Ellipsoid(range, _radarrender->m_lines_normals->at(i).x() / maxX, 0, _radarrender->m_lines_normals->at(i).y() / maxX, transX, transY, transZ);
			//m_lines_normals_pos->push_back(osg::Vec2(transX, transZ));
			m_lines_normals_pos3->push_back(osg::Vec3(transX, transY, transZ));
			m_lines_normals_pos->push_back(osg::Vec2(_radarrender->m_lines_normals->at(i).x() / maxX, _radarrender->m_lines_normals->at(i).y() / maxX));
			m_lines_normals_nor->push_back(osg::Vec2(_radarrender->m_lines_normals->at(i).z(), _radarrender->m_lines_normals->at(i).w()));
		}
		lineGeometry->setVertexAttribArray(0, m_lines_normals_pos3, osg::Array::BIND_PER_VERTEX);
		lineGeometry->setVertexAttribArray(1, m_lines_normals_nor, osg::Array::BIND_PER_VERTEX);

		osg::ref_ptr<osg::Shader> lineVertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> lineFragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		lineVertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarLineWithNormalVS.glsl");
		lineFragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarLineWithNormalPS.glsl");
		osg::ref_ptr<osg::Program> lineProgram = new osg::Program;
		lineProgram->addShader(lineVertexShader);
		lineProgram->addShader(lineFragmentShader);
		osg::ref_ptr<osg::StateSet> lineStateSet = lineGeode->getOrCreateStateSet();
		lineStateSet->setAttributeAndModes(lineProgram);

		// 设置颜色
		osg::ref_ptr<osg::Uniform> colorUniform = new osg::Uniform("mainColor", osg::Vec4f(78.0 / 255, 201.0 / 255, 176.0 / 255, 1.0));
		lineStateSet->addUniform(colorUniform);

		// 绑定mvp
		lineStateSet->addUniform(mvpUniform);

		osg::ref_ptr<osg::DrawArrays> radar_lines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, m_lines_normals_pos->size());
		lineGeometry->addPrimitiveSet(radar_lines);
		lineGeode->addDrawable(lineGeometry);

		// 面绘制模式
		osg::ref_ptr<osg::Geode> surfaceGeode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> surfaceGeometry = new osg::Geometry;

		surfaceGeometry->setVertexAttribArray(0, m_lines_normals_pos, osg::Array::BIND_PER_VERTEX);
		surfaceGeometry->setVertexAttribArray(1, m_lines_normals_nor, osg::Array::BIND_PER_VERTEX);

		osg::ref_ptr<osg::Shader> surfaceVertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> surfaceFragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		osg::ref_ptr<osg::Shader> surfaceGeometryShader = new osg::Shader(osg::Shader::GEOMETRY);
		surfaceVertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarRoundScanVS.glsl");
		surfaceFragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarRoundScanPS.glsl");
		surfaceGeometryShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "RadarRoundScanGS.glsl");
		osg::ref_ptr<osg::Program> surfaceProgram = new osg::Program;
		surfaceProgram->addShader(surfaceVertexShader);
		surfaceProgram->addShader(surfaceFragmentShader);
		surfaceProgram->addShader(surfaceGeometryShader);
		
		osg::ref_ptr<osg::StateSet> surfaceStateSet = surfaceGeode->getOrCreateStateSet();
		surfaceStateSet->setAttributeAndModes(surfaceProgram);

		// 设置颜色
		surfaceStateSet->addUniform(colorUniform);

		// 设置角度
		const float startAngle = (0.0 / 180.0) * M_PI;
		const float endAngle = (360.0 / 180.0) * M_PI;
		const float stepAngle = (endAngle - startAngle) / 20.0;
		osg::ref_ptr<osg::Uniform> startAngleUniform = new osg::Uniform("startAngle", startAngle);
		surfaceStateSet->addUniform(startAngleUniform);
		osg::ref_ptr<osg::Uniform> stepAngleUniform = new osg::Uniform("stepAngle", stepAngle);
		surfaceStateSet->addUniform(stepAngleUniform);

		// 绑定mvp
		surfaceStateSet->addUniform(mvpUniform);

		// 变换到地球坐标系
		osg::ref_ptr<osg::Uniform> llh_lowerUniform = new osg::Uniform("llh_lower", osg::Vec3f(range.minLatitude * 180 / M_PI, range.minLongtitude * 180 / M_PI, range.minHeight));
		surfaceStateSet->addUniform(llh_lowerUniform);
		osg::ref_ptr<osg::Uniform> llh_upperUniform = new osg::Uniform("llh_upper", osg::Vec3f(range.maxLatitude * 180 / M_PI, range.maxLongtitude * 180 / M_PI, range.maxHeight));
		surfaceStateSet->addUniform(llh_upperUniform);

		osg::ref_ptr<osg::DrawArrays> radar_lines_to_surfaces = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, m_lines_normals_pos->size());
		surfaceGeometry->addPrimitiveSet(radar_lines_to_surfaces);
		surfaceGeode->addDrawable(surfaceGeometry);

		// Blend Rendering Related 使用颜色的ALPHA通道进行透明材质渲染
		surfaceStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		surfaceStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		return surfaceGeode;
	}
}