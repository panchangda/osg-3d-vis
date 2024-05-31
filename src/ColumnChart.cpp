#include "ColumnChart.h"

osg::ref_ptr<osg::Geode> ColumnChart::drawQuads(osg::Vec3 location, osg::Vec3 color, float height)
{
	osg::Vec3 ver1 = location;
	osg::Vec3 ver2 = location + osg::Vec3(0.0f, 2.0f, 0.0f);
	osg::Vec3 ver3 = location + osg::Vec3(2.0f, 2.0f, 0.0f);
	osg::Vec3 ver4 = location + osg::Vec3(2.0f, 0.0f, 0.0f);
	osg::Vec3 ver5 = location + osg::Vec3(0.0f, 0.0f, height);
	osg::Vec3 ver6 = location + osg::Vec3(0.0f, 2.0f, height);
	osg::Vec3 ver7 = location + osg::Vec3(2.0f, 2.0f, height);
	osg::Vec3 ver8 = location + osg::Vec3(2.0f, 0.0f, height);

	//下面
	osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v1 = new osg::Vec3Array;
	geom1->setVertexArray(v1.get());
	v1->push_back(ver1);
	v1->push_back(ver2);
	v1->push_back(ver3);
	v1->push_back(ver4);
	geom1->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//上面
	osg::ref_ptr<osg::Geometry> geom2 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v2 = new osg::Vec3Array;
	geom2->setVertexArray(v2.get());
	v2->push_back(ver5);
	v2->push_back(ver6);
	v2->push_back(ver7);
	v2->push_back(ver8);
	geom2->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//前面
	osg::ref_ptr<osg::Geometry> geom3 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v3 = new osg::Vec3Array;
	geom3->setVertexArray(v3.get());
	v3->push_back(ver4);
	v3->push_back(ver3);
	v3->push_back(ver7);
	v3->push_back(ver8);
	geom3->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//后面
	osg::ref_ptr<osg::Geometry> geom4 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v4 = new osg::Vec3Array;
	geom4->setVertexArray(v4.get());
	v4->push_back(ver1);
	v4->push_back(ver2);
	v4->push_back(ver6);
	v4->push_back(ver5);
	geom4->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//左面
	osg::ref_ptr<osg::Geometry> geom5 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v5 = new osg::Vec3Array;
	geom5->setVertexArray(v5.get());
	v5->push_back(ver1);
	v5->push_back(ver4);
	v5->push_back(ver8);
	v5->push_back(ver5);
	geom5->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//右面
	osg::ref_ptr<osg::Geometry> geom6 = new osg::Geometry;
	//创建四个顶点的数组
	osg::ref_ptr<osg::Vec3Array> v6 = new osg::Vec3Array;
	geom6->setVertexArray(v6.get());
	v6->push_back(ver2);
	v6->push_back(ver3);
	v6->push_back(ver7);
	v6->push_back(ver6);
	geom6->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom1.get());
	geode->addDrawable(geom2.get());
	geode->addDrawable(geom3.get());
	geode->addDrawable(geom4.get());
	geode->addDrawable(geom5.get());
	geode->addDrawable(geom6.get());
	return geode.get();

}

osg::ref_ptr<osg::Geometry> ColumnChart::drawFace(osg::ref_ptr<osg::Vec3Array> v, osg::Vec4 color)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geom->setVertexArray(v.get());
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
	c->push_back(color);
	c->push_back(color);
	c->push_back(color);
	c->push_back(color);
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	return geom;
}

osg::ref_ptr<osg::Geode> ColumnChart::generateColumn()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	for (int j = 0; j < 5; j++) {


		//生成一个随机的颜色
		float r = rand() % 100 / 100.f;
		float g = rand() % 100 / 100.f;
		float b = rand() % 100 / 100.f;
		osg::Vec4 color = osg::Vec4(r, g, b, 0.5f);

		for (int i = 0; i < 5; i++) {
			int random = rand() % 20;
			while (random < 1)random = rand() % 20;
			//geode->addDrawable(drawQuads(osg::Vec3(i * 4, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f), random));
			osg::Vec3 location = osg::Vec3(j * 4,i*4 , 0.0f);
			osg::Vec3 ver1 = location;
			osg::Vec3 ver2 = location + osg::Vec3(0.0f, 2.0f, 0.0f);
			osg::Vec3 ver3 = location + osg::Vec3(2.0f, 2.0f, 0.0f);
			osg::Vec3 ver4 = location + osg::Vec3(2.0f, 0.0f, 0.0f);
			osg::Vec3 ver5 = location + osg::Vec3(0.0f, 0.0f, random);
			osg::Vec3 ver6 = location + osg::Vec3(0.0f, 2.0f, random);
			osg::Vec3 ver7 = location + osg::Vec3(2.0f, 2.0f, random);
			osg::Vec3 ver8 = location + osg::Vec3(2.0f, 0.0f, random);


			osg::ref_ptr<osg::Vec3Array> v1 = new osg::Vec3Array;
			v1->push_back(ver1);
			v1->push_back(ver2);
			v1->push_back(ver3);
			v1->push_back(ver4);
			geode->addDrawable(drawFace(v1, color));

			osg::ref_ptr<osg::Vec3Array> v2 = new osg::Vec3Array;
			v2->push_back(ver5);
			v2->push_back(ver6);
			v2->push_back(ver7);
			v2->push_back(ver8);
			geode->addDrawable(drawFace(v2, color));

			osg::ref_ptr<osg::Vec3Array> v3 = new osg::Vec3Array;
			v3->push_back(ver4);
			v3->push_back(ver3);
			v3->push_back(ver7);
			v3->push_back(ver8);
			geode->addDrawable(drawFace(v3, color));

			osg::ref_ptr<osg::Vec3Array> v4 = new osg::Vec3Array;
			v4->push_back(ver1);
			v4->push_back(ver2);
			v4->push_back(ver6);
			v4->push_back(ver5);
			geode->addDrawable(drawFace(v4, color));

			osg::ref_ptr<osg::Vec3Array> v5 = new osg::Vec3Array;
			v5->push_back(ver1);
			v5->push_back(ver4);
			v5->push_back(ver8);
			v5->push_back(ver5);
			geode->addDrawable(drawFace(v5, color));

			osg::ref_ptr<osg::Vec3Array> v6 = new osg::Vec3Array;
			v6->push_back(ver2);
			v6->push_back(ver3);
			v6->push_back(ver7);
			v6->push_back(ver6);
			geode->addDrawable(drawFace(v6, color));
		}
	}
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	return geode;
}
