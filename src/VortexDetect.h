#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include "Util.h"

namespace VortexDetect {
	
	// 参数文件地址
	const string parasPath = std::string(OSG_3D_VIS_CONFIG_PREFIX) +"VortexDetect.txt";

	string vecDataPathU;
	string vecDataPathV;
	int longitudeNum;
	int latitudeNum;
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
			else if (line.find("longitudeNum") != string::npos)
				longitudeNum = stoi(line.substr(line.find("=") + 1));
			else if (line.find("latitudeNum") != string::npos)
				latitudeNum = stoi(line.substr(line.find("=") + 1));
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

	static const int a = 3;
	static const int b = 2;

	static bool is_valid_line(const osg::Vec2& a, const osg::Vec2& b, const vector<osg::Vec2>& speed)
	{
		bool ok = true;
		if (a.x() >= latitudeNum || a.x() < 0 || a.y() >= longitudeNum || a.y() < 0
			|| b.x() >= latitudeNum || b.x() < 0 || b.y() >= longitudeNum || b.y() < 0) return false;
		if (a.x() == b.x())
		{
			for (int i = a.y(); i <= b.y(); ++i)
			{
				if (speed[a.x() * longitudeNum + i].x() == 9999 || speed[a.x() * longitudeNum + i].y() == 9999)
				{
					ok = false;
					break;
				}
			}
		}
		else if (a.y() == b.y())
		{
			for (int i = a.x(); i <= b.x(); i++)
			{
				if (speed[i * longitudeNum + a.y()].x() == 9999 || speed[i * longitudeNum + a.y()].y() == 9999)
				{
					ok = false;
					break;
				}
			}
		}
		return ok;
	}

	static bool is_valid_quad(const osg::Vec2& a, int q, const vector<osg::Vec2>& speed)
	{
		bool ok = true;
		if (a.x() + q >= latitudeNum || a.x() - q < 0 || a.y() + q >= longitudeNum || a.y() - q < 0) return false;
		for (int i = -q; i <= q; i++)//x
		{
			for (int j = -q; j <= q; ++j)//y
			{
				if (speed[(a.x() + i) * longitudeNum + a.y() + j].x() == 9999 || speed[(a.x() + i) * longitudeNum + a.y() + j].y() == 9999)
				{
					ok = false;
					break;
				}
			}
			if (!ok) break;
		}

		return ok;
	}

	int signXY(osg::Vec2& a)
	{
		if (a.x() >= 0 && a.y() >= 0) 
			return 0;
		if (a.x() <= 0 && a.y() >= 0) 
			return 1;
		if (a.x() <= 0 && a.y() <= 0)
			return 2;
		return 3;
	}

	static vector<osg::Vec2> clockwise(vector<osg::Vec2>& speed)
	{
		//1.东西方向的v值
		vector<osg::Vec2> num1;//存索引
		for (int i = 0; i < latitudeNum; i++)
		{
			for (int j = 0; j < longitudeNum - 1; j++)
			{
				if (!is_valid_line(osg::Vec2(i, j - a), osg::Vec2(i, j + 1 + a), speed)) continue;
				if (speed[i * longitudeNum + j].y() >= 0 && speed[i * longitudeNum + j + 1].y() <= 0)
				{
					if (speed[i * longitudeNum + j - a].y() >= 0 && speed[i * longitudeNum + j + 1 + a].y() <= 0
						&& fabs(speed[i * longitudeNum + j].y()) < fabs(speed[i * longitudeNum + j - a].y())
						&& fabs(speed[i * longitudeNum + j + 1].y()) < fabs(speed[i * longitudeNum + j + 1 + a].y()))
					{
						num1.push_back(osg::Vec2(i, j));
						num1.push_back(osg::Vec2(i, j + 1));
					}
				}
			}
		}
		//2.南北方向的u值
		vector<osg::Vec2> num2;
		for (int i = 0; i < num1.size(); i += 2)
		{
			if (!is_valid_line(osg::Vec2(num1[i].x() - a, num1[i].y()), osg::Vec2(num1[i].x() + a, num1[i].y()), speed) ||
				!is_valid_line(osg::Vec2(num1[i].x() - a, num1[i].y() + 1), osg::Vec2(num1[i].x() + a, num1[i].y() + 1), speed))
			{
				continue;
			}

			if (speed[(num1[i].x() + a) * longitudeNum + num1[i].y()].x() >= 0
				&& speed[(num1[i].x() + a) * longitudeNum + num1[i].y() + 1].x() >= 0
				&& fabs(speed[(num1[i].x() + a) * longitudeNum + num1[i].y()].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y()].x())
				&& fabs(speed[(num1[i].x() + a) * longitudeNum + num1[i].y() + 1].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y() + 1].x())
				&& speed[(num1[i].x() - a) * longitudeNum + num1[i].y()].x() <= 0
				&& speed[(num1[i].x() - a) * longitudeNum + num1[i].y() + 1].x() <= 0
				&& fabs(speed[(num1[i].x() - a) * longitudeNum + num1[i].y()].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y()].x())
				&& fabs(speed[(num1[i].x() - a) * longitudeNum + num1[i].y() + 1].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y() + 1].x()))
			{
				num2.push_back(num1[i]);
				num2.push_back(num1[i + 1]);
			}
		}

		//3.找到局部最小值，可能的涡旋中心
		vector<osg::Vec2> nums3;
		set<int> in_nums2;
		for (int i = 0; i < num2.size(); i++)
		{
			in_nums2.insert(num2[i].x() * longitudeNum + num2[i].y());
		}
		set<int> vortexp;
		for (int c = 0; c < num2.size(); ++c)
		{
			osg::Vec2 min_point = osg::Vec2(0, 0);
			osg::Vec2 cur = num2[c];
			float minLength = 1e9;
			if (!is_valid_quad(cur, b, speed)) continue;
			while (1)
			{
				for (int i = -b; i <= b; i++)
				{
					for (int j = -b; j <= b; ++j)
					{
						if (speed[(cur.x() + i) * longitudeNum + cur.y() + j].length() < minLength)
						{
							minLength = speed[(cur.x() + i) * longitudeNum + cur.y() + j].length();
							min_point = osg::Vec2(cur.x() + i, cur.y() + j);
						}

					}
				}
				if (cur.x() == min_point.x() && cur.y() == min_point.y())
				{
					if (vortexp.count(cur.x() * longitudeNum + cur.y()) == 0)
					{
						vortexp.insert(cur.x() * longitudeNum + cur.y());
						nums3.push_back(cur);
					}
					break;
				}
				//如果最小值的点不在num2中，说明这个点不是涡旋中心
				if (in_nums2.count(min_point.x() * longitudeNum + min_point.y()) == 0) //这个要不要加呢？？？？？？？？？？？？？？？？？？
				{
					break;
				}
				else
				{
					cur = min_point;
				}

			}
		}

		//4.两个相邻的速度矢量必须位于相邻的象限内或者两个相邻的象限中
		vector<osg::Vec2> ans;
		for (int c = 0; c < nums3.size(); c++)
		{
			osg::Vec2 cur = nums3[c];
			if (cur.x() + a - 1 >= latitudeNum || cur.x() - a + 1 < 0 || cur.y() + a - 1 >= longitudeNum || cur.y() - a + 1 < 0) continue;
			vector<osg::Vec2> ccw;//逆时针存
			for (int y = cur.y() + a - 1; y >= cur.y() - a + 1; --y)//上
			{
				ccw.push_back(osg::Vec2(cur.x() + a - 1, y));
			}
			for (int x = cur.x() + a - 1; x >= cur.x() - a + 1; --x)//左
			{
				ccw.push_back(osg::Vec2(x, cur.y() - a + 1));
			}
			for (int y = cur.y() - a + 1; y <= cur.y() + a - 1; ++y)//下
			{
				ccw.push_back(osg::Vec2(cur.x() - a + 1, y));
			}
			for (int x = cur.x() - a + 1; x <= cur.x() + a - 1; ++x)//右
			{
				ccw.push_back(osg::Vec2(x, cur.y() + a - 1));
			}

			bool flag = true;
			int signXY0 = signXY(speed[ccw[0].x() * longitudeNum + ccw[0].y()]);
			for (int i = 1; i < ccw.size(); ++i)
			{
				int signXY1 = signXY(speed[ccw[i].x() * longitudeNum + ccw[i].y()]);

				if (signXY0 != signXY1 && (signXY0 + 1) % 4 != signXY1)
				{
					flag = false;
					break;
				}
				signXY0 = signXY1;
			}
			if (flag)
				ans.push_back(cur);
		}
		return ans;

	}

	static vector<osg::Vec2> counterclockwise(vector<osg::Vec2>& speed)
	{
		//1.东西方向的v值
		vector<osg::Vec2> num1;//存索引
		for (int i = 0; i < latitudeNum; i++)
		{
			for (int j = 0; j < longitudeNum - 1; j++)
			{
				if (!is_valid_line(osg::Vec2(i, j - a), osg::Vec2(i, j + 1 + a), speed)) continue;
				if (speed[i * longitudeNum + j].y() <= 0 && speed[i * longitudeNum + j + 1].y() >= 0)
				{
					if (speed[i * longitudeNum + j - a].y() <= 0 && speed[i * longitudeNum + j + 1 + a].y() >= 0
						&& fabs(speed[i * longitudeNum + j].y()) < fabs(speed[i * longitudeNum + j - a].y())
						&& fabs(speed[i * longitudeNum + j + 1].y()) < fabs(speed[i * longitudeNum + j + 1 + a].y()))
					{
						num1.push_back(osg::Vec2(i, j));
						num1.push_back(osg::Vec2(i, j + 1));
					}
				}
			}
		}
		//2.南北方向的u值
		vector<osg::Vec2> num2;
		for (int i = 0; i < num1.size(); i += 2)
		{
			if (!is_valid_line(osg::Vec2(num1[i].x() - a, num1[i].y()), osg::Vec2(num1[i].x() + a, num1[i].y()), speed) ||
				!is_valid_line(osg::Vec2(num1[i].x() - a, num1[i].y() + 1), osg::Vec2(num1[i].x() + a, num1[i].y() + 1), speed))
			{
				continue;
			}

			if (speed[(num1[i].x() + a) * longitudeNum + num1[i].y()].x() <= 0
				&& speed[(num1[i].x() + a) * longitudeNum + num1[i].y() + 1].x() <= 0
				&& fabs(speed[(num1[i].x() + a) * longitudeNum + num1[i].y()].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y()].x())
				&& fabs(speed[(num1[i].x() + a) * longitudeNum + num1[i].y() + 1].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y() + 1].x())
				&& speed[(num1[i].x() - a) * longitudeNum + num1[i].y()].x() >= 0
				&& speed[(num1[i].x() - a) * longitudeNum + num1[i].y() + 1].x() >= 0
				&& fabs(speed[(num1[i].x() - a) * longitudeNum + num1[i].y()].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y()].x())
				&& fabs(speed[(num1[i].x() - a) * longitudeNum + num1[i].y() + 1].x()) > fabs(speed[num1[i].x() * longitudeNum + num1[i].y() + 1].x()))
			{
				num2.push_back(num1[i]);
				num2.push_back(num1[i + 1]);
			}
		}

		//3.找到局部最小值，可能的涡旋中心
		vector<osg::Vec2> nums3;
		set<int> in_nums2;
		for (int i = 0; i < num2.size(); i++)
		{
			in_nums2.insert(num2[i].x() * longitudeNum + num2[i].y());
		}
		set<int> vortexp;
		for (int c = 0; c < num2.size(); ++c)
		{
			osg::Vec2 min_point = osg::Vec2(0, 0);
			osg::Vec2 cur = num2[c];
			float minLength = 1e9;
			if (!is_valid_quad(cur, b, speed)) continue;
			while (1)
			{
				for (int i = -b; i <= b; i++)
				{
					for (int j = -b; j <= b; ++j)
					{
						if (speed[(cur.x() + i) * longitudeNum + cur.y() + j].length() < minLength)
						{
							minLength = speed[(cur.x() + i) * longitudeNum + cur.y() + j].length();
							min_point = osg::Vec2(cur.x() + i, cur.y() + j);
						}

					}
				}
				if (cur.x() == min_point.x() && cur.y() == min_point.y())
				{
					if (vortexp.count(cur.x() * longitudeNum + cur.y()) == 0)
					{
						vortexp.insert(cur.x() * longitudeNum + cur.y());
						nums3.push_back(cur);
					}
					break;
				}
				//如果最小值的点不在num2中，说明这个点不是涡旋中心
				if (in_nums2.count(min_point.x() * longitudeNum + min_point.y()) == 0) //这个要不要加呢？？？？？？？？？？？？？？？？？？
				{
					break;
				}
				else
				{
					cur = min_point;
				}

			}
		}

		//4.两个相邻的速度矢量必须位于相邻的象限内或者两个相邻的象限中
		vector<osg::Vec2> ans;
		for (int c = 0; c < nums3.size(); c++)
		{
			osg::Vec2 cur = nums3[c];
			if (cur.x() + a - 1 >= latitudeNum || cur.x() - a + 1 < 0 || cur.y() + a - 1 >= longitudeNum || cur.y() - a + 1 < 0) continue;
			vector<osg::Vec2> ccw;//逆时针存
			for (int y = cur.y() + a - 1; y >= cur.y() - a + 1; --y)//上
			{
				ccw.push_back(osg::Vec2(cur.x() + a - 1, y));
			}
			for (int x = cur.x() + a - 1; x >= cur.x() - a + 1; --x)//左
			{
				ccw.push_back(osg::Vec2(x, cur.y() - a + 1));
			}
			for (int y = cur.y() - a + 1; y <= cur.y() + a - 1; ++y)//下
			{
				ccw.push_back(osg::Vec2(cur.x() - a + 1, y));
			}
			for (int x = cur.x() - a + 1; x <= cur.x() + a - 1; ++x)//右
			{
				ccw.push_back(osg::Vec2(x, cur.y() + a - 1));
			}

			bool flag = true;
			int signXY0 = signXY(speed[ccw[0].x() * longitudeNum + ccw[0].y()]);
			for (int i = 1; i < ccw.size(); ++i)
			{
				int signXY1 = signXY(speed[ccw[i].x() * longitudeNum + ccw[i].y()]);

				if (signXY0 != signXY1 && (signXY0 + 1) % 4 != signXY1)
				{
					flag = false;
					break;
				}
				signXY0 = signXY1;
			}
			if (flag) 
				ans.push_back(cur);
		}
		return ans;

	}

	
	vector<osg::Vec2> detection(vector<osg::Vec2>& speed)
	{
		vector<osg::Vec2> ans, ans1;
		ans = counterclockwise(speed);
		ans1 = clockwise(speed);

		// 让ans加上ans1
		for (int i = 0; i < ans1.size(); i++)
		{
			ans.push_back(ans1[i]);
		}

		//cout << ans.size() << endl;
		return ans;
	}

	vector<osg::Vec2> vecdata;

	osg::ref_ptr<osg::Geode> Generate(osg::Group* grp, osg::Camera* camera, llhRange range = llhRange(), float alphaCoef = 1.0f)
	{
		osg::Geode* geode = new osg::Geode;
		osg::Geometry* geometry = new osg::Geometry;

		vector<osg::Vec2> vertices = detection(vecdata);
		vector<osg::Vec2> texCoord;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

		for (int i = 0; i < vertices.size(); i++) {
			double x = vertices[i].x() / latitudeNum; 
			double y = vertices[i].y() / longitudeNum;
			double z = 0;
			double tx, ty, tz;
			llh2xyz_Ellipsoid(range,
				x, y, z,
				tx, ty, tz);
			v->push_back(osg::Vec3(tx, ty, tz));
		}
		geometry->setVertexArray(v);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices.size()));
		
		// 设置绘制点的大小
		osg::StateSet* stateSet = geometry->getOrCreateStateSet();
		osg::Point* point = new osg::Point(10.0f);
		stateSet->setAttribute(point);
		
		// 设置绘制点的颜色
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, alphaCoef));
		geometry->setColorArray(colors);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		//stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		//stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		geode->addDrawable(geometry);

		return geode;
	}

	void Generate3D(osg::Group* grp, osg::Camera* camera, llhRange range = llhRange(), int height = 1)
	{
		// 读取参数
		getParas();
		// 读取流场数据
		vecdata = loadVecData(vecDataPathU, vecDataPathV);
		if (height == 1)
		{
			grp->addChild(Generate(grp, camera, range));
			return;
		}
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < vecdata.size(); j++)
			{
				if (i % 3 == 0)
				{
					if (vecdata[j].x() != 9999 && vecdata[j].y() != 9999)
					{
						vecdata[j].x() -= 0.1;
						vecdata[j].y() /= 2;
					}
				}
				else if (i % 3 == 1)
				{
					if (vecdata[j].x() != 9999 && vecdata[j].y() != 9999)
					{
						vecdata[j].x() += 0.3;
						vecdata[j].y() *= 2;
					}
				}
				else if (i % 3 == 2)
				{
					if (vecdata[j].x() != 9999 && vecdata[j].y() != 9999)
					{
						vecdata[j].x() -= 0.2;
						vecdata[j].y() -= 0.01;
					}
				}
			}
			llhRange newRange = range;
			newRange.minHeight = float(i) / height * (range.maxHeight - range.minHeight) + range.minHeight;
			grp->addChild(Generate(grp, camera, newRange, 1.0f / height));
		}
	}

	void Generate3DVertices(vector<osg::Vec3>& vertices, vector<osg::Vec2>& texCoord)
	{
		vertices.resize(0);
		texCoord.resize(0);
		int longitudeNum = 10;
		int latitudeNum = 10;
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

	void GenerateML(osg::Group* grp, osg::Camera* camera, llhRange range = llhRange())
	{
		// 第二次渲染：使用framebuffer的结果作为纹理，用program2进行渲染，渲染到屏幕上
		// 第二次渲染使用的面片
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		vector<osg::Vec3> vertices;
		vector<osg::Vec2> texCoord;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
		Generate3DVertices(vertices, texCoord);

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

		// 第二次渲染使用的program2
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
		vertexShader->loadShaderSourceFromFile("../shader/VelocityLICVS.glsl");
		fragmentShader->loadShaderSourceFromFile("../shader/VortexMLPS.glsl");
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());

		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(geometry.get());
		osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
		stateset->setAttributeAndModes(program);

		// 绑定 texLIC
		auto filename = "../data/vortexML.png";
		osg::Uniform* texLICUniform = new osg::Uniform("texLIC", 2);
		stateset->setTextureAttributeAndModes(2, new osg::Texture2D(osgDB::readImageFile(filename)));
		stateset->addUniform(texLICUniform);

		// 绑定 mvp
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
		mvpUniform->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
		stateset->addUniform(mvpUniform);

		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);//管理深度测试
		//stateset2->setMode(GL_DEPTH, osg::StateAttribute::OFF);//管理深度测试

		// Blend Rendering Related 使用颜色的ALPHA通道进行透明材质渲染
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		grp->addChild(geode);
	}

};