#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>  // 用于 std::setw 和 std::setfill
#include <sstream>  // 用于 std::ostringstream
#include <osgViewer/Viewer>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/MatrixTransform>


using namespace std;
namespace OSGBLoader {
static void findDir(const string& dir, vector<string>& subDirs)
{
	const string dir1 = "Tile_+000+";
	const string dir2 = "Tile_+000-";
	const string dir3 = "Tile_+001+";
	const string dir4 = "Tile_+001-";

	for (int i = 0; i <= 34; ++i) {
		// 使用 std::ostringstream 格式化数字
		std::ostringstream oss{};
		oss << std::setw(3) << std::setfill('0') << i; // 设置宽度为 3，填充字符为 '0'
		subDirs.push_back(dir1+oss.str()); // 将生成的字符串存储到向量中
	}
	for (int i = 1; i <= 21; ++i) {
		// 使用 std::ostringstream 格式化数字
		std::ostringstream oss{};
		oss << std::setw(3) << std::setfill('0') << i; // 设置宽度为 3，填充字符为 '0'
		subDirs.push_back(dir2 + oss.str()); // 将生成的字符串存储到向量中
	}
	for (int i = 0; i <= 34; ++i) {
		// 使用 std::ostringstream 格式化数字
		std::ostringstream oss{};
		oss << std::setw(3) << std::setfill('0') << i; // 设置宽度为 3，填充字符为 '0'
		subDirs.push_back(dir3 + oss.str()); // 将生成的字符串存储到向量中
	}
	for (int i = 1; i <= 9; ++i) {
		// 使用 std::ostringstream 格式化数字
		std::ostringstream oss{};
		oss << std::setw(3) << std::setfill('0') << i; // 设置宽度为 3，填充字符为 '0'
		subDirs.push_back(dir4 + oss.str()); // 将生成的字符串存储到向量中
	}
}

	static std::string DirOrPathGetName(std::string filePath)
	{
		size_t m = filePath.find_last_of('\\');
		if (m == string::npos)
		{
			return filePath;
		}

		size_t p = filePath.find_last_of('.');
		if (p != string::npos && p > m)
		{
			filePath.erase(p);
		}

		std::string dirPath = filePath;
		dirPath.erase(0, m + 1);
		return dirPath;
	}

	void createObliqueIndexes(std::string fileDir)
	{
		string dataDir = fileDir + "/Data";

		osg::ref_ptr<osg::Group> group = new osg::Group();
		vector<string> subDirs;
		findDir(dataDir, subDirs);

		for (size_t i = 0; i < subDirs.size(); i++)
		{
			string name = DirOrPathGetName(subDirs[i]);
			string path = subDirs[i] + "/" + name + ".osgb";

			if (!filesystem::exists(path))
				continue;
			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path);
			osg::ref_ptr<osg::PagedLOD> lod = new osg::PagedLOD();
			if (!node.valid())
				continue;
			//auto bs=node->computeBound();
			auto bs = node->getBound();
			auto c = bs.center();
			auto r = bs.radius();
			lod->setCenter(c);
			lod->setRadius(r);
			lod->setRangeMode(osg::LOD::RangeMode::PIXEL_SIZE_ON_SCREEN);
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			geode->getOrCreateStateSet();
			lod->addChild(geode.get());

			std::string relativeFilePath = "./Data/" + name + "/" + name + ".osgb";

			lod->setFileName(0, "");
			lod->setFileName(1, relativeFilePath);

			lod->setRange(0, 0, 1.0);																							//��һ�㲻�ɼ�
			lod->setRange(1, 1.0, FLT_MAX);

			lod->setDatabasePath("");

			group->addChild(lod);
		}
		std::string outputLodFile = fileDir + "/Data.osgb";
		osgDB::writeNodeFile(*group, outputLodFile);
	}

	osg::ref_ptr<osg::CoordinateSystemNode> LoadFromPath(string path) {
		std::string fileDir = path;
		std::string outputLodFile = fileDir + "/Data.osgb";
		createObliqueIndexes(fileDir);

		osg::ref_ptr<osg::Group> model_root = new osg::Group;

		osg::ref_ptr<osg::CoordinateSystemNode>csn = new osg::CoordinateSystemNode;
		csn->setEllipsoidModel(new osg::EllipsoidModel);
		osg::Matrixd mtTemp;
		// ������Ҫ+180
		// �ο�https://dandelioncloud.cn/article/details/1610866466171863042
		csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(30.2567787), osg::DegreesToRadians(300.2549953), 10, mtTemp);
		//auto pEllModel = new osg::EllipsoidModel();
		//csn->setEllipsoidModel(pEllModel);

		osg::Node* node = new osg::Node;
		node = osgDB::readNodeFile(outputLodFile);

		osg::ref_ptr<osg::MatrixTransform> spMatrixTrans = new osg::MatrixTransform;
		auto matrix = spMatrixTrans->getMatrix();
		// ̫С�˿����壬����ǰ��������ôϸ�¡�������Ϊ10000���塣
		matrix.makeScale(osg::Vec3(1, 1, 1));
		spMatrixTrans->setMatrix(matrix);
		spMatrixTrans->addChild(node);

		double x, y, z;

		//pEllModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(30.2567787), osg::DegreesToRadians(120.2549953), 1000, x, y, z);
		osg::ref_ptr<osg::MatrixTransform> spPosition = new osg::MatrixTransform;
		spPosition->addChild(spMatrixTrans);
		spPosition->setMatrix(mtTemp);

		model_root->addChild(spPosition);

		csn->addChild(model_root);

		return csn.get();
	}
}
