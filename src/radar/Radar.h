#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>

#include <osg/PrimitiveSet>
#include<osg/PolygonMode>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osg/Group>


#include "../Util.h"
#include "APMRadarRender.h"
#include <iostream>
#include <osgDB/WriteFile>
#include <array>
#include "../volumeRender.h"
using namespace  std;


namespace Radar {
	class VoxelRadar {
	public:
		RadarRender* _radarrender;
		int voxels[256][256][256];
		unsigned char* voxelsData = new unsigned char[sizeof(float) * 256 * 256 * 256];
		osg_3d_vis::llhRange _range;
		inline void initRadar(osg_3d_vis::llhRange llh) { _range = llh; }
		inline void transPointsToVoxels(osg::ref_ptr<osg::Vec3Array> points, float curd, osg_3d_vis::llhRange smallRange)
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

		inline void submitRadar(osg::Group* grp)
		{
			osg::ref_ptr<osg::Texture3D> voxelsTexture = new osg::Texture3D;
			osg::ref_ptr<osg::Image> voxelsImage = new osg::Image;
			voxelsImage->setImage(256, 256, 256, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, voxelsData, osg::Image::USE_NEW_DELETE);
			//voxelsTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			//voxelsTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
			voxelsTexture->setImage(voxelsImage);


			static const std::array<uint32_t, 3> dim = { 256, 256, 256 };
			static const std::array<float, 2> lonRng = { float(_range.minLongtitude / M_PI * 180.f - 360.f), float(_range.maxLongtitude / M_PI * 180.f - 360.f) };
			static const std::array<float, 2> latRng = { float(_range.minLatitude / M_PI * 180.f), float(_range.maxLatitude / M_PI * 180.f) };
			static const std::array<float, 2> hRng = { float(_range.minHeight), float(_range.maxHeight) };
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

		inline void addRadar(osg_3d_vis::llhRange range = osg_3d_vis::llhRange())
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

		inline void ExportRadar()
		{
			std::string filePath = std::string(OSG_3D_VIS_DATA_PREFIX) + "OutPutVoxelData.binary";
			// 打开文件进行二进制写入
			std::ofstream outFile(filePath, std::ios::binary);
			if (!outFile) {
				std::cerr << "Failed to open file: " << filePath << std::endl;
				return;
			}

			// 写入数据到文件
			outFile.write(reinterpret_cast<const char*>(voxelsData), sizeof(float) * 256 * 256 * 256);

			// 关闭文件
			outFile.close();

			if (outFile.good()) {
				std::cout << "Voxel data successfully saved to " << filePath << std::endl;
			}
			else {
				std::cerr << "Failed to write voxel data to " << filePath << std::endl;
			}


		}
	};

	class Radar {
	public:
		Radar(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root);
		RadarRender* _radarrender;
		osg::Uniform* mvpUniform;
		osg::ref_ptr<osg::Camera> camera;
		void setCamera(osg::Camera* cam) {
			camera = cam;
			mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
			mvpUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(camera));
		};
		osg::ref_ptr<osg::Texture2D> color;
		osg::ref_ptr<osg::Texture2D> depth;
		osg::ref_ptr<osg::Camera> forTexture;


		std::vector<osg_3d_vis::llhRange> ranges;
		std::vector<osg::ref_ptr<osg::Geometry>> Geos;
		//for actualy radar draw 
		osg::ref_ptr<osg::Geode> RadarRT;

		std::vector<osg::Vec4> EmiFactors;
		std::vector<osg::ref_ptr<osg::Geometry>> EmiGeos;
		// for EMI draw
		osg::ref_ptr<osg::Geode> Emirt;

		void GenerateRadarMesh();

		void GenerateMeiMesh();

		void Addllh(osg_3d_vis::llhRange range)
		{
			ranges.push_back(range);
			Geos.push_back(Generate(ranges.back()));
		}
		void addEmi(osg::Vec4 fac)
		{
			EmiFactors.push_back(fac);
			osg_3d_vis::llhRange range{ fac.x(), fac.x() + fac.w(), fac.y(),fac.y() + fac.w(), fac.z(),fac.z() };
			EmiGeos.push_back(GenerateEmi(range));
		}
		osg::ref_ptr<osg::Geometry>  Generate(osg_3d_vis::llhRange range);

		osg::ref_ptr<osg::Geometry>  GenerateEmi(osg_3d_vis::llhRange range);

		//for ui part 
		void updateR(double value);
		void updateG(double value);
		void updateB(double value);
		void updateA(double value);
		void updateLineWidth(double value);
		void updateDrawStyle(int index);

		void updateEMIR(double value);
		void updateEMIG(double value);
		void updateEMIB(double value);
		void updateEMIPHI(double value);
		void updateEMITHETA(double value);
	};

};