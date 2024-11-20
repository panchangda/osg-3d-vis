#include "Cloud.h"

#include "../util.h"


Cloud::Cloud(osg::ref_ptr<osg::Group> _root)
{
	auto quad = createFullScreenQuad();

	auto Geode = new osg::Geode;
	Geode->addDrawable(quad);

	auto state = Geode->getOrCreateStateSet();

    osg::ref_ptr<osg::Program> shaderProgram = new osg::Program;

    osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	auto s = std::string(OSG_3D_VIS_SHADER_PREFIX);
    vertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "cloud/cloudVS.glsl");
    fragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "cloud/cloudPS.glsl");

    shaderProgram->addShader(vertexShader.get());
    shaderProgram->addShader(fragmentShader.get());
    osg::ref_ptr<osg::Uniform> time = new osg::Uniform("u_time", 0.0f);
    time->setUpdateCallback(new osg_3d_vis::TimeUniformCallback());

    state->setAttribute(shaderProgram);
    osg::ref_ptr<osg::Depth> depth = new osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS); // ʼ��ͨ����Ȳ���
    depth->setWriteMask(false);             // ��ֹ���д�룬�Է�ֹ������������������

    // �����������ӵ�������� StateSet ��
    state->setAttributeAndModes(depth, osg::StateAttribute::ON);
    state->setRenderBinDetails(-1, "RenderBin");
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    state->addUniform(time);
    _root->addChild(Geode);
}
