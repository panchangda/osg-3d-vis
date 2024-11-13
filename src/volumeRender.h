#ifndef SCIVIS_SCALAR_VISER_DIRECT_VOLUME_RENDERER_H
#define SCIVIS_SCALAR_VISER_DIRECT_VOLUME_RENDERER_H

#include <string>

#include <array>
#include <map>

#include <osg/CullFace>
#include <osg/CoordinateSystemNode>
#include <osg/ShapeDrawable>
#include <osg/Texture1D>
#include <osg/Texture3D>

namespace SciVis
{
	namespace ScalarViser
	{

		class DirectVolumeRenderer
		{
		public:
			struct ShadingParam {
				bool useShading;
				float ka;
				float kd;
				float ks;
				float shininess;
				osg::Vec3 lightPos;
			};

		private:
			struct PerRendererParam
			{
				osg::ref_ptr<osg::Group> grp;
				osg::ref_ptr<osg::Program> program;

				osg::ref_ptr<osg::Uniform> eyePos;
				osg::ref_ptr<osg::Uniform> sliceCntr;
				osg::ref_ptr<osg::Uniform> sliceDir;
				osg::ref_ptr<osg::Uniform> dt;
				osg::ref_ptr<osg::Uniform> maxStepCnt;
				osg::ref_ptr<osg::Uniform> useSlice;
				osg::ref_ptr<osg::Uniform> useDownSample;

				osg::ref_ptr<osg::Uniform> useShading;
				osg::ref_ptr<osg::Uniform> ka;
				osg::ref_ptr<osg::Uniform> kd;
				osg::ref_ptr<osg::Uniform> ks;
				osg::ref_ptr<osg::Uniform> shininess;
				osg::ref_ptr<osg::Uniform> lightPos;

				class Callback : public osg::NodeCallback
				{
				private:
					osg::Vec3 eyePos;

					osg::ref_ptr<osg::Uniform> eyePosUni;
					osg::ref_ptr<osg::Uniform> useDownSampleUni;

				public:
					Callback(
						osg::ref_ptr<osg::Uniform> eyePosUni,
						osg::ref_ptr<osg::Uniform> useDownSampleUni)
						: eyePosUni(eyePosUni), useDownSampleUni(useDownSampleUni)
					{}
					virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
					{
						auto eyePos = nv->getEyePoint();
						eyePosUni->set(eyePos);

						if (this->eyePos != eyePos)
							useDownSampleUni->set(1);
						else
							useDownSampleUni->set(0);
						this->eyePos = eyePos;

						traverse(node, nv);
					}
				};

				PerRendererParam()
				{
					grp = new osg::Group;

					osg::ref_ptr<osg::Shader> vertShader = osg::Shader::readShaderFile(
						osg::Shader::VERTEX,
						std::string(OSG_3D_VIS_SHADER_PREFIX) + "volumeRenderVS.glsl");
					osg::ref_ptr<osg::Shader> fragShader = osg::Shader::readShaderFile(
						osg::Shader::FRAGMENT,
						std::string(OSG_3D_VIS_SHADER_PREFIX) + "volumeRenderPS.glsl");
					program = new osg::Program;
					program->addShader(vertShader);
					program->addShader(fragShader);

#define STATEMENT(name, val) name = new osg::Uniform(#name, val)
					STATEMENT(eyePos, osg::Vec3());
					STATEMENT(dt, static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) * .008f);
					STATEMENT(maxStepCnt, 100);
					STATEMENT(useSlice, 0);
					STATEMENT(sliceCntr, osg::Vec3());
					STATEMENT(sliceDir, osg::Vec3());
					STATEMENT(useDownSample, 0);

					STATEMENT(useShading, 0);
					STATEMENT(ka, .5f);
					STATEMENT(kd, .5f);
					STATEMENT(ks, .5f);
					STATEMENT(shininess, 16.f);
					STATEMENT(lightPos, osg::Vec3());
#undef STATEMENT

					grp->setCullCallback(new Callback(eyePos, useDownSample));
				}
			};
			PerRendererParam param;

			struct PerVolParam
			{
				bool isDisplayed;

				osg::ref_ptr<osg::Uniform> minLatitute;
				osg::ref_ptr<osg::Uniform> maxLatitute;
				osg::ref_ptr<osg::Uniform> minLongtitute;
				osg::ref_ptr<osg::Uniform> maxLongtitute;
				osg::ref_ptr<osg::Uniform> minHeight;
				osg::ref_ptr<osg::Uniform> maxHeight;
				osg::ref_ptr<osg::Uniform> volStartFromZeroLon;
				osg::ref_ptr<osg::Uniform> rotMat;
				osg::ref_ptr<osg::Uniform> dSamplePos;

				osg::ref_ptr<osg::ShapeDrawable> sphere;
				osg::ref_ptr<osg::Texture3D> volTex;
				osg::ref_ptr<osg::Texture1D> tfTex;

				PerVolParam(
					osg::ref_ptr<osg::Texture3D> volTex,
					osg::ref_ptr<osg::Texture1D> tfTex,
					const std::array<uint32_t, 3>& volDim,
					PerRendererParam* renderer)
					: volTex(volTex), tfTex(tfTex)
				{
					const auto MinHeight = static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) * 1.1f;
					const auto MaxHeight = static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) * 1.3f;

					auto tessl = new osg::TessellationHints;
					tessl->setDetailRatio(10.f);
					sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.f, 0.f, 0.f), MaxHeight), tessl);

					auto states = sphere->getOrCreateStateSet();
#define STATEMENT(name, val)                                                                       \
    name = new osg::Uniform(#name, val);                                                           \
    states->addUniform(name)
					STATEMENT(minLatitute, deg2Rad(-10.f));
					STATEMENT(maxLatitute, deg2Rad(+10.f));
					STATEMENT(minLongtitute, deg2Rad(-20.f));
					STATEMENT(maxLongtitute, deg2Rad(+20.f));
					STATEMENT(minHeight, MinHeight);
					STATEMENT(maxHeight, MaxHeight);
					STATEMENT(volStartFromZeroLon, 0);
					{
						osg::Matrix3 tmpMat;
						tmpMat.makeIdentity();
						STATEMENT(rotMat, tmpMat);
					}
					STATEMENT(dSamplePos,
						osg::Vec3(
							1.f / volDim[0],
							1.f / volDim[1],
							1.f / volDim[2]));
#undef STATEMENT
					states->addUniform(renderer->eyePos);
					states->addUniform(renderer->dt);
					states->addUniform(renderer->maxStepCnt);
					states->addUniform(renderer->useSlice);
					states->addUniform(renderer->sliceCntr);
					states->addUniform(renderer->sliceDir);
					states->addUniform(renderer->useDownSample);

					states->addUniform(renderer->useShading);
					states->addUniform(renderer->ka);
					states->addUniform(renderer->kd);
					states->addUniform(renderer->ks);
					states->addUniform(renderer->shininess);
					states->addUniform(renderer->lightPos);

					states->setTextureAttributeAndModes(0, volTex, osg::StateAttribute::ON);
					states->setTextureAttributeAndModes(1, tfTex, osg::StateAttribute::ON);

					auto volTexUni = new osg::Uniform(osg::Uniform::SAMPLER_3D, "volTex");
					volTexUni->set(0);
					auto tfTexUni = new osg::Uniform(osg::Uniform::SAMPLER_1D, "tfTex");
					tfTexUni->set(1);
					states->addUniform(volTexUni);
					states->addUniform(tfTexUni);

					osg::ref_ptr<osg::CullFace> cf = new osg::CullFace(osg::CullFace::BACK);
					states->setAttributeAndModes(cf);

					states->setAttributeAndModes(renderer->program, osg::StateAttribute::ON);
					states->setMode(GL_BLEND, osg::StateAttribute::ON);
					states->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				/*
				* ����: SetTransferFunction
				* ����: ���ø������ʱ�Ĵ��亯��
				* ����:
				* -- tfTex: ��Ĵ��亯����OSGһά����
				*/
				void SetTransferFunction(osg::ref_ptr<osg::Texture1D> tfTex)
				{
					this->tfTex = tfTex;
					auto states = sphere->getOrCreateStateSet();
					states->setTextureAttributeAndModes(1, this->tfTex, osg::StateAttribute::ON);
				}
				/*
				* ����: SetLongtituteRange
				* ����: ���ø������ʱ�ľ��ȷ�Χ����λΪ�Ƕȣ�
				* ����:
				* -- minLonDeg: ������Сֵ
				* -- maxLonDeg: �������ֵ
				* ����ֵ: ������Ĳ������Ϸ�������false�������óɹ�������true
				*/
				bool SetLongtituteRange(float minLonDeg, float maxLonDeg)
				{
					if (minLonDeg < -180.f) return false;
					if (maxLonDeg > +180.f) return false;
					if (minLonDeg >= maxLonDeg) return false;

					minLongtitute->set(deg2Rad(minLonDeg));
					maxLongtitute->set(deg2Rad(maxLonDeg));

					computeRotMat();
					return true;
				}
				std::array<float, 2> GetLongtituteRange() const
				{
					std::array<float, 2> ret;
					minLongtitute->get(ret[0]);
					maxLongtitute->get(ret[1]);
					return ret;
				}
				/*
				* ����: SetLatituteRange
				* ����: ���ø������ʱ��γ�ȷ�Χ����λΪ�Ƕȣ�
				* ����:
				* -- minLatDeg: γ����Сֵ
				* -- maxLatDeg: γ�����ֵ
				* ����ֵ: ������Ĳ������Ϸ�������false�������óɹ�������true
				*/
				bool SetLatituteRange(float minLatDeg, float maxLatDeg)
				{
					if (minLatDeg < -90.f) return false;
					if (maxLatDeg > +90.f) return false;
					if (minLatDeg >= maxLatDeg) return false;

					minLatitute->set(deg2Rad(minLatDeg));
					maxLatitute->set(deg2Rad(maxLatDeg));

					computeRotMat();
					return true;
				}
				std::array<float, 2> GetLatituteRange() const
				{
					std::array<float, 2> ret;
					minLatitute->get(ret[0]);
					maxLatitute->get(ret[1]);
					return ret;
				}
				/*
				* ����: SetHeightFromCenterRange
				* ����: ���ø������ʱ�ĸ߶ȣ������ģ���Χ
				* ����:
				* -- minH: �߶���Сֵ
				* -- maxH: �߶����ֵ
				* ����ֵ: ������Ĳ������Ϸ�������false�������óɹ�������true
				*/
				bool SetHeightFromCenterRange(float minH, float maxH)
				{
					if (minH < 0.f) return false;
					if (minH >= maxH) return false;

					minHeight->set(minH);
					maxHeight->set(maxH);

					computeRotMat();
					sphere->setShape(new osg::Sphere(osg::Vec3(0.f, 0.f, 0.f), maxH));
					return true;
				}
				std::array<float, 2> GetHeightFromCenterRange() const
				{
					std::array<float, 2> ret;
					minHeight->get(ret[0]);
					maxHeight->get(ret[1]);
					return ret;
				}
				/*
				* ����: SetVolumeStartFromLongtituteZero
				* ����: ��ȫ��������X=0��Ӧ�ľ���Ϊ0�ȣ���Ҫ�����ù���
				* ����:
				* -- flag: Ϊtrueʱ�������ù��ܡ�Ϊfalseʱ���رոù���
				*/
				void SetVolumeStartFromLongtituteZero(bool flag)
				{
					if (flag) volStartFromZeroLon->set(1);
					else volStartFromZeroLon->set(0);
				}

			private:
				float deg2Rad(float deg)
				{
					return deg * osg::PI / 180.f;
				};
				void computeRotMat()
				{
					float minLon, maxLon;
					float minLat, maxLat;
					float minH, maxH;
					minLongtitute->get(minLon);
					maxLongtitute->get(maxLon);
					minLatitute->get(minLat);
					maxLatitute->get(maxLat);
					minHeight->get(minH);
					maxHeight->get(maxH);

					auto lon = .5f * (maxLon + minLon);
					auto lat = .5f * (maxLat + minLat);
					auto h = .5f * (maxH + minH);
					osg::Vec3 dir;
					dir.z() = h * sin(lat);
					h = h * cos(lat);
					dir.y() = h * sin(lon);
					dir.x() = h * cos(lon);
					dir.normalize();

					osg::Matrix3 rotMat;
					rotMat(2, 0) = dir.x();
					rotMat(2, 1) = dir.y();
					rotMat(2, 2) = dir.z();
					auto tmp = osg::Vec3(0.f, 0.f, 1.f);
					tmp = tmp ^ dir;
					rotMat(0, 0) = tmp.x();
					rotMat(0, 1) = tmp.y();
					rotMat(0, 2) = tmp.z();
					tmp = dir ^ tmp;
					rotMat(1, 0) = tmp.x();
					rotMat(1, 1) = tmp.y();
					rotMat(1, 2) = tmp.z();

					this->rotMat->set(rotMat);
				}

				friend class DirectVolumeRenderer;
			};
			std::map<std::string, PerVolParam> vols;

		public:
			DirectVolumeRenderer()
			{}

			/*
			* ����: GetGroup
			* ����: ��ȡ�û��������OSG�ڵ�
			* ����ֵ: OSG�ڵ�
			*/
			osg::Group* GetGroup()
			{
				return param.grp.get();
			}
			/*
			* ����: AddVolume
			* ����: ��û���������һ����
			* ����:
			* -- name: ���������ơ���ͬ��������費ͬ����������
			* -- volTex: ���OSG��ά����
			* -- tfTex: ��Ĵ��亯����OSGһά����
			* -- volDim: �����ά�ߴ�
			* -- isDisplayed: Ϊtrueʱ���屻�����ᱻ���ơ�������ֻ�������������������ᱻ����
			*/
			void AddVolume(
				const std::string& name,
				osg::ref_ptr<osg::Texture3D> volTex,
				osg::ref_ptr<osg::Texture1D> tfTex,
				const std::array<uint32_t, 3>& volDim,
				bool isDisplayed = true)
			{
				auto itr = vols.find(name);
				if (itr != vols.end() && itr->second.isDisplayed) {
					param.grp->removeChild(itr->second.sphere);
					vols.erase(itr);
				}
				auto opt = vols.emplace(
					std::piecewise_construct,
					std::forward_as_tuple(name),
					std::forward_as_tuple(volTex, tfTex, volDim, &param));

				opt.first->second.isDisplayed = isDisplayed;
				if (isDisplayed)
					param.grp->addChild(opt.first->second.sphere);
			}
			/*
			* ����: DisplayVolume
			* ����: ���Ƹû�������е�һ���壬λ������е������彫�������ơ�һ�����ڲ����嶯������������岻�ڸ������ʱ�������嶼���ᱻ���ơ�
			* ����:
			* -- name: �������
			*/
			void DisplayVolume(const std::string& name)
			{
				for (auto itr = vols.begin(); itr != vols.end(); ++itr) {
					if (itr->first == name) {
						itr->second.isDisplayed = true;
						param.grp->addChild(itr->second.sphere);
					}
					else if (itr->second.isDisplayed == true) {
						itr->second.isDisplayed = false;
						param.grp->removeChild(itr->second.sphere);
					}
				}
			}
			/*
			* ����: GetVolumes
			* ����: ��ȡ������У����ڻ���ʱ�������������
			*/
			std::map<std::string, PerVolParam>& GetVolumes()
			{
				return vols;
			}
			/*
			* ����: GetVolume
			* ����: ��ȡ������У����ڻ���ʱ���������
			* ����:
			* -- name: �������
			* ����ֵ: ��Ļ�������
			*/
			PerVolParam* GetVolume(const std::string& name)
			{
				auto itr = vols.find(name);
				if (itr == vols.end())
					return nullptr;
				return &(itr->second);
			}
			/*
			* ����: GetVolumeNum
			* ����: ��ȡ�û���������������
			* ����ֵ: �������
			*/
			size_t GetVolumeNum() const
			{
				return vols.size();
			}
			/*
			* ����: SetDeltaT
			* ����: ���������ʱ�����ߴ����Ĳ���
			* ����:
			* -- dt: ���ߴ����Ĳ���
			*/
			void SetDeltaT(float dt)
			{
				param.dt->set(dt);
			}
			float GetDeltaT() const
			{
				float ret;
				param.dt->get(ret);
				return ret;
			}
			/*
			* ����: SetMaxStepCount
			* ����: ���������ʱ�����ߴ����������
			* ����:
			* -- maxStepCnt: ���ߴ����������
			*/
			void SetMaxStepCount(int maxStepCnt)
			{
				param.maxStepCnt->set(maxStepCnt);
			}
			int GetMaxStepCount() const
			{
				int ret;
				param.maxStepCnt->get(ret);
				return ret;
			}
			/*
			* ����: SetSlicing
			* ����: ����������е�����
			* ����:
			* -- cntr: ��������ĵ㣨��תʱ�Ĳο��㣩��λ����ֲ���������ȡֵ��ΧΪ[0, 1]
			* -- dir: ����ĵ�λ��������λ����ֲ�����
			*/
			void SetSlicing(const osg::Vec3& cntr, const osg::Vec3& dir)
			{
				param.useSlice->set(1);
				param.sliceCntr->set(cntr);
				param.sliceDir->set(dir);
			}
			void DisableSlicing()
			{
				param.useSlice->set(0);
			}
			/*
			* ����: SetShading
			* ����: ����������еĹ�����ɫ����
			* ����:
			* -- param: Blinn-Phong������ɫ����
			*/
			void SetShading(const ShadingParam& param)
			{
				if (!param.useShading)
					this->param.useShading->set(0);
				else {
					this->param.useShading->set(1);
					this->param.ka->set(param.ka);
					this->param.kd->set(param.kd);
					this->param.ks->set(param.ks);
					this->param.shininess->set(param.shininess);
					this->param.lightPos->set(param.lightPos);
				}
			}
		};

	} // namespace ScalarViser
} // namespace SciVis

#endif // !SCIVIS_SCALAR_VISER_DIRECT_VOLUME_RENDERER_H