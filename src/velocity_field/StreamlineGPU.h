#ifndef VelocityFieldGPU_H
#define VelocityFieldGPU_H

#include <osg/Texture2D>
#include <osg/Group>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <cmath>
#include <fstream>
#include <osg/Math>
#include <osgDB/WriteFile>
#include <osg/Geometry>
#include <osg/LineWidth>
#include "../util.h"


namespace VelocityFieldGPU {

	//osg::createTexturedQuadGeometry();
	inline osg::ref_ptr<osg::Geometry>createScreenQuad() {
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
		v->push_back(osg::Vec3(-1, -1, 0));
		v->push_back(osg::Vec3(1, -1, 0));
		v->push_back(osg::Vec3(1, 1, 0));
		v->push_back(osg::Vec3(-1, 1, 0));

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
	inline osg::ref_ptr<osg::Geometry>createScaledScreenQuad() {
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
		v->push_back(osg::Vec3(-1, -1, 0));
		v->push_back(osg::Vec3(1, -1, 0));
		v->push_back(osg::Vec3(1, 1, 0));
		v->push_back(osg::Vec3(-1, 1, 0));

		vt->push_back(osg::Vec2(0.2, 0.2));
		vt->push_back(osg::Vec2(0.8, 0.2));
		vt->push_back(osg::Vec2(0.8, 0.8));
		vt->push_back(osg::Vec2(0.2, 0.8));
		geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
		osg::ref_ptr<osg::DrawArrays> quad = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
		geometry->addPrimitiveSet(quad.get());
		return geometry;
	}
	// �ڶ�����Ⱦ�Ķ���
	inline osg::ref_ptr<osg::Geometry> createEarthQuads(osg_3d_vis::llhRange range, int longitudeNum, int latitudeNum)
	{
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		std::vector<osg::Vec3> vertices;
		std::vector<osg::Vec2> texCoord;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
		vertices.resize(0);
		texCoord.resize(0);
                double longstep = 1.0f / longitudeNum;
                double latstep = 1.0f / latitudeNum;
		/* ����С���Σ���Ϊlongstep����Ϊlatstep������longitudeNum * latitudeNum��С���Σ�������������� */
		bool fromLeft = true;
		for (int i = 0; i < latitudeNum; i++)
		{
			if (fromLeft)
				for (int j = 0; j <= longitudeNum; j++)
				{

					vertices.push_back(osg::Vec3(longstep * j, latstep * i, 0));
					vertices.push_back(osg::Vec3(longstep * j, latstep * (i + 1), 0));
					texCoord.push_back(osg::Vec2(latstep * i, longstep * j));
					texCoord.push_back(osg::Vec2(latstep * (i + 1), longstep * j));
				}
			else
				for (int j = longitudeNum; j >= 0; j--)
				{
					vertices.push_back(osg::Vec3(longstep * j, latstep * i, 0));
					vertices.push_back(osg::Vec3(longstep * j, latstep * (i + 1), 0));
					texCoord.push_back(osg::Vec2(latstep * i, longstep * j));
					texCoord.push_back(osg::Vec2(latstep * (i + 1), longstep * j));
				}
			fromLeft = !fromLeft;
		}

		for (int i = 0; i < vertices.size(); i++) {
			double x = vertices[i].x();
			double y = vertices[i].y();
			double z = vertices[i].z();
			double tx, ty, tz;
			llh2xyz_Ellipsoid(range,
				x, y, z,
				tx, ty, tz);
			v->push_back(osg::Vec3(tx, ty, tz));
			vt->push_back(texCoord[i]);
		}

		geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, vertices.size()));
		return geometry;
	}


	class StreamLineGPU {
	public:
		std::vector<float> x, y, u, v;
		std::vector<osg::Vec2> position;
		osg::ref_ptr<osg::Geode> geode;

		osg::Group* root;

		osg::Camera* mainCamera;

		std::string shaderPath;

		int rttTextureSize;
		int particlesSum;
		int particlesTextureSize;
		int longitudeNum;
		int latitudeNum;
		int tileXSum;
		int tileYSum;

		// wind field parameters as shader uniform
		// calcSpeed Pass uniforms
		osg::Vec3 dimension;
		float pixelSize;
		float speedFactor;
		
		// updatePos Pass uniforms
		float tileXInterval;
		float tileYInterval;
		float dropRate;
		float dropRateBump;
		// 0: random glsl; 1: density; 2:informationEntropy
		int randomSeedSelection;

		// drawLines Pass uniforms
		// 0: straightline, 1: circularArc, 2: BezierCurve
		int curveAlgorithm;
		float hueRange;
		float hueOffset;
		float Saturation;
		float Value;
		float Opacity;
		float lineWidth;
		// line segment subs
		int segmentSubdivisions;

		// blend Pass uniforms
		float decayRate;

		

		osg::ref_ptr<osg::Image> windTexturesUImage;
		osg::ref_ptr<osg::Image> windTexturesVImage;
		osg::ref_ptr<osg::Image> currentParticlesPositionImage;
		osg::ref_ptr<osg::Image> weightedTileImage;
		osg::ref_ptr<osg::Image> translucentImage;
		osg::ref_ptr<osg::Image> translucentImage2;
		osg::ref_ptr<osg::Texture2D> windTexturesUTex;
		osg::ref_ptr<osg::Texture2D> windTexturesVTex;
		osg::ref_ptr<osg::Texture2D> prevParticlesPositionTex;
		osg::ref_ptr<osg::Texture2D> currentParticlesPositionTex;
		osg::ref_ptr<osg::Texture2D> densityWeightedTileTex1D;
		osg::ref_ptr<osg::Texture2D> informEntropyWeightedTileTex1D;
		osg::ref_ptr<osg::Texture2D> particleSpeedTex;
		osg::ref_ptr<osg::Texture2D> nextParticlesPositionTex;
		osg::ref_ptr<osg::Texture2D> linesTex;
		osg::ref_ptr<osg::Texture2D> arrowsTex;
		osg::ref_ptr<osg::Texture2D> lastFrameTex;
		osg::ref_ptr<osg::Texture2D> blendTex;
		osg::ref_ptr<osg::Texture2D> testTex;


		osg::ref_ptr<osg::Camera> calculateSpeedRTTCamera;
		osg::ref_ptr<osg::Camera> updatePositionRTTCamera;
		osg::ref_ptr<osg::Camera> drawLinesRTTCamera;
		osg::ref_ptr<osg::Camera> drawArrowsRTTCamera;
		osg::ref_ptr<osg::Camera> copyCurrentParticleToPrevParticleRTTCamera;
		osg::ref_ptr<osg::Camera> copyNextParticleToCurrentParticleRTTCamera;
		osg::ref_ptr<osg::Camera> blendTexRTTCamera;
		osg::ref_ptr<osg::Camera> copyblendTexToLastFrameTexRTTCamera; 
		osg::ref_ptr<osg::Camera> testCamera;

		osg_3d_vis::llhRange OriginalRange;
		float OriginalMinLat, OriginalMaxLat, OriginalMinLon, OriginalMaxLon;
		float latStart, latInterval, lonStart, lonInterval;

		osg_3d_vis::llhRange range;

		osg::Matrixd viewMatrix;
		
		bool isDrawingFlag;

		bool jumpFrame;

		osg::Timer* timer_t;
		double prevTimeTick;

		void initFromDatFile(std::string str1, std::string str2, osg_3d_vis::llhRange range) {
			std::ifstream fs(str1, std::ios::binary);
			if (!fs.is_open()) std::cout << "Cannot open file from path: " << "str1" << std::endl;
			float temp;
			float _u[52000];
			float _v[52000];
			fs.read((char*)&temp, sizeof(float));
			fs.read((char*)_u, sizeof(float) * 52000);
			fs.close();

			fs.open(str2, std::ios::binary);
			if (!fs.is_open()) std::cout << "Cannot open file from path: " << "str2" << std::endl;
			fs.read((char*)&temp, sizeof(float));
			fs.read((char*)_v, sizeof(float) * 52000);
			fs.close();
			for (int i = 0; i < 52000; i++) {
				u.push_back(_u[i]);
				v.push_back(_v[i]);
			}
			//for (int i = 0; i < 52000; i++) {
			//	if (u[i] == 9999) u[i] = 0;
			//	if (v[i] == 9999) v[i] = 0;
			//}
			float maxU = -1, maxV = -1, minU = 1, minV = 1;
			int uValidNum = 0, vValidNum = 0;
			float uSum = 0, vSum = 0;
			for (int i = 0; i < 52000; i++) {
				if (u[i] == 9999 || u[i]  == 0.0) continue;
				maxU = maxU > u[i] ? maxU : u[i];
				minU = minU < u[i] ? minU : u[i];
				uValidNum++;
				uSum += u[i];
			}
			for (int i = 0; i < 52000; i++) {
				if (v[i] == 9999 || v[i] == 0.0) continue;
				maxV = maxV > v[i] ? maxV : v[i];
				minV = minV < v[i] ? minV : v[i];
				vValidNum++;
				vSum += v[i];
			}
			std::cout << "maxU :" << maxU << ", minU: " << minU << ", avgU: " << uSum / uValidNum << std::endl;
			std::cout << "maxV :" << maxV << ", minV: " << minV << ", avgV: " << vSum / vValidNum << std::endl;


			longitudeNum = 205;
			latitudeNum = 249;

            particlesTextureSize = 128;
			particlesSum = this->particlesTextureSize * this->particlesTextureSize;
            rttTextureSize = 2048;

			longitudeNum = 205;
			latitudeNum = 249;
			tileXSum = 50;
			tileYSum = 50;


			// wind field parameters as shader uniform
			// calcSpeed Pass uniforms
			dimension = osg::Vec3(longitudeNum, latitudeNum, 1);
			pixelSize = 0.5f;
			speedFactor = 0.01f; //0.004f;

			// updatePos Pass uniforms
			tileXInterval = 1.0f / tileXSum;
			tileYInterval = 1.0f / tileXSum;
			dropRate = 0.01f;
			dropRateBump = 0.1f;
			// 0: random glsl; 1: density; 2:informationEntropy
			randomSeedSelection = 0;

			// drawLines Pass uniforms
			// 0: straightline, 1: circularArc, 2: BezierCurve
			curveAlgorithm = 0;
			hueRange = 360.0f;
			hueOffset = 0.0f;
			Saturation = 0.75f;
			Value = 1.0f;
			Opacity = 1.0f;
			lineWidth = 4.0f;
			// line segment subs
            segmentSubdivisions = 3;

			// blend Pass uniforms
            decayRate = 0.98f;

			//root->setDataVariance(osg::Object::DYNAMIC);
			
			this->intializeTexture2Ds();

			viewMatrix = osg::Matrixd();

			isDrawingFlag = true;

			latStart = 0.0f;
			lonStart = 0.0f;
            latInterval = 1.0f;
            lonInterval = 1.0f;

			jumpFrame = true;

			timer_t = new osg::Timer;
			prevTimeTick = timer_t->tick();

		}
		void intializeTexture2Ds() {
			initializeParticlePositionTexture();
			initializeWindFieldTexture();
			initializeTileWeightTexture();
			initializeOtherTextures();
			initializeEmptyImage();
		}
		void initializeOtherTextures(){
			particleSpeedTex = new osg::Texture2D;
			particleSpeedTex->setSourceFormat(GL_RGBA);
			particleSpeedTex->setInternalFormat(GL_RGBA32F_ARB);
			particleSpeedTex->setTextureSize(this->particlesTextureSize, this->particlesTextureSize);
			particleSpeedTex->setSourceType(GL_FLOAT);
			particleSpeedTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			particleSpeedTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

			nextParticlesPositionTex = new osg::Texture2D;
			nextParticlesPositionTex->setTextureSize(this->particlesTextureSize, this->particlesTextureSize);
			nextParticlesPositionTex->setSourceFormat(GL_RGBA);
			nextParticlesPositionTex->setInternalFormat(GL_RGBA32F_ARB);
			nextParticlesPositionTex->setSourceType(GL_FLOAT);
			nextParticlesPositionTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			nextParticlesPositionTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

			testTex = new osg::Texture2D;
			testTex->setSourceFormat(GL_RGBA);
			testTex->setInternalFormat(GL_RGBA);
			testTex->setSourceType(GL_FLOAT);
			testTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			testTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

			linesTex = new osg::Texture2D;
			linesTex->setSourceFormat(GL_RGBA);
			linesTex->setInternalFormat(GL_RGBA32F_ARB);
			linesTex->setSourceType(GL_FLOAT);
			linesTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			linesTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

			arrowsTex = new osg::Texture2D;
			arrowsTex->setSourceFormat(GL_RGBA);
			arrowsTex->setInternalFormat(GL_RGBA32F_ARB);
			arrowsTex->setSourceType(GL_FLOAT);
			arrowsTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			arrowsTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

			lastFrameTex = new osg::Texture2D;
			lastFrameTex->setSourceFormat(GL_RGBA);
			lastFrameTex->setInternalFormat(GL_RGBA);
			lastFrameTex->setSourceType(GL_FLOAT);
			lastFrameTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			lastFrameTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

			blendTex = new osg::Texture2D;
			blendTex->setSourceFormat(GL_RGBA);
			blendTex->setInternalFormat(GL_RGBA);
			blendTex->setSourceType(GL_FLOAT);
			blendTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			blendTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		}
		void initializeEmptyImage(){
			float* translucentColors = new float[sizeof(float) * 4 * this->rttTextureSize * this->rttTextureSize];
			for (int i = 0; i < this->rttTextureSize * this->rttTextureSize; i++) {
				translucentColors[i * 4 + 0] = 0.0f;
				translucentColors[i * 4 + 1] = 0.0f;
				translucentColors[i * 4 + 2] = 0.0f;
				translucentColors[i * 4 + 3] = 0.0f;
			}
			translucentImage = new osg::Image();
			translucentImage->setImage(rttTextureSize, rttTextureSize, 1, GL_RGBA, GL_RGBA, GL_FLOAT, (unsigned char*)translucentColors, osg::Image::USE_NEW_DELETE);

			lastFrameTex->setImage(translucentImage);
			testTex->setImage(translucentImage);
			//lastFrameTex->setDataVariance(osg::Object::DYNAMIC);

		}
		void initializeWindFieldTexture() {
			float* windTexturesU = new float[sizeof(float) * 3 * longitudeNum * latitudeNum];
			float* windTexturesV = new float[sizeof(float) * 3 * longitudeNum * latitudeNum];
			//unsigned char* windTexturesU = new unsigned char[sizeof(float) * 3 * longitudeNum * latitudeNum];
			//unsigned char* windTexturesV = new unsigned char[sizeof(float) * 3 * longitudeNum * latitudeNum];
			for (int i = 0; i < longitudeNum * latitudeNum; i++)
			{
				// ���������ţ�128-255Ϊ����0-127Ϊ������shader�ж��⴦��
				windTexturesU[i * 3] = u[i]/10.0f + 0.5f;
				windTexturesV[i * 3] = v[i]/10.0f + 0.5f;
				//windTexturesU[i * 3] = u[i]*128 + 128;                                                                                                                                                                                                                                                                                                       
				//windTexturesV[i * 3] = v[i]*128 + 128;
				// �����Чֵ�ĵڶ�άΪ0����ЧֵΪ1
				if (u[i] == 9999 || v[i] == 9999)
				{
					windTexturesU[i * 3 + 1] = 1.0f;
					windTexturesV[i * 3 + 1] = 1.0f;
				}
				else
				{
					windTexturesU[i * 3 + 1] = 0.0f;
					windTexturesV[i * 3 + 1] = 0.0f;
				}
			}

			// ���ɷ糡����
			windTexturesUImage = new osg::Image();
			windTexturesVImage = new osg::Image();
			windTexturesUImage->setImage(longitudeNum, latitudeNum, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)windTexturesU, osg::Image::USE_NEW_DELETE);
			windTexturesVImage->setImage(longitudeNum, latitudeNum, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)windTexturesV, osg::Image::USE_NEW_DELETE);
			//windTexturesUImage->setImage(longitudeNum, latitudeNum, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)windTexturesU, osg::Image::USE_NEW_DELETE);
			//windTexturesVImage->setImage(longitudeNum, latitudeNum, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)windTexturesV, osg::Image::USE_NEW_DELETE);
			osgDB::writeImageFile(*windTexturesUImage, "windTexturesUImage.jpg");
			osgDB::writeImageFile(*windTexturesVImage, "windTexturesVImage.jpg");
			

			windTexturesUTex = new osg::Texture2D(windTexturesUImage);
			windTexturesVTex = new osg::Texture2D(windTexturesVImage);
                        windTexturesUTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
                        windTexturesUTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
                        windTexturesVTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
                        windTexturesVTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

		}
		void initializeParticlePositionTexture() {
			float* randomParticles = new float[sizeof(float) * 4 * this->particlesTextureSize * this->particlesTextureSize];
			for (int i = 0; i < this->particlesSum; i++) {
				randomParticles[i * 4 + 0] = float(rand()) / RAND_MAX;
				randomParticles[i * 4 + 1] = float(rand()) / RAND_MAX;
				randomParticles[i * 4 + 2] = 0.0f;
                randomParticles[i * 4 + 3] = 1.0f;
			}
			currentParticlesPositionImage = new osg::Image();
            currentParticlesPositionImage->setImage(particlesTextureSize, particlesTextureSize, 1, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT, (unsigned char*)randomParticles, osg::Image::USE_NEW_DELETE);
			osgDB::writeImageFile(*currentParticlesPositionImage, "currentParticlePositionImage.png");


			currentParticlesPositionTex = new osg::Texture2D(currentParticlesPositionImage);
			currentParticlesPositionTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			currentParticlesPositionTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

			float* randomParticles2 = new float[sizeof(float) * 4 * this->particlesTextureSize * this->particlesTextureSize];
			for (int i = 0; i < this->particlesSum; i++) {
				randomParticles2[i * 4 + 0] = float(rand()) / RAND_MAX;
				randomParticles2[i * 4 + 1] = float(rand()) / RAND_MAX;
				randomParticles2[i * 4 + 2] = 0.0f;
				randomParticles2[i * 4 + 3] = 0.0f;
			}
			osg::ref_ptr<osg::Image> prevParticlesPositionImage = new osg::Image();
            prevParticlesPositionImage->setImage(particlesTextureSize, particlesTextureSize, 1, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT, (unsigned char*)randomParticles2, osg::Image::USE_NEW_DELETE);
			prevParticlesPositionTex = new osg::Texture2D(prevParticlesPositionImage);
			prevParticlesPositionTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			prevParticlesPositionTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
		}
		void initializeTileWeightTexture(){
            int samplerLength = 8192;

			// calculate weightSum
            //float weightSum = tileXSum* tileYSum * 5.0f;
			double densityWeightSum = 0.0f;
			double infoEntropyWeightSum = 0.0f;
			std::vector<float> areaDensity;
			std::vector<float> areaInformationEntropy;
			areaDensity.resize(tileXSum*tileYSum);
			areaInformationEntropy.resize(tileXSum*tileYSum);

			for(int i=0;i<tileXSum; i++){
				for(int j=0;j<tileYSum;j++){
					float density = calculateAreaDensity(i, j);
					areaDensity[i*tileYSum + j] = density;
					densityWeightSum+=density;

					float infoEntropy = calculateAreaInformationEntropy(i, j);
					areaInformationEntropy[i*tileYSum + j] = infoEntropy;
					infoEntropyWeightSum+=infoEntropy;

				}
			}


			// transform xyWeight to 1DTexture
			float interval = samplerLength / densityWeightSum;
			float tmpPos = 0.0f;
			float* densityWeightedTile = new float[sizeof(float) * 3 * samplerLength];
			for (int i = 0; i < tileXSum; i++) {
				for (int j = 0; j < tileYSum; j++) {
                    //float tileWeight = j < tileYSum/2.0f ? 2.0f : 8.0f;
					float tileWeight = areaDensity[i*tileYSum + j];
					float currentTileLength = tileWeight * interval;
					int start = int(tmpPos);
					int end = int(tmpPos + currentTileLength);
					for (int k = start; k < end && k <samplerLength; k++) {
						densityWeightedTile[k * 3 + 0] = float(i) / tileXSum;
						densityWeightedTile[k * 3 + 1] = float(j) / tileYSum;
						densityWeightedTile[k * 3 + 2] = 0.0f;
					}
					tmpPos += currentTileLength;
				}
			}
			weightedTileImage = new osg::Image();
			weightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)densityWeightedTile, osg::Image::USE_NEW_DELETE);
			//osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
			densityWeightedTileTex1D = new osg::Texture2D(weightedTileImage);
			densityWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			densityWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);




			// initialize info Entropy Tex 1D
			interval = samplerLength / infoEntropyWeightSum;
			tmpPos = 0.0f;
			float* infoEntropyWeightedTile = new float[sizeof(float) * 3 * samplerLength];
			for (int i = 0; i < tileXSum; i++) {
				for (int j = 0; j < tileYSum; j++) {
					//float tileWeight = j < tileYSum/2.0f ? 2.0f : 8.0f;
					float tileWeight = areaInformationEntropy[i*tileYSum + j];
					float currentTileLength = tileWeight * interval;
					int start = int(tmpPos);
					int end = int(tmpPos + currentTileLength);
					for (int k = start; k < end && k <samplerLength; k++) {
						infoEntropyWeightedTile[k * 3 + 0] = float(i) / tileXSum;
						infoEntropyWeightedTile[k * 3 + 1] = float(j) / tileYSum;
						infoEntropyWeightedTile[k * 3 + 2] = 0.0f;
					}
					tmpPos += currentTileLength;
				}
			}
			osg::ref_ptr<osg::Image> infoEntropyWeightedTileImage = new osg::Image();
			infoEntropyWeightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)infoEntropyWeightedTile, osg::Image::USE_NEW_DELETE);
			//osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
			informEntropyWeightedTileTex1D = new osg::Texture2D(infoEntropyWeightedTileImage);
			informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);



		}
		float calculateAreaInformationEntropy(int x, int y){
			int lonStart = int(float(x)/tileXSum * longitudeNum);
			int latStart = int(float(y)/tileYSum * latitudeNum);
			int lonEnd = lonStart + int(tileXInterval*longitudeNum);
			int latEnd = latStart + int(tileYInterval*latitudeNum);
			int nSum = (lonEnd - lonStart + 1) * (latEnd - latStart + 1) ;
			float areaDensitySum = 0.0f;
			osg::Vec2 areaUVToward = osg::Vec2();
			for(int i=lonStart;i<=lonEnd;i++){
				for(int j=latStart;j<=latEnd;j++){
					float uVal = std::abs( u[i*latitudeNum + j] );
					float vVal = std::abs( v[i*latitudeNum + j] );
					if(uVal != 9999 && vVal != 9999){
						//areaDensitySum += uVal + vVal;
						areaUVToward += osg::Vec2(uVal, vVal);
					}
				}
			}

			float areaDensityAvg = areaDensitySum / (float) nSum;
			areaUVToward.normalize();
			float areaInformationEntropy = 0.0f;
			for(int i=lonStart;i<=lonEnd;i++){
				for(int j=latStart;j<=latEnd;j++){
					float uVal = std::abs( u[i*latitudeNum + j] );
					float vVal = std::abs( v[i*latitudeNum + j] );
					if(uVal != 9999 && vVal != 9999){
						osg::Vec2 dir(uVal, vVal);
						dir.normalize();
						float dLength = dir.length();
						areaInformationEntropy += osg::square(  dLength * (1.0 - dir * areaUVToward) );
						//areaInformationEntropy += osg::square((uVal + vVal - areaDensityAvg));
					}
				}
			}
			return osg::square(areaInformationEntropy)*10.0f;
		}
		float calculateAreaDensity(int x, int y){
			int lonStart = int(float(x)/tileXSum * longitudeNum);
			int latStart = int(float(y)/tileYSum * latitudeNum);
			int lonEnd = lonStart + int(tileXInterval*longitudeNum);
			int latEnd = latStart + int(tileYInterval*latitudeNum);
			float areaDensity = 0.0f;
			for(int i=lonStart;i<=lonEnd;i++){
				for(int j=latStart;j<=latEnd;j++){
					float uVal = std::abs( u[i*latitudeNum + j] );
					float vVal = std::abs( v[i*latitudeNum + j] );
					if(uVal != 9999 && vVal != 9999){
						if(uVal + vVal > 1.6f)
							areaDensity += uVal + vVal;
					}
				}
			}
			return areaDensity;
		}

		void createCalculateSpeedPass() {
			calculateSpeedRTTCamera = new osg::Camera;
            root->addChild(calculateSpeedRTTCamera.get());
			//auto a = mainCamera->getCullMask();
			//auto b = calculateSpeedRTTCamera->getCullMask();
			//auto c = b & a;
			osg::ref_ptr<osg::Geometry> calculateSpeedPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = calculateSpeedPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
                        vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
                        fragmentShader->loadShaderSourceFromFile(shaderPath+"calculateSpeed.fs");
                        osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
            stateset->setAttributeAndModes(program.get());

                        osg::ref_ptr<osg::Uniform> windTexturesUUniform = new osg::Uniform("U", 0);
                        stateset->setTextureAttributeAndModes(0, windTexturesUTex, osg::StateAttribute::ON);
                        stateset->addUniform(windTexturesUUniform);
                        osg::ref_ptr<osg::Uniform> windTexturesVUniform = new osg::Uniform("V", 1);
                        stateset->setTextureAttributeAndModes(1, windTexturesVTex, osg::StateAttribute::ON);
                        stateset->addUniform(windTexturesVUniform);
                        osg::ref_ptr<osg::Uniform> currentParticlesPositionUniform = new osg::Uniform("currentParticlesPosition", 2);
                        stateset->setTextureAttributeAndModes(2, currentParticlesPositionTex, osg::StateAttribute::ON);
                        stateset->addUniform(currentParticlesPositionUniform);

                        osg::ref_ptr<osg::Uniform> dimensionUniform = new osg::Uniform("dimension", this->dimension);
                        stateset->addUniform(dimensionUniform);
                        osg::ref_ptr<osg::Uniform> pixelSizeUniform = new osg::Uniform("pixelSize", this->pixelSize);
                        stateset->addUniform(pixelSizeUniform);
                        osg::ref_ptr<osg::Uniform> speedFactorUniform = new osg::Uniform("speedFactor", this->speedFactor);
                        stateset->addUniform(speedFactorUniform);

						osg::ref_ptr<osg::Uniform> latStartUniform = new osg::Uniform("latStart", (float) this->latStart);
						stateset->addUniform(latStartUniform);
						osg::ref_ptr<osg::Uniform> lonStartUniform = new osg::Uniform("lonStart", (float) this->lonStart);
						stateset->addUniform(lonStartUniform);
						osg::ref_ptr<osg::Uniform> latIntervalUniform = new osg::Uniform("latInterval", (float) this->latInterval);
						stateset->addUniform(latIntervalUniform);
						osg::ref_ptr<osg::Uniform> lonIntervalUniform = new osg::Uniform("lonInterval", (float) this->lonInterval);
						stateset->addUniform(lonIntervalUniform);

//            stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//            stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

                        osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
                        tmpGeo->addDrawable(calculateSpeedPassGeometry.get());
                        calculateSpeedRTTCamera->removeChildren(0, calculateSpeedRTTCamera->getNumChildren());
                        calculateSpeedRTTCamera->addChild(tmpGeo);


                        calculateSpeedRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 0);
			calculateSpeedRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
                                    calculateSpeedRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        calculateSpeedRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
            calculateSpeedRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            calculateSpeedRTTCamera->setViewport(0, 0, this->particlesTextureSize, this->particlesTextureSize);
                        calculateSpeedRTTCamera->attach(osg::Camera::COLOR_BUFFER, particleSpeedTex.get());

            //calculateSpeedRTTCamera->setPreDrawCallback(new calculateSpeedPassDrawCallback(this));

		}

		void updateCalculateSpeedPass(){
			float t = timer_t->tick();
			jumpFrame = !jumpFrame;
		}

		void createUpdatePositionPass() {
			updatePositionRTTCamera = new osg::Camera;
			root->addChild(updatePositionRTTCamera);

			osg::ref_ptr<osg::Geometry> updatePositionPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = updatePositionPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"updatePosition.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);

			osg::ref_ptr<osg::Uniform> currentParticlesPositionUniform1 = new osg::Uniform("currentParticlesPosition", 0);
			stateset->setTextureAttributeAndModes(0, currentParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(currentParticlesPositionUniform1);

			osg::ref_ptr<osg::Uniform> particlesSpeedUniform = new osg::Uniform("particlesSpeed", 1);
			stateset->setTextureAttributeAndModes(1, particleSpeedTex, osg::StateAttribute::ON);
			stateset->addUniform(particlesSpeedUniform);

			osg::ref_ptr<osg::Uniform> densityWeightedTileTex1DUniform = new osg::Uniform("densityWeightedTileTex1D", 2);
			stateset->setTextureAttributeAndModes(2, densityWeightedTileTex1D, osg::StateAttribute::ON);
			stateset->addUniform(densityWeightedTileTex1DUniform);

			osg::ref_ptr<osg::Uniform> informEntropyWeightedTileTex1DUniform = new osg::Uniform("informEntropyWeightedTileTex1D", 3);
			stateset->setTextureAttributeAndModes(3, informEntropyWeightedTileTex1D, osg::StateAttribute::ON);
			stateset->addUniform(informEntropyWeightedTileTex1DUniform);

			osg::ref_ptr<osg::Uniform> tileXIntervalUniform = new osg::Uniform("tileXInterval", this->tileXInterval);
			stateset->addUniform(tileXIntervalUniform);

			osg::ref_ptr<osg::Uniform> tileYIntervalUniform = new osg::Uniform("tileYInterval", this->tileYInterval);
			stateset->addUniform(tileYIntervalUniform);
			
			osg::ref_ptr<osg::Uniform> randomCoefficientUniform = new osg::Uniform("randomCoefficient", float(rand()) / RAND_MAX);
			stateset->addUniform(randomCoefficientUniform);

			osg::ref_ptr<osg::Uniform> dropRateUniform = new osg::Uniform("dropRate", this->dropRate);
			stateset->addUniform(dropRateUniform);

			osg::ref_ptr<osg::Uniform> dropRateBumpUniform = new osg::Uniform("dropRateBump", this->dropRateBump);
			stateset->addUniform(dropRateBumpUniform);

			osg::ref_ptr<osg::Uniform> randomSeedSelectionUniform = new osg::Uniform("randomSeedSelection", this->randomSeedSelection);
			stateset->addUniform(randomSeedSelectionUniform);
			

			//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(updatePositionPassGeometry.get());

			updatePositionRTTCamera->removeChildren(0, updatePositionRTTCamera->getNumChildren());
			updatePositionRTTCamera->addChild(tmpGeo);

			updatePositionRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 1);
			updatePositionRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
                        updatePositionRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
                        updatePositionRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			updatePositionRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            updatePositionRTTCamera->setViewport(0, 0, this->particlesTextureSize, this->particlesTextureSize);
                        updatePositionRTTCamera->attach(osg::Camera::COLOR_BUFFER, nextParticlesPositionTex.get());

                        updatePositionRTTCamera->setPreDrawCallback(new UpdatePositionPassDrawCallback(this));
		}
		void updateUpdatePositionPass() {

			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (updatePositionRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("randomCoefficient")->set(float(rand()) / RAND_MAX);
		}

		void GenerateAllPixels(osg::ref_ptr<osg::Geometry>DrawLinePassGeometry,osg::ref_ptr<osg::Vec2Array> texCoord, osg::ref_ptr<osg::FloatArray> signs, int size, int segmentSubdivisions)
		{
			double xstep = 1.0f / size;
			double ystep = 1.0f / size;
			double xnow = xstep / 2;
			double ynow = ystep / 2;
			int index = 0;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					for (float k = 0.0; k <= 1.0; k += 1.0f / segmentSubdivisions) {
						texCoord->push_back(osg::Vec2(xnow, ynow));
						signs->push_back(k);
					}
					// create drawArrays with ref_ptr or runtime error
					osg::ref_ptr<osg::DrawArrays> lineStrip = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, index, segmentSubdivisions+1);
					DrawLinePassGeometry->addPrimitiveSet(lineStrip.get());
					index += segmentSubdivisions + 1;
					ynow += ystep;
				}
				xnow += xstep;
				ynow = ystep / 2;
			}
		}
		void createDrawLinesPass() {
			drawLinesRTTCamera = new osg::Camera;
			root->addChild(drawLinesRTTCamera);

			osg::ref_ptr<osg::Geometry> DrawLinePassGeometry = new osg::Geometry;
			osg::ref_ptr<osg::StateSet> stateset = DrawLinePassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Vec2Array> texCoord = new osg::Vec2Array;
			osg::ref_ptr<osg::FloatArray> sign = new osg::FloatArray;

			GenerateAllPixels(DrawLinePassGeometry, texCoord, sign, this->particlesTextureSize, this->segmentSubdivisions);

			DrawLinePassGeometry->setVertexAttribArray(0, texCoord, osg::Array::BIND_PER_VERTEX);
			DrawLinePassGeometry->setVertexAttribArray(1, sign, osg::Array::BIND_PER_VERTEX);


			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
			vertexShader->loadShaderSourceFromFile(shaderPath+"drawSegment.vert");
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
			fragmentShader->loadShaderSourceFromFile(shaderPath+"drawSegment.frag");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			program->addBindAttribLocation("texCoord", 0);
			program->addBindAttribLocation("sign", 1);
			stateset->setAttributeAndModes(program);
			
			stateset->setAttributeAndModes(new osg::LineWidth(lineWidth));
			
			//// texture2Ds
			osg::ref_ptr<osg::Uniform> prevParticlesPositionUniform = new osg::Uniform("prevParticlesPosition", 0);
			stateset->setTextureAttributeAndModes(0, prevParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(prevParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> currentParticlesPositionUniform = new osg::Uniform("currentParticlesPosition", 1);
			stateset->setTextureAttributeAndModes(1, currentParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(currentParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> nextParticlesPositionUniform = new osg::Uniform("nextParticlesPosition", 2);
			stateset->setTextureAttributeAndModes(2, nextParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(nextParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> particleSpeedUniform = new osg::Uniform("particleSpeed", 3);
			stateset->setTextureAttributeAndModes(3, particleSpeedTex, osg::StateAttribute::ON);
			stateset->addUniform(particleSpeedUniform);

			// uniforms
			osg::ref_ptr<osg::Uniform> hueRangeUniform = new osg::Uniform("hueRange", this->hueRange);
			stateset->addUniform(hueRangeUniform);
			osg::ref_ptr<osg::Uniform> hueOffsetmUniform = new osg::Uniform("hueOffset", this->hueOffset);
			stateset->addUniform(hueOffsetmUniform);
			osg::ref_ptr<osg::Uniform> curveAlgorithmUniform = new osg::Uniform("curveAlgorithm", this->curveAlgorithm);
			stateset->addUniform(curveAlgorithmUniform);
			osg::ref_ptr<osg::Uniform> SaturationUniform = new osg::Uniform("S", this->Saturation);
			stateset->addUniform(SaturationUniform);
			osg::ref_ptr<osg::Uniform> ValueUniform = new osg::Uniform("V", this->Value);
			stateset->addUniform(ValueUniform);
			osg::ref_ptr<osg::Uniform> OpacityUniform = new osg::Uniform("Opacity", this->Opacity);
			stateset->addUniform(OpacityUniform);
			

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(DrawLinePassGeometry.get());
			drawLinesRTTCamera->removeChildren(0, drawLinesRTTCamera->getNumChildren());
			drawLinesRTTCamera->addChild(tmpGeo);

			drawLinesRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 2);
			drawLinesRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			drawLinesRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawLinesRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			drawLinesRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            drawLinesRTTCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
			drawLinesRTTCamera->attach(osg::Camera::COLOR_BUFFER, linesTex);
		}

		void GenerateArrows(osg::ref_ptr<osg::Geometry>DrawArrowsPassGeometry,osg::ref_ptr<osg::Vec2Array> texCoord, osg::ref_ptr<osg::FloatArray> signs, int size){
			double xstep = 1.0f / size;
			double ystep = 1.0f / size;
			double xnow = xstep / 2;
			double ynow = ystep / 2;
			int index = 0;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					for(int k = 0; k < 4; k++){
						texCoord->push_back(osg::Vec2(xnow, ynow));
						signs->push_back( (float)k );
					}
					// create drawArrays with ref_ptr or runtime error
					osg::ref_ptr<osg::DrawArrays> arrowLines = new osg::DrawArrays(osg::PrimitiveSet::LINES, index, 4);
					DrawArrowsPassGeometry->addPrimitiveSet(arrowLines.get());
					index += 4;
					ynow += ystep;
				}
				xnow += xstep;
				ynow = ystep / 2;
			}
		}
		void createDrawArrowsPass(){
			drawArrowsRTTCamera = new osg::Camera;
			root->addChild(drawArrowsRTTCamera);

			osg::ref_ptr<osg::Geometry> DrawArrowsPassGeometry = new osg::Geometry;
			osg::ref_ptr<osg::StateSet> stateset = DrawArrowsPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Vec2Array> texCoord = new osg::Vec2Array;
			osg::ref_ptr<osg::FloatArray> sign = new osg::FloatArray;

			GenerateArrows(DrawArrowsPassGeometry, texCoord, sign, this->particlesTextureSize);

			DrawArrowsPassGeometry->setVertexAttribArray(0, texCoord, osg::Array::BIND_PER_VERTEX);
			DrawArrowsPassGeometry->setVertexAttribArray(1, sign, osg::Array::BIND_PER_VERTEX);


			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
			vertexShader->loadShaderSourceFromFile(shaderPath+"drawArrows.vs");
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
			fragmentShader->loadShaderSourceFromFile(shaderPath+"drawArrows.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			program->addBindAttribLocation("texCoord", 0);
			program->addBindAttribLocation("sign", 1);
			stateset->setAttributeAndModes(program);

			//stateset->setAttributeAndModes(new osg::LineWidth(arrowWidth));

			//// texture2Ds
			osg::ref_ptr<osg::Uniform> prevParticlesPositionUniform = new osg::Uniform("prevParticlesPosition", 0);
			stateset->setTextureAttributeAndModes(0, prevParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(prevParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> currentParticlesPositionUniform = new osg::Uniform("currentParticlesPosition", 1);
			stateset->setTextureAttributeAndModes(1, currentParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(currentParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> nextParticlesPositionUniform = new osg::Uniform("nextParticlesPosition", 2);
			stateset->setTextureAttributeAndModes(2, nextParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(nextParticlesPositionUniform);
			osg::ref_ptr<osg::Uniform> particleSpeedUniform = new osg::Uniform("particleSpeed", 3);
			stateset->setTextureAttributeAndModes(3, particleSpeedTex, osg::StateAttribute::ON);
			stateset->addUniform(particleSpeedUniform);

			// uniforms
			osg::ref_ptr<osg::Uniform> arrowLengthUniform = new osg::Uniform("arrowLength", (float)2.0f);
			stateset->addUniform(arrowLengthUniform);
			osg::ref_ptr<osg::Uniform> arrowOpacityUniform = new osg::Uniform("arrowOpacity", (float)0.5f);
			stateset->addUniform(arrowOpacityUniform);

			osg::ref_ptr<osg::Uniform> SaturationUniform = new osg::Uniform("S", this->Saturation);
			stateset->addUniform(SaturationUniform);
			osg::ref_ptr<osg::Uniform> ValueUniform = new osg::Uniform("V", this->Value);
			stateset->addUniform(ValueUniform);
			osg::ref_ptr<osg::Uniform> OpacityUniform = new osg::Uniform("Opacity", this->Opacity);
			stateset->addUniform(OpacityUniform);




			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(DrawArrowsPassGeometry.get());
			drawArrowsRTTCamera->removeChildren(0, drawArrowsRTTCamera->getNumChildren());
			drawArrowsRTTCamera->addChild(tmpGeo);

			drawArrowsRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 3);
			drawArrowsRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			drawArrowsRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawArrowsRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			drawArrowsRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			drawArrowsRTTCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
			drawArrowsRTTCamera->attach(osg::Camera::COLOR_BUFFER, arrowsTex);
		}
		void createCopyCurrentParticleToPrevParticlePass() {
			copyCurrentParticleToPrevParticleRTTCamera = new osg::Camera;
			root->addChild(copyCurrentParticleToPrevParticleRTTCamera);

			osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"samplerTex2D.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);

			osg::ref_ptr<osg::Uniform> currentParticlesPositionUniform = new osg::Uniform("blendedTex", 0);
			stateset->setTextureAttributeAndModes(0, currentParticlesPositionTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(currentParticlesPositionUniform);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(copyPassGeometry.get());
			copyCurrentParticleToPrevParticleRTTCamera->removeChildren(0, copyCurrentParticleToPrevParticleRTTCamera->getNumChildren());
			copyCurrentParticleToPrevParticleRTTCamera->addChild(tmpGeo);

			copyCurrentParticleToPrevParticleRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 4);
			copyCurrentParticleToPrevParticleRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			copyCurrentParticleToPrevParticleRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			copyCurrentParticleToPrevParticleRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			copyCurrentParticleToPrevParticleRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            copyCurrentParticleToPrevParticleRTTCamera->setViewport(0, 0, this->particlesTextureSize, this->particlesTextureSize);
			copyCurrentParticleToPrevParticleRTTCamera->attach(osg::Camera::COLOR_BUFFER, prevParticlesPositionTex);
		}
		void createTestPass() {
			testCamera = new osg::Camera;
			root->addChild(testCamera);
			osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"test.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);

			osg::ref_ptr<osg::Uniform> tex0Uniform = new osg::Uniform("texture0", 0);
			stateset->setTextureAttributeAndModes(0, currentParticlesPositionTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(tex0Uniform);
			osg::ref_ptr<osg::Uniform> tex1Uniform = new osg::Uniform("texture1", 1);
			stateset->setTextureAttributeAndModes(1, prevParticlesPositionTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(tex1Uniform);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(copyPassGeometry.get());
			testCamera->removeChildren(0, testCamera->getNumChildren());
			testCamera->addChild(tmpGeo);

			testCamera->setRenderOrder(osg::Camera::PRE_RENDER, 4);
			testCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			testCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			testCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            testCamera->setViewport(0, 0, this->particlesTextureSize, this->particlesTextureSize);
			testCamera->attach(osg::Camera::COLOR_BUFFER, testTex);
		}
		void createCopyNextParticleToCurrentParticlePass() {
			copyNextParticleToCurrentParticleRTTCamera = new osg::Camera;
			root->addChild(copyNextParticleToCurrentParticleRTTCamera);

			osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"samplerTex2D.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);

			osg::ref_ptr<osg::Uniform> nextParticlesPositionUniform = new osg::Uniform("blendedTex", 0);
			stateset->setTextureAttributeAndModes(0, nextParticlesPositionTex, osg::StateAttribute::ON);
			stateset->addUniform(nextParticlesPositionUniform);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(copyPassGeometry.get());
			copyNextParticleToCurrentParticleRTTCamera->removeChildren(0, copyNextParticleToCurrentParticleRTTCamera->getNumChildren());
			copyNextParticleToCurrentParticleRTTCamera->addChild(tmpGeo);
			copyNextParticleToCurrentParticleRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 5);
			copyNextParticleToCurrentParticleRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			copyNextParticleToCurrentParticleRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			copyNextParticleToCurrentParticleRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			copyNextParticleToCurrentParticleRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            copyNextParticleToCurrentParticleRTTCamera->setViewport(0, 0, this->particlesTextureSize, this->particlesTextureSize);
			copyNextParticleToCurrentParticleRTTCamera->attach(osg::Camera::COLOR_BUFFER, currentParticlesPositionTex);
		}
		void createBlendPass() {
			blendTexRTTCamera = new osg::Camera;
			root->addChild(blendTexRTTCamera);

			osg::ref_ptr<osg::Geometry> BlendPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = BlendPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"blendTex.fs");

			// Create a shader program and attach the shaders
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);

			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());

			osg::ref_ptr<osg::Uniform> uniformLastFrameTex = new osg::Uniform("lastFrameTex", 0);
			stateset->setTextureAttributeAndModes(0, lastFrameTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(uniformLastFrameTex);
			osg::ref_ptr<osg::Uniform> uniformLineTex = new osg::Uniform("lineTex", 1);
			stateset->setTextureAttributeAndModes(1, linesTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(uniformLineTex);
			osg::ref_ptr<osg::Uniform> decayRateUniform = new osg::Uniform("decayRate", this->decayRate);
			stateset->addUniform(decayRateUniform);
			stateset->setAttributeAndModes(program);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(BlendPassGeometry.get());

			blendTexRTTCamera->removeChildren(0, blendTexRTTCamera->getNumChildren());
			blendTexRTTCamera->addChild(tmpGeo);
			blendTexRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			blendTexRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        blendTexRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			blendTexRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 6);
			blendTexRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
                        blendTexRTTCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
			blendTexRTTCamera->attach(osg::Camera::COLOR_BUFFER, blendTex);

			blendTexRTTCamera->setPostDrawCallback(new UpdateblendPassFinalDrawCallback(this));
		}

		void createCopyblendTexToLastFrameTexPass() {
			copyblendTexToLastFrameTexRTTCamera = new osg::Camera;
			root->addChild(copyblendTexToLastFrameTexRTTCamera);

			osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuad.vs");
			fragmentShader->loadShaderSourceFromFile(shaderPath+"samplerTex2D.fs");
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);
			
			osg::ref_ptr<osg::Uniform> blendTexUniform = new osg::Uniform("blendedTex", 0);
			stateset->setTextureAttributeAndModes(0, blendTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(blendTexUniform);

			//Ϊʲô����Ͼͼ���?
			//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			//stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			copyPassGeometry->setStateSet(stateset);

			osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
			tmpGeo->addDrawable(copyPassGeometry.get());
			copyblendTexToLastFrameTexRTTCamera->removeChildren(0, copyblendTexToLastFrameTexRTTCamera->getNumChildren());
			copyblendTexToLastFrameTexRTTCamera->addChild(tmpGeo);

			copyblendTexToLastFrameTexRTTCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
			copyblendTexToLastFrameTexRTTCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			copyblendTexToLastFrameTexRTTCamera->setRenderOrder(osg::Camera::PRE_RENDER, 7);
                        copyblendTexToLastFrameTexRTTCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			copyblendTexToLastFrameTexRTTCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
                        copyblendTexToLastFrameTexRTTCamera->setViewport(0, 0,  this->rttTextureSize,  this->rttTextureSize);
			copyblendTexToLastFrameTexRTTCamera->attach(osg::Camera::COLOR_BUFFER, lastFrameTex);
		}
		void createRenderPass() {
			osg::ref_ptr<osg::Geometry> renderPassGeometry = createEarthQuads(range, longitudeNum/5, latitudeNum/5);
                        //osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
			osg::ref_ptr<osg::StateSet> stateset = renderPassGeometry->getOrCreateStateSet();
			// Create vertex and fragment shaders
			osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
			osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
			vertexShader->loadShaderSourceFromFile(shaderPath+"fullscreenQuadMVP.vs");
            fragmentShader->loadShaderSourceFromFile(shaderPath+"samplerTex2D.fs");

			
			// Create a shader program and attach the shaders
			osg::ref_ptr<osg::Program> program = new osg::Program;
			program->addBindAttribLocation("Vertex", 0);
			program->addBindAttribLocation("TexCoord", 1);
			program->addShader(vertexShader.get());
			program->addShader(fragmentShader.get());
			stateset->setAttributeAndModes(program);

			// �� mvp
			osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
			mvpUniform->setUpdateCallback(new MVPRedrawCallback(mainCamera, this));
			stateset->addUniform(mvpUniform);

			osg::ref_ptr<osg::Uniform> uniformBlendedTex = new osg::Uniform("blendedTex", 0);
            stateset->setTextureAttributeAndModes(0, blendTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(uniformBlendedTex);

			osg::ref_ptr<osg::Uniform> uniformArrowsTex = new osg::Uniform("arrowsTex", 1);
			stateset->setTextureAttributeAndModes(1, arrowsTex.get(), osg::StateAttribute::ON);
			stateset->addUniform(uniformArrowsTex);

			osg::ref_ptr<osg::Uniform> jumpFrameUniform = new osg::Uniform("jumpFrame", this->jumpFrame);
			stateset->addUniform(jumpFrameUniform);

			// Blend Rendering Related ʹ����ɫ��ALPHAͨ������͸��������Ⱦ
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			renderPassGeometry->setStateSet(stateset);

			geode->addDrawable(renderPassGeometry.get());

			//mainCamera->setPreDrawCallback(new UpdateRenderPassDrawCallback(this));
		}

		//void updateRenderPassDrawCB(){
		//	jumpFrame = !jumpFrame;
		//}
		void setRenderPassTextureBindingToNothing(){
			osg::ref_ptr<osg::StateSet> stateset = geode->getDrawable(0)->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, testTex.get(), osg::StateAttribute::ON);
		}

		void updateCalculatePassTextureBindings(double &minLat, double &maxLat, double &minLon, double &maxLon){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (calculateSpeedRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			
			float bias = 5.0f;
			if(minLat < OriginalMinLat+ bias && maxLat > OriginalMaxLat - bias){
				this->latInterval = 1.0f;
				this->latStart = 0.0f;
				stateset->getUniform("latStart")->set((float)this->latStart);
				stateset->getUniform("latInterval")->set((float)this->latInterval);
				minLat = OriginalMinLat;
				maxLat = OriginalMaxLat;

				this->lonInterval = 1.0f;
				this->lonStart = 0.0f;
				stateset->getUniform("lonStart")->set((float)this->lonStart);
				stateset->getUniform("lonInterval")->set((float)this->lonInterval);

				minLon = OriginalMinLon;
				maxLon = OriginalMaxLon;
				return;

			}
			else{

				if(minLat < OriginalMinLat ) minLat = OriginalMinLat;
				float _maxLat = maxLat;
				if(maxLat >OriginalMaxLat) maxLat = OriginalMaxLat;

				this->latInterval = (maxLat - minLat) / (OriginalMaxLat - OriginalMinLat);
				this->latStart = (minLat - OriginalMinLat) / (OriginalMaxLat - OriginalMinLat);
				if(maxLat < OriginalMinLat || minLat > OriginalMaxLat){
					this->latInterval = 0;
					this->latStart = 0;
				}

				stateset->getUniform("latStart")->set((float)this->latStart);
				stateset->getUniform("latInterval")->set((float)this->latInterval);
			}



			if(minLon < OriginalMinLon + bias && maxLon > OriginalMaxLon - bias){
				this->latInterval = 1.0f;
				this->latStart = 0.0f;
				stateset->getUniform("latStart")->set((float)this->latStart);
				stateset->getUniform("latInterval")->set((float)this->latInterval);
				minLat = OriginalMinLat;
				maxLat = OriginalMaxLat;

				this->lonInterval = 1.0f;
				this->lonStart = 0.0f;
				stateset->getUniform("lonStart")->set((float)this->lonStart);
				stateset->getUniform("lonInterval")->set((float)this->lonInterval);

				minLon = OriginalMinLon;
				maxLon = OriginalMaxLon;
				
				return;
			}else{
				if(minLon < OriginalMinLon) minLon = OriginalMinLon;
				if(maxLon > OriginalMaxLon) maxLon = OriginalMaxLon;

				this->lonInterval = (maxLon - minLon) / (OriginalMaxLon - OriginalMinLon);
				this->lonStart = (minLon - OriginalMinLon) / (OriginalMaxLon - OriginalMinLon);
				if(maxLon < OriginalMinLon || minLon > OriginalMaxLon){
					this->latInterval = 0;;
					this->latStart = 0;
				}
				stateset->getUniform("lonStart")->set((float)this->lonStart);
				stateset->getUniform("lonInterval")->set((float)this->lonInterval);
			}
		}


		// below are functions for dynamic resolution texture redrawing

		void updateRenderPassTextureBindings(){
			osg::ref_ptr<osg::StateSet> stateset = geode->getDrawable(0)->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, blendTex.get(), osg::StateAttribute::ON);
		}
		void updateLatLonRange(osg::Vec3 Origin, osg::Vec3 d, double &minLat, double &maxLat, double &minLon, double &maxLon, osg::ref_ptr<osg::EllipsoidModel> m_rpModel){
			osg::Vec3 intersectPoint = osg::Vec3();
			if(getIntersectP(Origin, d, intersectPoint)){
				double lat(0.0), lon(0.0), height(0.0);
				m_rpModel->convertXYZToLatLongHeight(intersectPoint.x(), intersectPoint.y(), intersectPoint.z(), lat, lon, height);
				if(lat > maxLat) maxLat = lat;
				if(lat < minLat) minLat = lat;
				if(lon > maxLon) maxLon = lon;
				if(lon < minLon) minLon = lon;
			}else{
				maxLat = 52.0;
				minLat = -10.0;
				maxLon = 150.0;
				minLon = 99.0;
			}
		}

		bool getIntersectP(osg::Vec3 Origin, osg::Vec3 d, osg::Vec3& IntersectP){
			float a = d*d;
			float b = Origin*d*2;
			float c = Origin*Origin - 6378137.0f * 6378137.0f;
			// b^2 - 4ac
			float b2s4ac = b*b - 4*a*c;
			if( b2s4ac < 0) return false;
			
			// d is always point to sphere
			float t = (-b -  std::sqrt(b2s4ac)) / 2 * a;
			IntersectP =  Origin + d*t ;

			return true;
		}
		void getViewerParameters(double &minLat, double &maxLat, double &minLon, double &maxLon){
			osg::Vec3 eye, center, up;
			mainCamera->getViewMatrixAsLookAt(eye, center, up);
			
			double lon(0.0),lat(0.0),height(0.0);
			osg::ref_ptr<osg::EllipsoidModel> m_rpModel = new osg::EllipsoidModel;

			osg::Vec3 d(-eye.x(), -eye.y(), -eye.z());
			d.normalize();
			//v = -v;

			//osg::Vec3 intersectPoint = d*6378137.0;
			osg::Vec3 intersectPoint = osg::Vec3();
			getIntersectP(eye, d, intersectPoint);

			m_rpModel->convertXYZToLatLongHeight(intersectPoint.x(), intersectPoint.y(), intersectPoint.z(), lat, lon, height);


			double fovy, aspectRatio, zNear, zFar;
			mainCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

			double dis = 6378137.0/sin(osg::DegreesToRadians(fovy/2.0));
			double s = 0.0;
			double scale = 1.0;


			if (eye.length() > dis)
			{
				//˵��Զ�����
				s = acos(6378137.0/eye.length());
				scale = (eye.length()-6378137.0)*tan(osg::DegreesToRadians(fovy/2.0))/(s*6378137.0);
			}
			else
			{

				s = osg::PI - (osg::PI - asin(eye.length()/dis))- osg::DegreesToRadians(fovy/2.0);
			}

			double tempLon(lon),tempLat(lat),dMinLon(lon),dMinLat(lat),dMaxLon(lon),dMaxLat(lat);
			//tempLon = osg::RadiansToDegrees(lon);
			//tempLat = osg::RadiansToDegrees(lat);
			//minLon = osg::RadiansToDegrees(lon - s*scale);
			//maxLon = osg::RadiansToDegrees(lon + s*scale);
			//minLat = osg::RadiansToDegrees(lat - s*scale);
			//maxLat = osg::RadiansToDegrees(lat + s*scale);
			float bias = 0.01f;

			//minLon = osg::RadiansToDegrees(lon - s * aspectRatio - bias);
			//maxLon = osg::RadiansToDegrees(lon + s * aspectRatio + bias);
			//minLat = osg::RadiansToDegrees(lat - s - bias);
			//maxLat = osg::RadiansToDegrees(lat + s + bias);
			//
			//return ; 

			minLon = lon - s * aspectRatio  - bias ;
			maxLon = lon + s * aspectRatio  - bias;
			minLat = lat - s - bias;
			maxLat = lat + s + bias;


			// dir: 
			osg::Vec3 dir = osg::Vec3(center - eye);
			osg::Vec3 right = dir^up;
			dir.normalize();
			up.normalize();
			right.normalize();

			float halfHeight = tan(osg::DegreesToRadians(fovy/2.0)) * zNear;
			float halfWidth = halfHeight * aspectRatio;

			osg::Vec3 zNearPlaneMidPoint = eye + dir*zNear;
			osg::Vec3 rightUpVector = zNearPlaneMidPoint + right*halfWidth + up*halfHeight - eye;
			osg::Vec3 rightDownVector = zNearPlaneMidPoint + right*halfWidth - up*halfHeight - eye;
			osg::Vec3 leftDownVector = zNearPlaneMidPoint - right*halfWidth - up*halfHeight - eye;
			osg::Vec3 leftUpVector = zNearPlaneMidPoint - right*halfWidth + up*halfHeight - eye;

			rightUpVector.normalize();rightDownVector.normalize();leftDownVector.normalize();leftUpVector.normalize();

			updateLatLonRange(eye,rightUpVector, minLat, maxLat, minLon, maxLon, m_rpModel);
			updateLatLonRange(eye,rightDownVector, minLat, maxLat, minLon, maxLon, m_rpModel);
			updateLatLonRange(eye,leftDownVector, minLat, maxLat, minLon, maxLon, m_rpModel);
			updateLatLonRange(eye,leftUpVector, minLat, maxLat, minLon, maxLon, m_rpModel);

			minLon = osg::RadiansToDegrees(minLon);
			maxLon = osg::RadiansToDegrees(maxLon);
			minLat = osg::RadiansToDegrees(minLat);
			maxLat = osg::RadiansToDegrees(maxLat);
		}
		void reDraw(){
			double minLat, maxLat, minLon, maxLon;

			getViewerParameters(minLat, maxLat, minLon, maxLon);

			updateCalculatePassTextureBindings(minLat,maxLat,minLon,maxLon);
			
			this->range = osg_3d_vis::llhRange(minLat, maxLat, minLon, maxLon, range.minHeight, range.maxHeight);
			
			// set LastFrameTex to transparent
			osg::ref_ptr<osg::Image> tmpImage = new osg::Image;
			translucentImage->copySubImage(0, 0, 0, tmpImage);
			translucentImage->dirty();


			//geode->removeDrawables(0, geode->getNumDrawables());
			// 
			createRenderPass();


			setDrawingFlag(true);
			//root->removeChildren(0, root->getNumChildren());
		}


		bool isCameraSteady(osg::Matrixd _viewMatrix){
			return viewMatrix == _viewMatrix;
		}
		void updateMainCameraView(osg::Matrixd _viewMatrix){
			viewMatrix = _viewMatrix;
		}
		void setDrawingFlag(bool flag){
			isDrawingFlag = flag;
		}
		bool isDrawing(){
			return isDrawingFlag;
		}

		void setGeode(osg::ref_ptr < osg::Geode > _geode) {
			this->geode = _geode;
		}
		void setRoot(osg::Group* _root) {
			this->root = _root;
		}
		void setMainCamera(osg::Camera* _camera) {
			this->mainCamera = _camera;
		}
		void setLlhRange(osg_3d_vis::llhRange _range) {
			OriginalMinLat = osg::RadiansToDegrees(_range.minLatitude);
			OriginalMaxLat = osg::RadiansToDegrees(_range.maxLatitude);
			OriginalMinLon = osg::RadiansToDegrees(_range.minLongtitude);
			OriginalMaxLon = osg::RadiansToDegrees(_range.maxLongtitude);
			this->OriginalRange = _range;
			this->range = _range;
		}
		void setShaderPath(std::string _shaderPath){
			this->shaderPath = _shaderPath;
		}

		class UpdatePositionPassDrawCallback : public osg::Camera::DrawCallback {
		public:
			StreamLineGPU* sl;
			UpdatePositionPassDrawCallback(StreamLineGPU* _sl) {
				sl = _sl;
			}
			virtual void operator()(osg::RenderInfo& renderInfo) const
			{
				sl->updateUpdatePositionPass();
			}
		};
		class UpdateblendPassFinalDrawCallback : public osg::Camera::DrawCallback {
		public:
			StreamLineGPU* sl;
			UpdateblendPassFinalDrawCallback(StreamLineGPU* _sl) {
				sl = _sl;
			}
			virtual void operator()(osg::RenderInfo& renderInfo) const
			{
				//osg::State* ss = renderInfo.getState();
				//ss->applyAttribute(sl->blendTex);
				//osg::Image* image = new osg::Image;
				
				//image->readImageFromCurrentTexture(ss->getContextID(), false, GL_RGBA32F_ARB);
				//sl->lastFrameTex->copyTexImage2D(*ss, 0, 0, 2048, 2048);
				//sl->lastFrameTex = new osg::Texture2D(*sl->blendTex, osg::CopyOp::DEEP_COPY_ALL);
				//stateset->setTextureAttributeAndModes(0, lastFrameTex.get(), osg::StateAttribute::ON);
			}
		};
		
		

		class MVPRedrawCallback : public osg::Uniform::Callback{
		public:
			osg::Camera* camera;
			StreamLineGPU* sl;
			MVPRedrawCallback(osg::Camera* _camera, StreamLineGPU *_sl) :
		camera(_camera), sl(_sl) {
			}

			virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
				osg::Matrixd viewMatrix = camera->getViewMatrix();
				osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
				osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * camera->getProjectionMatrix();
				uniform->set(modelViewProjectionMatrix);


				// dynamic resolution redraw: only post to area where camera can see
				//if(sl->isCameraSteady(viewMatrix)){
				//	if(!sl->isDrawing()){
				//		sl->reDraw();
				//		sl->setDrawingFlag(true);
				//	}
				//}else{
				//	sl->updateMainCameraView(viewMatrix);
				//	sl->setRenderPassTextureBindingToNothing();
				//	sl->setDrawingFlag(false);
				//}

			}
		};

		// qt slots
		void updateSpeedFactor(int value){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (calculateSpeedRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("speedFactor")->set(float(value) * 0.001f);
		}
		void updateDropRate(int value){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (updatePositionRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("dropRate")->set(float(value) * 0.01f);
		}
		void updateLineWidth(int value){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (drawLinesRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->setAttributeAndModes(new osg::LineWidth(value * 0.1f));
		}
		void updateDecayRate(int value){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (blendTexRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("decayRate")->set(float(100 - value) * 0.01f);
		}

		void updateRandomSeedSelection(int index){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (updatePositionRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("randomSeedSelection")->set((int)index);
		}
		void updateSmooth(int index){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (drawLinesRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("curveAlgorithm")->set((int)index);
		}
		void updateColorTemplate(int index){
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (drawLinesRTTCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			if(index == 0){
				stateset->getUniform("hueRange")->set( float(360.0f));
				stateset->getUniform("hueOffset")->set( float(0.0f));
			}
			if(index == 1){
				stateset->getUniform("hueRange")->set( float(90.0f));
				stateset->getUniform("hueOffset")->set( float(0.0f));
			}
			if(index == 2){
				stateset->getUniform("hueRange")->set( float(90.0f));
				stateset->getUniform("hueOffset")->set( float(90.0f));
			}
			if(index == 3){
				stateset->getUniform("hueRange")->set( float(90.0f));
				stateset->getUniform("hueOffset")->set( float(180.0f));
			}
			if(index == 4){
				stateset->getUniform("hueRange")->set( float(90.0f));
				stateset->getUniform("hueOffset")->set( float(270.0f));
			}

		}
		
	};


	StreamLineGPU* Generate(osg::Group* root, osg::Camera* camera, osg_3d_vis::llhRange range);

}

#endif


