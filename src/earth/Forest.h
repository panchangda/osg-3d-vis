//
// Created by pcd on 2025/2/27.
//

#ifndef FOREST_H
#define FOREST_H
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Camera>


namespace osg_3d_vis {


class Forest {

  public:
    Forest();
    explicit Forest(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera);

    osg::Geode* createTerrain(osg::Vec3d& origin, osg::Vec3& size);

    class Tree : public osg::Referenced
    {
    public:

        Tree():
            _color(255,255,255,255),
            _width(1.0f),
            _height(1.0f),
            _type(0) {}

        Tree(const osg::Vec3& position, const osg::Vec3& normal, const osg::Vec4ub& color, float width, float height, unsigned int type):
            _position(position),
            _normal(_normal),
            _color(color),
            _width(width),
            _height(height),
            _type(type) {}

        osg::Vec3       _position;
        osg::Vec3       _normal;
        osg::Vec4ub     _color;
        float           _width;
        float           _height;
        unsigned int    _type;
    };

    typedef std::vector< osg::ref_ptr<Tree> > TreeList;

    class Cell : public osg::Referenced
    {
    public:
        typedef std::vector< osg::ref_ptr<Cell> > CellList;

        Cell():_parent(0) {}
        Cell(osg::BoundingBox& bb):_parent(0), _bb(bb) {}

        void addCell(Cell* cell) { cell->_parent=this; _cells.push_back(cell); }

        void addTree(Tree* tree) { _trees.push_back(tree); }

        void addTrees(const TreeList& trees) { _trees.insert(_trees.end(),trees.begin(),trees.end()); }

        void computeBound();

        bool contains(const osg::Vec3& position) const { return _bb.contains(position); }

        bool divide(unsigned int maxNumTreesPerCell=10);

        bool divide(bool xAxis, bool yAxis, bool zAxis);

        void bin();


        Cell*               _parent;
        osg::BoundingBox    _bb;
        CellList            _cells;
        TreeList            _trees;

    };



    float random(float min,float max) { return min + (max-min)*(float)rand()/(float)RAND_MAX; }
    int random(int min,int max) { return min + (int)((float)(max-min)*(float)rand()/(float)RAND_MAX); }

    void Forest::createTreeList(osg::Node* terrain,const osg::Vec3& origin, const osg::Vec3& size,
        unsigned int numTreesToCreate, unsigned int numTreesTypes, TreeList& trees,
        bool uniformScale, unsigned int scale);
    osg::Node* Forest::createBillboardGraph(Cell* cell,osg::StateSet* stateset);
    osg::Geometry* Forest::createSprite( float w, float h, osg::Vec4ub color);
};

};

#endif //FOREST_H
