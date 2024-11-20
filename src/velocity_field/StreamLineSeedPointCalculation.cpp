//
// Created by pcd on 2024/10/25.
//
#include "StreamlineCPU.h"

using namespace osg_3d_vis;

void StreamLineCPU::initializeTileWeightTexture() {

		samplerLength = 8192;
		tileDivideXSum = tileDivideYSum = 50;
		tileDivideXInterval = 1.0f / tileDivideXSum;
		tileDivideYInterval = 1.0f / tileDivideYSum;
		longitudeNum = 205;
		latitudeNum = 249;

		// calculate weightSum
		//float weightSum = tileDivideXSum* tileDivideYSum * 5.0f;
		double densityWeightSum = 0.0f;
		double infoEntropyWeightSum = 0.0f;
		std::vector<float> areaDensity;
		std::vector<float> areaInformationEntropy;
		areaDensity.resize(tileDivideXSum * tileDivideYSum);
		areaInformationEntropy.resize(tileDivideXSum * tileDivideYSum);

		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				float density = calculateAreaDensity(i, j);
				areaDensity[i * tileDivideYSum + j] = density;
				densityWeightSum += density;

				float infoEntropy = calculateAreaInformationEntropy(i, j);
				areaInformationEntropy[i * tileDivideYSum + j] = infoEntropy;
				infoEntropyWeightSum += infoEntropy;

			}
		}


		// transform xyWeight to 1DTexture
		float interval = samplerLength / densityWeightSum;
		float tmpPos = 0.0f;
		//float* densityWeightedTile = new float[sizeof(float) * 3 * samplerLength];
		densityWeightedTile.resize(3 * samplerLength);
		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				//float tileWeight = j < tileDivideYSum/2.0f ? 2.0f : 8.0f;
				float tileWeight = areaDensity[i * tileDivideYSum + j];
				float currentTileLength = tileWeight * interval;
				int start = int(tmpPos);
				int end = int(tmpPos + currentTileLength);
				for (int k = start; k < end && k < samplerLength; k++) {
					densityWeightedTile[k * 3 + 0] = float(i) / tileDivideXSum;
					densityWeightedTile[k * 3 + 1] = float(j) / tileDivideYSum;
					densityWeightedTile[k * 3 + 2] = 0.0f;
				}
				tmpPos += currentTileLength;
			}
		}
		//weightedTileImage = new osg::Image();
		//weightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)densityWeightedTile, osg::Image::USE_NEW_DELETE);
		////osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
		//densityWeightedTileTex1D = new osg::Texture2D(weightedTileImage);
		//densityWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
		//densityWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);




		// initialize info Entropy Tex 1D
		interval = samplerLength / infoEntropyWeightSum;
		tmpPos = 0.0f;
		//float* infoEntropyWeightedTile = new float[sizeof(float) * 3 * samplerLength];
		infoEntropyWeightedTile.resize( 3 * samplerLength);
		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				//float tileWeight = j < tileDivideYSum/2.0f ? 2.0f : 8.0f;
				float tileWeight = areaInformationEntropy[i * tileDivideYSum + j];
				float currentTileLength = tileWeight * interval;
				int start = int(tmpPos);
				int end = int(tmpPos + currentTileLength);
				for (int k = start; k < end && k < samplerLength; k++) {
					infoEntropyWeightedTile[k * 3 + 0] = float(i) / tileDivideXSum;
					infoEntropyWeightedTile[k * 3 + 1] = float(j) / tileDivideYSum;
					infoEntropyWeightedTile[k * 3 + 2] = 0.0f;
				}
				tmpPos += currentTileLength;
			}
		}
		//osg::ref_ptr<osg::Image> infoEntropyWeightedTileImage = new osg::Image();
		//infoEntropyWeightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)infoEntropyWeightedTile, osg::Image::USE_NEW_DELETE);
		//osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
		//informEntropyWeightedTileTex1D = new osg::Texture2D(infoEntropyWeightedTileImage);
		//informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
		//informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);



	}

	/*
	*
	Information Entropy is Calculated by Single Vector's Dir Diff Between Area Vsectors' Sum(or Avg) Dir
	Area Information Entropy is the Sum of the Information Entropy
	*
	*/

	float StreamLineCPU::calculateAreaInformationEntropy(int x, int y) {
		int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
		int latStart = int(float(y) / tileDivideYSum * latitudeNum);
		int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
		int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
		int nSum = (lonEnd - lonStart + 1) * (latEnd - latStart + 1);
		float areaDensitySum = 0.0f;
		osg::Vec2 areaUVToward = osg::Vec2();
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
					//areaDensitySum += uVal + vVal;
					areaUVToward += osg::Vec2(uVal, vVal);
				}
			}
		}

		float areaDensityAvg = areaDensitySum / (float)nSum;
		areaUVToward.normalize();
		float areaInformationEntropy = 0.0f;
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
					osg::Vec2 dir(uVal, vVal);
					dir.normalize();
					areaInformationEntropy += osg::square(1.0 - dir * areaUVToward);
					//areaInformationEntropy += osg::square((uVal + vVal - areaDensityAvg));
				}
			}
		}
		return areaInformationEntropy;
	}
	float StreamLineCPU::calculateAreaDensity(int x, int y) {
		int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
		int latStart = int(float(y) / tileDivideYSum * latitudeNum);
		int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
		int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
		float areaDensity = 0.0f;
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
						areaDensity += osg::square(uVal + vVal);
				}
			}
		}
		return areaDensity;
	}






	void StreamLineCPU::initializeTileWeightTexture3D() {

    samplerLength = 409600;
    tileDivideXSum = tileDivideYSum = tileDivideZSum = 50;
    tileDivideXInterval = 1.0f / tileDivideXSum;
    tileDivideYInterval = 1.0f / tileDivideYSum;
    tileDivideZInterval = 1.0f / tileDivideZSum;
    longitudeNum = 205;
    latitudeNum = 249;
    leverageNum = 199;  // 新增的深度方向的分割

    double densityWeightSum = 0.0f;
    double infoEntropyWeightSum = 0.0f;
    std::vector<float> areaDensity;
    std::vector<float> areaInformationEntropy;
    areaDensity.resize(tileDivideXSum * tileDivideYSum * tileDivideZSum);
    areaInformationEntropy.resize(tileDivideXSum * tileDivideYSum * tileDivideZSum);

    for (int i = 0; i < tileDivideXSum; i++) {
        for (int j = 0; j < tileDivideYSum; j++) {
            for (int k = 0; k < tileDivideZSum; k++) {
                float density = calculateAreaDensity3D(i, j, k);
                areaDensity[i * tileDivideYSum * tileDivideZSum + j * tileDivideZSum + k] = density;
                densityWeightSum += density;

                float infoEntropy = calculateAreaInformationEntropy3D(i, j, k);
                areaInformationEntropy[i * tileDivideYSum * tileDivideZSum + j * tileDivideZSum + k] = infoEntropy;
                infoEntropyWeightSum += infoEntropy;
            }
        }
    }

    // 将 xyWeight 转换为 1D 纹理
    float interval = samplerLength / densityWeightSum;
    float tmpPos = 0.0f;
    densityWeightedTile.resize(3 * samplerLength);
    for (int i = 0; i < tileDivideXSum; i++) {
        for (int j = 0; j < tileDivideYSum; j++) {
            for (int k = 0; k < tileDivideZSum; k++) {
                float tileWeight = areaDensity[i * tileDivideYSum * tileDivideZSum + j * tileDivideZSum + k];
                float currentTileLength = tileWeight * interval;
                int start = int(tmpPos);
                int end = int(tmpPos + currentTileLength);
                for (int l = start; l < end && l < samplerLength; l++) {
                    densityWeightedTile[l * 3 + 0] = float(i) / tileDivideXSum;
                    densityWeightedTile[l * 3 + 1] = float(j) / tileDivideYSum;
                    densityWeightedTile[l * 3 + 2] = float(k) / tileDivideZSum;
                }
                tmpPos += currentTileLength;
            }
        }
    }

    // 类似地，对信息熵进行初始化
    interval = samplerLength / infoEntropyWeightSum;
    tmpPos = 0.0f;
    infoEntropyWeightedTile.resize(3 * samplerLength);
    for (int i = 0; i < tileDivideXSum; i++) {
        for (int j = 0; j < tileDivideYSum; j++) {
            for (int k = 0; k < tileDivideZSum; k++) {
                float tileWeight = areaInformationEntropy[i * tileDivideYSum * tileDivideZSum + j * tileDivideZSum + k];
                float currentTileLength = tileWeight * interval;
                int start = int(tmpPos);
                int end = int(tmpPos + currentTileLength);
                for (int l = start; l < end && l < samplerLength; l++) {
                    infoEntropyWeightedTile[l * 3 + 0] = float(i) / tileDivideXSum;
                    infoEntropyWeightedTile[l * 3 + 1] = float(j) / tileDivideYSum;
                    infoEntropyWeightedTile[l * 3 + 2] = float(k) / tileDivideZSum;
                }
                tmpPos += currentTileLength;
            }
        }
    }
}

float StreamLineCPU::calculateAreaInformationEntropy3D(int x, int y, int z) {
    int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
    int latStart = int(float(y) / tileDivideYSum * latitudeNum);
    int depthStart = int(float(z) / tileDivideZSum * leverageNum);
    int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
    int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
    int depthEnd = depthStart + int(tileDivideZInterval * leverageNum);
    int nSum = (lonEnd - lonStart + 1) * (latEnd - latStart + 1) * (depthEnd - depthStart + 1);
    osg::Vec3 areaUVWToward = osg::Vec3();
    for (int i = lonStart; i <= lonEnd; i++) {
        for (int j = latStart; j <= latEnd; j++) {
            for (int k = depthStart; k <= depthEnd; k++) {
                float uVal = std::abs(u[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float vVal = std::abs(v[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float wVal = std::abs(w[i * latitudeNum * leverageNum + j * leverageNum + k]);
                if (uVal != 9999 && vVal != 9999 && wVal != 9999) {
                    areaUVWToward += osg::Vec3(uVal, vVal, wVal/100.0f);
                }
            }
        }
    }

    areaUVWToward.normalize();
    float areaInformationEntropy = 0.0f;
    for (int i = lonStart; i <= lonEnd; i++) {
        for (int j = latStart; j <= latEnd; j++) {
            for (int k = depthStart; k <= depthEnd; k++) {
                float uVal = std::abs(u[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float vVal = std::abs(v[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float wVal = std::abs(w[i * latitudeNum * leverageNum + j * leverageNum + k]);
                if (uVal != 9999 && vVal != 9999 && wVal != 9999) {
                    osg::Vec3 dir(uVal, vVal, wVal);
                    dir.normalize();
                    areaInformationEntropy += osg::square(1.0 - dir * areaUVWToward);
                }
            }
        }
    }
    return areaInformationEntropy;
}

float StreamLineCPU::calculateAreaDensity3D(int x, int y, int z) {
    int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
    int latStart = int(float(y) / tileDivideYSum * latitudeNum);
    int depthStart = int(float(z) / tileDivideZSum * leverageNum);
    int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
    int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
    int depthEnd = depthStart + int(tileDivideZInterval * leverageNum);
    float areaDensity = 0.0f;
    for (int i = lonStart; i <= lonEnd; i++) {
        for (int j = latStart; j <= latEnd; j++) {
            for (int k = depthStart; k <= depthEnd; k++) {
                float uVal = std::abs(u[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float vVal = std::abs(v[i * latitudeNum * leverageNum + j * leverageNum + k]);
                float wVal = std::abs(w[i * latitudeNum * leverageNum + j * leverageNum + k]);
                if (uVal != 9999 && vVal != 9999 && wVal != 9999) {
                    areaDensity += osg::square(uVal + vVal + wVal);
                }
            }
        }
    }
    return areaDensity;
}
