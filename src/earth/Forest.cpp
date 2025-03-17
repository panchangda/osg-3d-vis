//
// Created by pcd on 2025/2/27.
//

#include "Forest.h"

#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/TexEnv>
#include <osgUtil/LineSegmentIntersector>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osg/AlphaFunc>
#include <osg/PositionAttitudeTransform>

#include "terrain_coords.h"
#include "../globals.h"
#include "../particles/Particle.h"


namespace osg_3d_vis{

    // Tree GeometryCollector，为了优化树木的绘制效率
    class TreeGeometryCollector : public osg::NodeVisitor
    {
    public:
        TreeGeometryCollector()
                : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

        void apply(osg::Geode& geode) override
        {
            for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
            {
                osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
                if (geom) geometryList.push_back(geom);
            }
            traverse(geode);
        }

        std::vector< osg::ref_ptr<osg::Geometry> > geometryList;
    };



    // NodeVisitor，用于在读取好的OBJ中抓取所有 Geometry，由于读取的OBJ中有多颗树木，因此需要先对其进行分离
    class GeometryCollector : public osg::NodeVisitor
    {
    public:
        GeometryCollector()
            : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

        void apply(osg::Geode& geode) override
        {
            for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
            {
                osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
                if (geom) geometryList.push_back(geom);
            }
            // 继续遍历子节点
            traverse(geode);
        }

        std::vector<osg::ref_ptr<osg::Geometry>> geometryList;
    };

    Forest::Forest(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera) {

        /* Terrain: */
        osg::Vec3d origin(0.0f,0.0f,0.0f);
        osg::Vec3 size(1000.0f,1000.0f,200.0f);
        if(drawEarth) {
            // 6) 若要贴地球，需要一个经纬度范围
            //   （你应根据实际数据决定）
            // origin & size 变为经纬高
            double latMin = 30.0, latMax = 30.5;
            double lonMin = 120.0, lonMax = 120.5;

            origin = llh2xyz_Ellipsoid(osg::DegreesToRadians((latMin + latMax) * 0.5f),
                                       osg::DegreesToRadians((lonMin + lonMax) * 0.5f + 180.0f),
                                       1000.0f);
            origin = osg::Vec3d((latMin + latMax) * 0.5f, (lonMin + lonMax) * 0.5f, 10.0f);
            size = osg::Vec3d(0.5f,0.5f,2000.0f);

        }else {
            // for earth size treescale match
            size*=100.0f;
        }
        osg::ref_ptr<osg::Node> terrain = createTerrain(origin,size);
        _root->addChild(terrain.get());


        /* Trees: */

        TreeList trees;
        int numTreesToCreate = 5000;
        const int numTreesTypes = 7;
        int treeScale = 30.0f;
        const std::string trees_file = std::string(OSG_3D_VIS_DATA_PREFIX) + "gpu-instances/trees9.obj";


        createTreeList(terrain.get(),origin,size,numTreesToCreate,numTreesTypes, trees, true, treeScale);

        osg::ref_ptr<osg::Node> trees_node = osgDB::readNodeFile(trees_file.c_str());


        if (!trees_node) {
            OSG_FATAL << "Failed to load" << trees_file <<  std::endl;
            return ;
        }
        // 收集几何
        GeometryCollector collector;
        trees_node->accept(collector);
        if (collector.geometryList.empty()) {
            OSG_FATAL << "No geometry found in the model: " << trees_node << std::endl;
            return;
        }

        auto tree_geoms = collector.geometryList;
        // osg::ref_ptr<osg::Geometry> baseGeom =
        //    new osg::Geometry(*collector.geometryList.front(), osg::CopyOp::DEEP_COPY_ALL);



        // 处理树木Transform并分别加入场景
        for(auto const t:trees) {
            osg::MatrixTransform* mt = new osg::MatrixTransform;

            // 1) 默认的“上方向”是 (0,0,1)
            osg::Vec3 defaultUp(0.0f, 0.0f, 1.0f);

            // 2) 计算“上方向”到“地形法线(t->_normal)”的四元数
            osg::Quat q;
            q.makeRotate(defaultUp, t->_normal);

            mt->setMatrix(
                osg::Matrix::scale(t->_width,t->_width,t->_height)
                *osg::Matrix::rotate(q)
                *osg::Matrix::translate(t->_position));


            mt->addChild(tree_geoms[t->_type]);
            _root->addChild(mt);
        }

//        // 3) 调用合并函数
//        osg::ref_ptr<osg::Geometry> mergedTreesGeom = mergeTreeGeometry(trees_node.get(), trees);
//
//        // 4) 若创建成功，则放到一个 Geode 中，然后加到场景根节点
//        if (mergedTreesGeom.valid())
//        {
//            osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//            geode->addDrawable( mergedTreesGeom );
//            _root->addChild( geode );
//        }

        // use billboard trees
        // unsigned int maxNumTreesPerCell = sqrtf(static_cast<float>(numTreesToCreate));
        // osg::ref_ptr<Cell> treeCell = new Cell;
        // treeCell->addTrees(trees);
        // treeCell->divide(maxNumTreesPerCell);
        // osg::Texture2D *tree_tex = new osg::Texture2D;
        // tree_tex->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP );
        // tree_tex->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP );
        //
        // const std::string treeImage = "Images/tree0.rgba";
        //
        // tree_tex->setImage(osgDB::readRefImageFile("Images/tree0.rgba"));
        //
        // osg::StateSet *tree_state = new osg::StateSet;
        // {
        //     tree_state->setTextureAttributeAndModes(0, tree_tex, osg::StateAttribute::ON );
        //
        //     tree_state->setTextureAttribute(0, new osg::TexEnv );
        //
        //     tree_state->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
        //
        //     osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
        //     alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
        //     tree_state->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );
        //
        //     tree_state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        //
        //     tree_state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        // }
        //
        // _root->addChild(createBillboardGraph(treeCell.get(),tree_state));



        /* Grasses: */

//        TreeList grasses;
//        int numGrassToCreate = 20000;
//        const int numGrassTypes = 1;
//        int grassScale = 500.0f;

//        createTreeList(terrain.get(),origin,size,numGrassToCreate,numGrassTypes, grasses, true, grassScale);

//        unsigned int maxNumGrassesPerCell = sqrtf(static_cast<float>(numGrassToCreate));
//        osg::ref_ptr<Cell> cell = new Cell;
//        cell->addTrees(grasses);
//        cell->divide(maxNumGrassesPerCell);

//        osg::Texture2D *tex = new osg::Texture2D;
//        tex->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP );
//        tex->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP );

//        const std::string grassImage = std::string(OSG_3D_VIS_DATA_PREFIX) + "gpu-instances/grass.png";

//        tex->setImage(osgDB::readRefImageFile(grassImage));

//        osg::StateSet *dstate = new osg::StateSet;
//        {
//            dstate->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON );

//            dstate->setTextureAttribute(0, new osg::TexEnv );

//            dstate->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );

//            osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
//            alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
//            dstate->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );

//            dstate->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

//            dstate->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
//        }

//        _root->addChild(createBillboardGraph(cell.get(),dstate));



        /* Create Particle Effects */
        {
            osg::Vec3 wind(1.0f,0.0f,0.0f);

            TreeList particleEffects;
            int numParticleEffectsToCreate = 10;
            const int numparticleEffectTypes = 5;
            int particleEffectScale = 3000.0f;

            createTreeList(terrain.get(),origin,size,
                numParticleEffectsToCreate,numparticleEffectTypes, particleEffects,
                true, particleEffectScale);


            for(auto const p:particleEffects) {

                osg::Vec3d position(p->_position);
                osgParticle::ExplosionEffect* explosion = new osgParticle::ExplosionEffect(position, particleEffectScale);
                osgParticle::ExplosionDebrisEffect* explosionDebri = new osgParticle::ExplosionDebrisEffect(position, particleEffectScale);
                osgParticle::SmokeEffect* smoke = new osgParticle::SmokeEffect(position, particleEffectScale);
                osgParticle::FireEffect* fire = new osgParticle::FireEffect(position, particleEffectScale);

                explosion->setWind(wind);
                explosionDebri->setWind(wind);
                smoke->setWind(wind);
                fire->setWind(wind);

                _root->addChild(explosion);
                _root->addChild(explosionDebri);
                _root->addChild(smoke);
                _root->addChild(fire);
            }
        }

        /* Create Weather */
        // 创建一个降水效果
        osg::ref_ptr<osgParticle::PrecipitationEffect> precipitationEffect = new osgParticle::PrecipitationEffect;

        //设置雪花浓度
        precipitationEffect->snow(1.0f);
        // 设置雨
        precipitationEffect->rain(1.0f);

        precipitationEffect->setUseFarLineSegments(false);

        // 可以设置风向，控制雨/雪飘落方向
        precipitationEffect->setWind(osg::Vec3(1.0f, 0.0f, 0.0f));

        // 如果需要更改粒子大小或纹理，也可以在 precipitation 内部做一些调整
        precipitationEffect->setParticleSize(0.5f);
        // precipitation->setParticleColor( ... );
        // precipitation->setNearTransition( ... );

        osg::Vec3d terrainCenter = llh2xyz_Ellipsoid(osg::DegreesToRadians(origin.x()),
                                                       osg::DegreesToRadians(origin.y() + 180.0f),
                                                       1000.0f);
        precipitationEffect->setPosition(terrainCenter);

        _root->addChild(precipitationEffect.get());



    }


osg::Geode* Forest::createTerrain(osg::Vec3d& origin, osg::Vec3& size)
{
    osg::Geode* geode = new osg::Geode();

    // ---------------------------------------
    // Set up a StateSet to texture the objects
    // ---------------------------------------
    osg::StateSet* stateset = new osg::StateSet();

    osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile("Images/lz.rgb");
    if (image)
    {
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setImage(image);
        stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    }

    geode->setStateSet( stateset );

    unsigned int numColumns = 38;
    unsigned int numRows = 39;
    unsigned int r;
    unsigned int c;

    // compute z range of z values of grid data so we can scale it.
    float min_z = FLT_MAX;
    float max_z = -FLT_MAX;
    for(r=0;r<numRows;++r)
    {
        for(c=0;c<numColumns;++c)
        {
            min_z = osg::minimum(min_z,osg_example_terrain_vertex[r+c*numRows][2]);
            max_z = osg::maximum(max_z,osg_example_terrain_vertex[r+c*numRows][2]);
        }
    }

    float scale_z = size.z()/(max_z-min_z);


    bool createGrid = false;
    if (createGrid)
    {

        osg::HeightField* grid = new osg::HeightField;
        grid->allocate(numColumns,numRows);
        grid->setOrigin(origin);
        grid->setXInterval(size.x()/(float)(numColumns-1));
        grid->setYInterval(size.y()/(float)(numRows-1));

        for(r=0;r<numRows;++r)
        {
            for(c=0;c<numColumns;++c)
            {
                float rawZ = osg_example_terrain_vertex[r + c * numRows][2];
                float mappedZ = (rawZ - min_z) * scale_z;
                grid->setHeight(c, r, mappedZ);
            }
        }
        geode->addDrawable(new osg::ShapeDrawable(grid));
    }
    else
    {
        osg::Geometry* geometry = new osg::Geometry;

        osg::Vec3Array& v = *(new osg::Vec3Array(numColumns*numRows));
        osg::Vec2Array& t = *(new osg::Vec2Array(numColumns*numRows));
        osg::Vec4ubArray& color = *(new osg::Vec4ubArray(1));

        color[0].set(255,255,255,255);

        float rowTexDelta = 1.0f/(float)(numRows-1);
        float columnTexDelta = 1.0f/(float)(numColumns-1);

        osg::Vec3 pos = origin;
        osg::Vec2 tex(0.0f,0.0f);

        // 5) 如果要本地平铺，使用下述 delta
        float rowCoordDelta    = size.y() / (float)(numRows    - 1);
        float columnCoordDelta = size.x() / (float)(numColumns - 1);

        // 可以使用一个已创建的椭球, 或临时 new:
        osg::ref_ptr<osg::EllipsoidModel> ellipsoid = new osg::EllipsoidModel();

        // 7) 填充顶点坐标 v[i]
        int vi = 0;
        for(r = 0; r < numRows; ++r)
        {
            // rowFrac 用于插值(0..1)
            float rowFrac = (float)r / (float)(numRows - 1);

            for(c = 0; c < numColumns; ++c)
            {
                float colFrac = (float)c / (float)(numColumns - 1);

                // 取原始数据：注意 `r + c*numRows` => z
                float rawZ = osg_example_terrain_vertex[r + c*numRows][2];
                float mappedZ = (rawZ - min_z)*scale_z; // 归一化到 [0..size.z()]

                // 纹理坐标(若你用贴图)
                t[vi].set(colFrac, rowFrac);

                if (drawEarth)
                {
                    // 贴到地球
                    double latDeg = origin.x() + rowFrac*(size.x());
                    double lonDeg = origin.y() + colFrac*(size.y());


                    // double x, y, z;
                    // ellipsoid->convertLatLongHeightToXYZ(
                    //     osg::DegreesToRadians(latDeg),
                    //     osg::DegreesToRadians(lonDeg),
                    //     (double)mappedZ+10000.0f,  // alt
                    //     x, y, z
                    // );
                    // v[vi].set((float)x, (float)y, (float)z);


                    osg::Vec3d res = llh2xyz_Ellipsoid(osg::DegreesToRadians(latDeg),
                                                       osg::DegreesToRadians(lonDeg + 180.0f),
                                                       mappedZ* 3.0f);
                    v[vi].set((float)res.x(), (float)res.y(), (float)res.z());


                }
                else
                {
                    // 本地平铺
                    float localX = origin.x() + colFrac * size.x();
                    float localY = origin.y() + rowFrac * size.y();
                    float localZ = origin.z() + mappedZ;

                    v[vi].set(localX, localY, localZ);
                }

                vi++;
            }
        }

        geometry->setVertexArray(&v);
        geometry->setColorArray(&color, osg::Array::BIND_OVERALL);
        geometry->setTexCoordArray(0,&t);
        geometry->setUseDisplayList(false);
        geometry->setUseVertexBufferObjects(true);

        for(r=0;r<numRows-1;++r)
        {
            osg::DrawElementsUShort& drawElements = *(new osg::DrawElementsUShort(GL_QUAD_STRIP,2*numColumns));
            geometry->addPrimitiveSet(&drawElements);
            int ei=0;
            for(c=0;c<numColumns;++c)
            {
                drawElements[ei++] = (r+1)*numColumns+c;
                drawElements[ei++] = (r)*numColumns+c;
            }
        }

        geode->addDrawable(geometry);

        osgUtil::SmoothingVisitor sv;
        sv.smooth(*geometry);
    }

    return geode;
}
    void Forest::createTreeList(osg::Node* terrain,const osg::Vec3& origin, const osg::Vec3& size,
        unsigned int numTreesToCreate,unsigned int numTreesTypes, TreeList& trees,
        bool uniformScale, unsigned int scale)
    {

        float max_TreeHeight = sqrtf(size.length2()/(float)numTreesToCreate);
        float max_TreeWidth = max_TreeHeight*0.5f;

        float min_TreeHeight = max_TreeHeight*0.3f;
        float min_TreeWidth = min_TreeHeight*0.5f;

        // set uniform scale for tree objs
        if(uniformScale) {
            min_TreeHeight = max_TreeHeight = scale;
            min_TreeWidth = max_TreeWidth = scale;
        }



        trees.reserve(trees.size()+numTreesToCreate);


        for(unsigned int i=0;i<numTreesToCreate;++i)
        {
            Tree* tree = new Tree;
            osg::Vec3d random_tree_pos(random(origin.x(),origin.x()+size.x()),random(origin.y(),origin.y()+size.y()), 0.0f);
            osg::Vec3d upper_pos = random_tree_pos + osg::Vec3(0.0f, 0.0f, size.z());
            if(drawEarth) {
                upper_pos = llh2xyz_Ellipsoid(
                    osg::DegreesToRadians(random_tree_pos.x()),
                    osg::DegreesToRadians(random_tree_pos.y() + 180.0f),
                    random_tree_pos.z() + 10000.0f);
                random_tree_pos = llh2xyz_Ellipsoid(osg::DegreesToRadians(random_tree_pos.x()),
                    osg::DegreesToRadians(random_tree_pos.y() + 180.0f), random_tree_pos.z());
            }

            tree->_position.set(random_tree_pos);
            tree->_color.set(random(128,255),random(128,255),random(128,255),255);
            tree->_width = random(min_TreeWidth,max_TreeWidth);
            tree->_height = random(min_TreeHeight,max_TreeHeight);
            tree->_type = random(0, numTreesTypes-1);

            if (terrain)
            {
                osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
                    new osgUtil::LineSegmentIntersector(random_tree_pos,upper_pos);

                osgUtil::IntersectionVisitor iv(intersector.get());

                terrain->accept(iv);

                if (intersector->containsIntersections())
                {
                    osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
                    for(osgUtil::LineSegmentIntersector::Intersections::iterator itr = intersections.begin();
                        itr != intersections.end();
                        ++itr)
                    {
                        const osgUtil::LineSegmentIntersector::Intersection& intersection = *itr;
                        tree->_position = intersection.getWorldIntersectPoint();


                        // 关键：获取地形法线，并归一化
                        osg::Vec3 normal = intersection.getWorldIntersectNormal();
                        if (std::isnan(normal.x()) || std::isnan(normal.y()) || std::isnan(normal.z()) ||
    normal.length2() < 1e-12)
                        {
                            normal.set(0.0f, 0.0f, 1.0f); // 默认朝上
                        }
                        else
                        {
                            normal.normalize();
                        }

                        tree->_normal = normal; // 存下来，后面构造矩阵用



                    }
                    trees.push_back(tree);
                }
            }


        }
    }

    /// \brief 将树模型(多几何)合并到一个 Geometry，然后对每棵树做矩阵变换，合并到最终大网格
/// \param modelNode 加载的.obj树模型（可能内部包含多个 Geometry）
/// \param trees     树的位姿、缩放等信息
/// \return 合并后的 Geometry
    osg::ref_ptr<osg::Geometry> Forest::mergeTreeGeometry(
            osg::Node* modelNode,
            const TreeList trees )
    {
        // 1) 收集原始几何
        GeometryCollector collector;
        modelNode->accept(collector);
        if (collector.geometryList.empty())
        {
            osg::notify(osg::FATAL) << "[mergeTreeGeometry] No geometry in modelNode" << std::endl;
            return nullptr;
        }

        // 为了示例，假设 modelNode 中只有 1 个或者少量 geom，这里只处理第一个 Geometry
        osg::Geometry* baseGeom = collector.geometryList.front().get();

        // 2) 从 baseGeom 中获取源顶点、法线、纹理坐标等
        auto* srcVerts   = dynamic_cast<osg::Vec3Array*>( baseGeom->getVertexArray() );
        auto* srcNormals = dynamic_cast<osg::Vec3Array*>( baseGeom->getNormalArray() );

        // 如果还有纹理坐标、颜色数组，也类似拿到
        // auto* srcTexcoords = dynamic_cast<osg::Vec2Array*>( baseGeom->getTexCoordArray(0) );

        if (!srcVerts)
        {
            osg::notify(osg::WARN) << "[mergeTreeGeometry] baseGeom has no vertices!" << std::endl;
            return nullptr;
        }

        // 3) 创建“目标”Geometry及其顶点法线缓冲
        osg::ref_ptr<osg::Vec3Array> mergedVerts   = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> mergedNormals = new osg::Vec3Array;

        // 也可以收集索引，如果源 geom 用的是 DrawElements，则需要合并索引
        // 为示例简单，假设是 DrawArrays(TRIANGLES) 或其他非索引模式
        // 处理索引逻辑时要注意 baseIndex offset

        // 4) 遍历所有“树实例”，将源几何“复制 + 变换”到 mergedVerts / mergedNormals 中
        for (auto tree : trees)
        {
            // 计算树的变换矩阵
            // 1) 默认 up vector(0,0,1) 到 tree->_normal 的旋转
            osg::Quat q;
            q.makeRotate(osg::Vec3(0,0,1), tree->_normal);

            osg::Matrix mat =
                    osg::Matrix::scale(tree->_width, tree->_width, tree->_height) *
                    osg::Matrix::rotate(q) *
                    osg::Matrix::translate(tree->_position);

            // 将 source 的所有顶点复制一遍到 merged
            for (unsigned int i = 0; i < srcVerts->size(); ++i)
            {
                osg::Vec3 v = (*srcVerts)[i] * mat; // 应用变换
                mergedVerts->push_back(v);

                if (srcNormals && srcNormals->size() > i)
                {
                    // 法线只应用旋转部分(不包含平移,不建议包含缩放; 如果要缩放,需要mat的逆转置)
                    osg::Vec3 n = (*srcNormals)[i] * mat;
                    n.normalize();
                    mergedNormals->push_back(n);
                }
            }
        }

        // 5) 创建最终的合并 Geometry
        osg::ref_ptr<osg::Geometry> mergedGeom = new osg::Geometry;
        mergedGeom->setVertexArray(mergedVerts);
        if (mergedNormals->size() == mergedVerts->size())
        {
            mergedGeom->setNormalArray(mergedNormals, osg::Array::BIND_PER_VERTEX);
        }

        // 设定图元类型。例如假设 baseGeom 每次是三角形
        // (如果 baseGeom->getPrimitiveSet(i) 是 DrawArrays(TRIANGLES, ...), 直接复制)
        // 这里演示：把所有三角形数目合并后，作为一个 DrawArrays
        unsigned int totalVerts = mergedVerts->size();
        mergedGeom->addPrimitiveSet(
                new osg::DrawArrays(GL_TRIANGLES, 0, totalVerts)
        );

        // 6) 一些优化
        // 关闭 DisplayList, 开启 VBO
        mergedGeom->setUseDisplayList(false);
        mergedGeom->setUseVertexBufferObjects(true);

        // 也可以调用平滑处理
        // osgUtil::SmoothingVisitor::smooth(*mergedGeom);

        return mergedGeom;
    }


    osg::Geometry* Forest::createSprite( float w, float h, osg::Vec4ub color )
    {
        // set up the coords
        osg::Vec3Array& v = *(new osg::Vec3Array(4));
        osg::Vec2Array& t = *(new osg::Vec2Array(4));
        osg::Vec4ubArray& c = *(new osg::Vec4ubArray(1));

        v[0].set(-w*0.5f,0.0f,0.0f);
        v[1].set( w*0.5f,0.0f,0.0f);
        v[2].set( w*0.5f,0.0f,h);
        v[3].set(-w*0.5f,0.0f,h);

        c[0] = color;

        t[0].set(0.0f,0.0f);
        t[1].set(1.0f,0.0f);
        t[2].set(1.0f,1.0f);
        t[3].set(0.0f,1.0f);

        osg::Geometry *geom = new osg::Geometry;

        geom->setVertexArray( &v );

        geom->setTexCoordArray( 0, &t );

        geom->setColorArray( &c, osg::Array::BIND_OVERALL );

        geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4) );

        return geom;

    }
    osg::Node* Forest::createBillboardGraph(Cell* cell,osg::StateSet* stateset)
    {
        bool needGroup = !(cell->_cells.empty());
        bool needBillboard = !(cell->_trees.empty());

        osg::Billboard* billboard = 0;
        osg::Group* group = 0;

        if (needBillboard)
        {
            billboard = new osg::Billboard;
            billboard->setStateSet(stateset);

            // 将 avg_normal 初始化为 (0,0,0)
            osg::Vec3d avg_normal(0.0f, 0.0f, 0.0f);

            for(TreeList::iterator itr=cell->_trees.begin();
                itr!=cell->_trees.end();
                ++itr)
            {
                Tree& tree = **itr;
                billboard->addDrawable(createSprite(tree._width,tree._height,tree._color),tree._position);
                avg_normal+= tree._normal;
            }

            // 一个 cell 内的草朝向相同
            avg_normal.normalize();
            billboard->setMode(osg::Billboard::AXIAL_ROT);

            billboard->setAxis(avg_normal); // 这里 avg_normal 应该是经过累加后反映地形的上向
            // billboard->setAxis(osg::Vec3(0.0,1.0,0.0));
            // billboard->setNormal(avg_normal);
            billboard->setNormal(osg::Vec3(0.0,0.0,-1.0)); // 设置与精灵默认法线一致
        }

        if (needGroup)
        {
            group = new osg::Group;
            for(Cell::CellList::iterator itr=cell->_cells.begin();
                itr!=cell->_cells.end();
                ++itr)
            {
                group->addChild(createBillboardGraph(itr->get(),stateset));
            }

            if (billboard) group->addChild(billboard);

        }
        if (group) return group;
        else return billboard;
    }

    void Forest::Cell::computeBound()
    {
        _bb.init();
        for(CellList::iterator citr=_cells.begin();
            citr!=_cells.end();
            ++citr)
        {
            (*citr)->computeBound();
            _bb.expandBy((*citr)->_bb);
        }

        for(TreeList::iterator titr=_trees.begin();
            titr!=_trees.end();
            ++titr)
        {
            _bb.expandBy((*titr)->_position);
        }
    }


bool Forest::Cell::divide(unsigned int maxNumTreesPerCell)
{

    if (_trees.size()<=maxNumTreesPerCell) return false;

    computeBound();

    float radius = _bb.radius();
    float divide_distance = radius*0.7f;
    if (divide((_bb.xMax()-_bb.xMin())>divide_distance,(_bb.yMax()-_bb.yMin())>divide_distance,(_bb.zMax()-_bb.zMin())>divide_distance))
    {
        // recusively divide the new cells till maxNumTreesPerCell is met.
        for(CellList::iterator citr=_cells.begin();
            citr!=_cells.end();
            ++citr)
        {
            (*citr)->divide(maxNumTreesPerCell);
        }
        return true;
   }
   else
   {
        return false;
   }
}

bool Forest::Cell::divide(bool xAxis, bool yAxis, bool zAxis)
{
    if (!(xAxis || yAxis || zAxis)) return false;

    if (_cells.empty())
        _cells.push_back(new Cell(_bb));

    if (xAxis)
    {
        unsigned int numCellsToDivide=_cells.size();
        for(unsigned int i=0;i<numCellsToDivide;++i)
        {
            Cell* orig_cell = _cells[i].get();
            Cell* new_cell = new Cell(orig_cell->_bb);

            float xCenter = (orig_cell->_bb.xMin()+orig_cell->_bb.xMax())*0.5f;
            orig_cell->_bb.xMax() = xCenter;
            new_cell->_bb.xMin() = xCenter;

            _cells.push_back(new_cell);
        }
    }

    if (yAxis)
    {
        unsigned int numCellsToDivide=_cells.size();
        for(unsigned int i=0;i<numCellsToDivide;++i)
        {
            Cell* orig_cell = _cells[i].get();
            Cell* new_cell = new Cell(orig_cell->_bb);

            float yCenter = (orig_cell->_bb.yMin()+orig_cell->_bb.yMax())*0.5f;
            orig_cell->_bb.yMax() = yCenter;
            new_cell->_bb.yMin() = yCenter;

            _cells.push_back(new_cell);
        }
    }

    if (zAxis)
    {
        unsigned int numCellsToDivide=_cells.size();
        for(unsigned int i=0;i<numCellsToDivide;++i)
        {
            Cell* orig_cell = _cells[i].get();
            Cell* new_cell = new Cell(orig_cell->_bb);

            float zCenter = (orig_cell->_bb.zMin()+orig_cell->_bb.zMax())*0.5f;
            orig_cell->_bb.zMax() = zCenter;
            new_cell->_bb.zMin() = zCenter;

            _cells.push_back(new_cell);
        }
    }

    bin();

    return true;

}

void Forest::Cell::bin()
{
    // put trees in appropriate cells.
    TreeList treesNotAssigned;
    for(TreeList::iterator titr=_trees.begin();
        titr!=_trees.end();
        ++titr)
    {
        Tree* tree = titr->get();
        bool assigned = false;
        for(CellList::iterator citr=_cells.begin();
            citr!=_cells.end() && !assigned;
            ++citr)
        {
            if ((*citr)->contains(tree->_position))
            {
                (*citr)->addTree(tree);
                assigned = true;
            }
        }
        if (!assigned) treesNotAssigned.push_back(tree);
    }

    // put the unassigned trees back into the original local tree list.
    _trees.swap(treesNotAssigned);


    // prune empty cells.
    CellList cellsNotEmpty;
    for(CellList::iterator citr=_cells.begin();
        citr!=_cells.end();
        ++citr)
    {
        if (!((*citr)->_trees.empty()))
        {
            cellsNotEmpty.push_back(*citr);
        }
    }
    _cells.swap(cellsNotEmpty);


}
};
