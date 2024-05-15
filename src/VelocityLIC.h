#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include "Util.h"

using namespace std;

namespace VelocityLIC {

	// 参数文件地址
	const string parasPath = string(OSG_3D_VIS_CONFIG_PREFIX) + "VelocityLIC.txt";

	string vecDataPathU;
	string vecDataPathV;
	string noiseTexPath;
	string vertexShaderPath1;
	string fragmentShaderPath1;
	string vertexShaderPath2;
	string fragmentShaderPath2;
	int longitudeNum;
	int latitudeNum;
	int isUniformColor;
	int isSingleMoving;
	int periodT;
	float deltaPhi;
	float alpha;

	void getParas()
	{
		ifstream inputFile(parasPath);

		if (!inputFile.is_open()) {
			cerr << "无法打开参数文件" << endl;
			return;
		}

		// 逐行读取文件内容并解析参数信息
		string line;
		while (getline(inputFile, line)) 
		{
			if (line.find("#") != string::npos)
				continue;
			else if (line.find("vecDataPathU") != string::npos) 
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				vecDataPathU = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("vecDataPathV") != string::npos) 
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				vecDataPathV = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("noiseTexPath") != string::npos) 
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				noiseTexPath = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("vertexShaderPath1") != string::npos) 
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				vertexShaderPath1 = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("fragmentShaderPath1") != string::npos)
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				fragmentShaderPath1 = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("vertexShaderPath2") != string::npos)
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				vertexShaderPath2 = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("fragmentShaderPath2") != string::npos)
			{
				size_t startPos = line.find("\"") + 1;
				size_t endPos = line.find_last_of("\"");
				fragmentShaderPath2 = line.substr(startPos, endPos - startPos);
			}
			else if (line.find("longitudeNum") != string::npos) 
				longitudeNum = stoi(line.substr(line.find("=") + 1));
			else if (line.find("latitudeNum") != string::npos) 
				latitudeNum = stoi(line.substr(line.find("=") + 1));
			else if (line.find("isUniformColor") != string::npos) 
				isUniformColor = stoi(line.substr(line.find("=") + 1));
			else if (line.find("isSingleMoving") != string::npos)
				isSingleMoving = stoi(line.substr(line.find("=") + 1));
			else if (line.find("periodT") != string::npos) 
				periodT = stoi(line.substr(line.find("=") + 1));
			else if (line.find("deltaPhi") != string::npos) 
				deltaPhi = stof(line.substr(line.find("=") + 1));
			else if (line.find("alpha") != string::npos)
				alpha = stof(line.substr(line.find("=") + 1));
			
		}

		// 关闭文件
		inputFile.close();
	}

	vector<osg::Vec2> loadVecData(string str1, string str2)
	{
		vector<osg::Vec2> data;
		ifstream fs(str1, ios::binary);
		if (!fs.is_open())
			return vector<osg::Vec2>();

		float temp;
		vector<float> u(longitudeNum * latitudeNum);
		fs.read((char*)&temp, sizeof(float));
		fs.read(reinterpret_cast<char*>(u.data()), sizeof(float) * longitudeNum * latitudeNum);
		fs.close();

		fs.open(str2, ios::binary);
		if (!fs.is_open())
			return vector<osg::Vec2>();

		vector<float> v(longitudeNum * latitudeNum);
		fs.read((char*)&temp, sizeof(float));
		fs.read(reinterpret_cast<char*>(v.data()), sizeof(float) * longitudeNum * latitudeNum);
		fs.close();

		for (int i = 0; i < longitudeNum * latitudeNum; i++)
		{
			data.push_back(osg::Vec2(u[i], v[i]));
		}
		return data;
	}

	// 第一次渲染的顶点
	void GenerateVertices(vector<osg::Vec2>& vertices)
	{
		vertices.resize(0);

		vertices.push_back(osg::Vec2(-1, 1));
		vertices.push_back(osg::Vec2(1, -1));
		vertices.push_back(osg::Vec2(-1, -1));
		vertices.push_back(osg::Vec2(-1, 1));
		vertices.push_back(osg::Vec2(1, 1));
		vertices.push_back(osg::Vec2(1, -1));
	}

	// 第二次渲染的顶点
	void Generate3DVertices(vector<osg::Vec3>& vertices, vector<osg::Vec2>& texCoord)
	{
		vertices.resize(0);
		texCoord.resize(0);
		double longstep = 1.0f / longitudeNum;
		double latstep = 1.0f / latitudeNum;

		// 绘制小矩形，宽为longstep，长为latstep，共有longitudeNum * latitudeNum个小矩形，铺满整个大矩形
		bool fromLeft = 1;
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

	}

	class PhiUpdateCallback : public osg::Uniform::Callback
	{
	public:
		PhiUpdateCallback(osg::Uniform* phiUniform) : phiUniform_(phiUniform) {}

		virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv)
		{
			if (phiUniform_.valid())
			{
				phi_ += deltaPhi; // 在每帧中增加 phi 的值
				phiUniform_->set(phi_);
			}
		}
	private:
		osg::ref_ptr<osg::Uniform> phiUniform_;
		float phi_ = 0;
	};

	osg::ref_ptr<osg::Geode> Generate(osg::Group* grp, osg::Camera* camera, llhRange range = llhRange())
	{
		// 生成顶点
		vector<osg::Vec2> vertices;
		GenerateVertices(vertices);
		osg::ref_ptr<osg::Vec2Array> v = new osg::Vec2Array();

		for (int i = 0; i < 6; i++)
		{
			float x = vertices[i].x();
			float y = vertices[i].y();
	
			v->push_back(osg::Vec2(x, y));
		}
		
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

		// 读取参数
		getParas();
		// 读取流场数据
		vector <osg::Vec2> vecdata = loadVecData(vecDataPathU, vecDataPathV);

		unsigned char* tempVelocityData = new unsigned char[sizeof(float) * 3 * longitudeNum * latitudeNum];
		for (int i = 0; i < longitudeNum * latitudeNum; i++)
		{
			// 区分正负号：128-255为正，0-127为负，在shader中额外处理
			// 9999为无效值，在tempVelocityData[3 * i + 2]中做出标注
			tempVelocityData[3 * i] = vecdata[i].x() * 127 + 128;
			tempVelocityData[3 * i + 1] = vecdata[i].y() * 127 + 128;
			if(vecdata[i].x() == 9999 || vecdata[i].y() == 9999)
				tempVelocityData[3 * i + 2] = 255.0f;
			else
				tempVelocityData[3 * i + 2] = 0.0f;
		}

		// VelocityImage 为流场数据
		osg::ref_ptr<osg::Image> VelocityImage = new osg::Image();
		// 将tempVelocityData记录到VelocityImage中，形成一张双通道的图片，并绑定到纹理VelocityDataTex中
		VelocityImage->setImage(longitudeNum, latitudeNum, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)tempVelocityData, osg::Image::USE_NEW_DELETE);
		//osgDB::writeImageFile(*VelocityImage, "img2.jpg");

		// 设置 shader
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); 
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		vertexShader->loadShaderSourceFromFile(vertexShaderPath1);
		fragmentShader->loadShaderSourceFromFile(fragmentShaderPath1);
		osg::ref_ptr<osg::Program> program1 = new osg::Program;
		program1->addShader(vertexShader.get());
		program1->addShader(fragmentShader.get());
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;

		osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();

		// 绑定顶点属性
		geometry->setVertexAttribArray(0, v, osg::Array::BIND_PER_VERTEX);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 6));
		geode->addDrawable(geometry.get());
		//geode->setStateSet(stateset);
		
		// 将VelocityImage绑定为Uniform纹理，加入到stateset中
		osg::ref_ptr<osg::Uniform> velocityTex = new osg::Uniform("velocityTex", 1);
		stateset->setTextureAttributeAndModes(1, new osg::Texture2D(VelocityImage), osg::StateAttribute::ON);
		stateset->addUniform(velocityTex);

		// 绑定噪声纹理
		osg::ref_ptr<osg::Uniform> noiseTexUniform = new osg::Uniform("noiseTex", 2);
		stateset->setTextureAttributeAndModes(2, new osg::Texture2D(osgDB::readImageFile(noiseTexPath)), osg::StateAttribute::ON);
		stateset->addUniform(noiseTexUniform);

		// 绑定纹理大小
		osg::ref_ptr<osg::Uniform> texSize = new osg::Uniform("texSize", osg::Vec4f(longitudeNum * 4, latitudeNum * 4, 0, 0));
		stateset->addUniform(texSize);

		// 设置颜色，1为黑白色，0为彩色
		osg::ref_ptr<osg::Uniform> isUniformColorUniform = new osg::Uniform("isUniformColor", isUniformColor);
		stateset->addUniform(isUniformColorUniform);

		// 设置运动或静态，1为运动，0为静态
		osg::ref_ptr<osg::Uniform> isSingleMovingUniform = new osg::Uniform("isSingleMoving", isSingleMoving);
		stateset->addUniform(isSingleMovingUniform);

		// 设置窗函数的周期
		osg::ref_ptr<osg::Uniform> TUniform = new osg::Uniform("T", periodT);
		stateset->addUniform(TUniform);

		// 设置窗函数的相位
		osg::ref_ptr<osg::Uniform> phiUniform = new osg::Uniform("phi", 0.0f);
		stateset->addUniform(phiUniform);

		// 设置透明度
		osg::ref_ptr<osg::Uniform> alphaUniform = new osg::Uniform("alpha", alpha);
		stateset->addUniform(alphaUniform);

		// 设置回调。通过修改相位，让积分结果运动起来
		PhiUpdateCallback* phiUpdateCallback = new PhiUpdateCallback(phiUniform);
		phiUniform->setUpdateCallback(phiUpdateCallback);

		stateset->setAttributeAndModes(program1);

		// 第一次渲染：使用program1进行渲染，但不渲染到屏幕上，而是储存在framebuffer中
		// 使用一个cameraSample来录制第一次渲染的结果，但不渲染到屏幕上
		osg::Camera* cameraSample = new osg::Camera;
		grp->addChild(cameraSample);
		cameraSample->addChild(geode);
		
		osg::ref_ptr<osg::Texture2D> LICTex = new osg::Texture2D;
		// 设置纹理参数
		LICTex->setSourceFormat(GL_RGBA);
		LICTex->setInternalFormat(GL_RGBA);
		LICTex->setSourceType(GL_FLOAT);
		//LICTex->setTextureSize(longitudeNum, latitudeNum);
		LICTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		LICTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		LICTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
		LICTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
		LICTex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::CLAMP_TO_EDGE);
		//cameraSample->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cameraSample->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		cameraSample->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//cameraSample->setProjectionMatrixAsOrtho2D(0, longitudeNum, 0, latitudeNum);
		//cameraSample->setRenderOrder(osg::Camera::PRE_RENDER);

		// 渲染到纹理
		cameraSample->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		cameraSample->setViewport(0, 0, longitudeNum, latitudeNum);
		cameraSample->attach(osg::Camera::COLOR_BUFFER, LICTex);

		

		// 第二次渲染：使用framebuffer的结果作为纹理，用program2进行渲染，渲染到屏幕上
		// 第二次渲染使用的面片
		osg::ref_ptr<osg::Geometry> geometry2 = new osg::Geometry;
		vector<osg::Vec3> vertices2;
		vector<osg::Vec2> texCoord2;
		osg::ref_ptr<osg::Vec3Array> v2 = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt2 = new osg::Vec2Array();
		Generate3DVertices(vertices2, texCoord2);

		for (int i = 0; i < vertices2.size(); i++) {
			double x = vertices2[i].x();
			double y = vertices2[i].y();
			double z = vertices2[i].z();
			double tx, ty, tz;
			llh2xyz_Ellipsoid(range,
				x, y, z,
				tx, ty, tz);
			v2->push_back(osg::Vec3(tx, ty, tz));
			vt2->push_back(texCoord2[i]);
		}
		geometry2->setVertexAttribArray(0, v2.get(), osg::Array::BIND_PER_VERTEX);
		geometry2->setVertexAttribArray(1, vt2.get(), osg::Array::BIND_PER_VERTEX);
		//geometry2->setVertexArray(v2);
		//geometry2->setTexCoordArray(0, texCoord2D);
		//geometry2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
		geometry2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, vertices2.size()));
		
		// 第二次渲染使用的program2
		osg::ref_ptr<osg::Shader> vertexShader2 = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> fragmentShader2 = new osg::Shader(osg::Shader::FRAGMENT);
		vertexShader2->loadShaderSourceFromFile(vertexShaderPath2);
		fragmentShader2->loadShaderSourceFromFile(fragmentShaderPath2);
		osg::ref_ptr<osg::Program> program2 = new osg::Program;
		program2->addShader(vertexShader2.get());
		program2->addShader(fragmentShader2.get());

		osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
		geode2->addDrawable(geometry2.get());
		osg::ref_ptr<osg::StateSet> stateset2 = geode2->getOrCreateStateSet();
		stateset2->setAttributeAndModes(program2);

		//stateset2->setTextureAttributeAndModes(0, LICTex);
		
		// 绑定 texLIC
		osg::Uniform* texLICUniform = new osg::Uniform("texLIC", 2);
		stateset2->setTextureAttributeAndModes(2, LICTex.get(), osg::StateAttribute::ON);
		stateset2->addUniform(texLICUniform);
		
		// 绑定 mvp
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
		mvpUniform->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
		stateset2->addUniform(mvpUniform);

		stateset2->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);//管理深度测试
		//stateset2->setMode(GL_DEPTH, osg::StateAttribute::OFF);//管理深度测试

		// Blend Rendering Related 使用颜色的ALPHA通道进行透明材质渲染
		stateset2->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset2->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		
		return geode2.get();
	}
};