
#include "SOP_Myflatten.h"
#include <UT/UT_DSOVersion.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <UT/UT_Interrupt.h>
#include <iostream>

using namespace HDK_Sample;

void
newSopOperator(OP_OperatorTable *table)
{
	//table frome <OP/OP_OperatorTable.h>
	table->addOperator(new OP_Operator(
		"hdk_myflatten",
		"Myflatten",
		SOP_Myflatten::myConstructor,
		SOP_Myflatten::myTemplateList,
		1,
		1,
		NULL));
}
//��������
OP_Node* SOP_Myflatten::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
	return new SOP_Myflatten(net, name, op);
}
//���캯��
SOP_Myflatten::SOP_Myflatten(OP_Network* net, const char* name, OP_Operator* op)
	: SOP_Node(net, name, op), myGroup(NULL)
{
	//�ֶ���������ID
	//����Ϊtrue������ǱȽϸ߼�������
	//���������Ǽ��һЩ���ܴ��ڵ��ظ�����
	//������ID�õ����ƴ��������¿����������������
	//���統����û�仯��ʱ��
	mySopFlags.setManagesDataIDs(true);
	//ȷ������һ��������
	mySopFlags.setNeedGuide1(true);
}
//��������
SOP_Myflatten::~SOP_Myflatten() {}
//������
//PRM_Name frome <OP/OP_OperatorTable.h> or "SOP_Myflatten.h"
static PRM_Name names[] = {
	PRM_Name("usedir", "Use Direction Vector"),
	PRM_Name("dist", "Distance"),
};
//������
PRM_Template SOP_Myflatten::myTemplateList[] = {
	//�����ͣ������ߴ磬��������Ĭ��ֵ���ַ�����������µ�С��ͷ����ѡ���б���Χ����ִ��sparedata���ѡ�鰴ť��
	//ʹ�õ������صĵ�һ��No extended type given and no export level given.
	PRM_Template(PRM_STRING, 1, &PRMgroupName, 0, &SOP_Node::pointGroupMenu,
				 0, 0, SOP_Node::getGroupSelectButton(GA_GROUP_POINT)),
	//�����ͣ������ߴ磬��������Ĭ��ֵ��ѡ���б�Ϊ�գ���Χ��
	PRM_Template(PRM_FLT_J,	1, &names[1], PRMzeroDefaults, 0, &PRMscaleRange),
	//�����͸�ѡ�򣬲����ߴ磬��������
	PRM_Template(PRM_TOGGLE, 1, &names[0]),
	//��������ƽ�������˵��������ߴ磬��������Ĭ��ֵ��ѡ���б�
	PRM_Template(PRM_ORD, 1, &PRMorientName, 0, &PRMplaneMenu),
	//�����ͷ��򣬳ߴ�3ʸ������������Ĭ��ֵZΪ1��
	PRM_Template(PRM_DIRECTION, 3, &PRMdirectionName, PRMzaxisDefaults),
	//���ն���
	PRM_Template(),
};
//���ò���״̬�Ƿ����
bool SOP_Myflatten::updateParmsFlags()
{
	bool changed;
	//��pi=parm iex������ֵ��
	//�����use direction vector��ѡ������orientation�����ã�
	//�����û��ѡ���ÿ��ã�
	changed  = enableParm(3, !DIRPOP());
	//�����use direction vector��ѡ������orientation���ã�
	//�����û��ѡ���ò����ã�
	changed |= enableParm(4, DIRPOP());
	return changed;
}
OP_ERROR SOP_Myflatten::cookInputGroups(OP_Context &context, int alone)
{
	//������������PRMgroupName����Ǻ�parameterֱ�Ӱ󶨵�
	return cookInputPointGroups(
		context,	//ָ�������ģ����ڰ󶨵�group parameter
		myGroup,	//�ýڵ�Ĳ����顣
					//���������ж�����������ڵ�ֻʹ��һ����������myGroup
		alone,		//�����cookMySop�������ô���������true
					//true��ζ���齫�������뼸��
					//flase��ζ���齫����gdp
		true,		//�����ֶ�ѡ������Ϊ��
		0,			//(default)ָ�����parameter����
		-1,			//(default)ָ���鼶���parameter����
					//-1��Ϊû�����������������������ƽڵ��ʱ���ֻ֧�ֵ���
		true,		//(default)true��ʾʹ���Ѿ����ڵ����������
					//false��ʾ����ָ��һ������
		false,		//(default)false��ʾ�µ���Ӧ���������
					//true��ʾ�µ���Ӧ���������
		true,		// (default) true means that all new groups should be detached, so not owned by the detail;
					//           false means that new point and primitive groups on gdp will be owned by gdp.
					//������һ�����ϵ�ǰ״̬
					//If fetchgdp is false, you must supply a gdp to create groups on in the paramerer pgdp.
		0			//(default)��ʾʹ���ĸ����������ǵ�һ��
	);
}
OP_ERROR SOP_Myflatten::cookMySop(OP_Context &context)
{
	//��������//�����������return��ʱ���Զ�����
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();
	//��ȡʱ��
	fpreal now = context.getTime();
	//��������ļ�����
	duplicateSource(0, context);
	//�󶨱������ȼ�(detail,prim,pt,vtx)
	setVariableOrder(3, 2, 0, 1);
	//�󶨱��ر�������һ������
	setCurGdh(0, myGdpHandle);
	//�������Բ��Ұ󶨵����ر���
	setupLocalVars();
	//ȷ��Ҫ������Щ��
	//UT_ERROR_ABORT������ζֹͣ��ǰ����
	//�����ǰû�д�����������Ҳû�д�����myGroup���ǿյ�
	if (error() < UT_ERROR_ABORT && cookInputGroups(context) < UT_ERROR_ABORT &&
		(!myGroup || !myGroup->isEmpty()))
	{
		//���������������ڵĲ���

		//������ESC,progress����ֹ�������ʱ��������������ֹ����
		UT_AutoInterrupt progress("Flattening Points");
		//��������ָ���������͵����飨ֻ��һ��pos���ԣ�
		UT_Array<GA_RWHandleV3> positionattribs(1);
		UT_Array<GA_RWHandleV3> normalattribs;
		UT_Array<GA_RWHandleV3> vectorattribs;
		//�������ͱ������ڴ�gdp�����ȡ������ʱ�浽����
		GA_Attribute *attrib;
		//��ȡgdp�ϵ����Բ���ͬʱ������������
		//��������ȷ����ѭ����������ÿ��ѭ���õ���ͬ���Ե�ָ��
		GA_FOR_ALL_POINT_ATTRIBUTES(gdp, attrib)
		{
			//std::cout << "for = " << std::endl;
			//����Ǹ�����������������ѭ��
			//���ڼ��ٲ���Ҫ�ļ���
			if (!attrib->needsTransform())
				continue;
			//�����������
			GA_TypeInfo typeinfo = attrib->getTypeInfo();
			if (typeinfo == GA_TYPE_POINT || typeinfo == GA_TYPE_HPOINT)
			{
				//�õ����Բ������
				GA_RWHandleV3 handle(attrib);
				//ȷ����������Ч��
				if (handle.isValid())
				{
					//�����Բ����ֱ���ӵ�������
					positionattribs.append(handle);
					//��Ϊ������mySopFlags.setManagesDataIDs(true)
					//ˢ�¸����Ե�����id
					attrib->bumpDataId();
				}
			}
			else if (typeinfo == GA_TYPE_NORMAL)
			{
				GA_RWHandleV3 handle(attrib);
				if (handle.isValid())
				{
					normalattribs.append(handle);
					attrib->bumpDataId();
				}
			}
			else if (typeinfo == GA_TYPE_VECTOR)
			{
				GA_RWHandleV3 handle(attrib);
				if (handle.isValid())
				{
					vectorattribs.append(handle);
					attrib->bumpDataId();
				}
			}
		}

		GA_Offset start;
		GA_Offset end;
		//1024����Ϊ��λ�ֿ鴦��
		//��ʼֵ����Ҫ�˾��Ǹ�it����
		//it.blockAdvance��һ�ε���startΪ0��֮��ÿ�ε���start+1024
		//����start��end��Ȼ�Ǵ�ֵ��ȥ�ĵ���it�����������Ȩ���޸�������������ֵ��
		//it.blockAdvanceÿ�ε���Ҳ�᷵��һ������ֵ�����Ƿ������һ�ε���
		//�����ֿ鴦��hdk���Զ��������߳�ô��������
		for (GA_Iterator it(gdp->getPointRange(myGroup)); it.blockAdvance(start, end);)
		{
			//std::cout << "*it=" << *it << std::endl;
			//std::cout << "start=" << start << std::endl;
			//std::cout << "end=" << end << std::endl;
			//escȡ������
			if (progress.wasInterrupted())
				break;
			//�Ե����ѭ��
			for (GA_Offset ptoff = start; ptoff < end; ++ptoff)
			{
				//�Զ��󶨾ֲ�����
				myCurPtOff[0] = ptoff;
				//��÷������
				float dist = DIST(now);
				//������orientģʽ�¿��Ʒ���
				UT_Vector3 normal;
				//���û�й�ѡʹ��usedir
				if (!DIRPOP())
				{
					switch (ORIENT())
					{
					case 0:
						//xyƽ��
						normal.assign(0, 0, 1);
						break;
					case 1:
						//yzƽ��
						normal.assign(1, 0, 0);
						break;
					case 2:
						//xzƽ��
						normal.assign(0, 1, 0);
						break;
					}
				}
				//�����ѡ��ʹ��dist
				else
				{
					//�õ�����
					normal.assign(NX(now), NY(now), NZ(now));
					//���//c++�Ŀ⻹�Ǳ�����һЩ���ò����ġ�����normalize
					normal.normalize();
				}
				//�ڶ���ѭ�������Խ���ѭ��
				//posֻ��һ����ֻѭ��һ��
				for (exint i = 0; i < positionattribs.size(); ++i)
				{
					//�õ�pos����
					UT_Vector3 p = positionattribs(i).get(ptoff);
					//����ʹ��orientģʽy��ѹƽ��ô����xzƽ��
					//dot(normal, p)��õ�y���ƫ��ֵ*normal���normalx��z�����
					//���û�й�ѡdist��ôP��ȥ���ֵ�ͻ��y���0
					//dist���Ǹ�ƫ��ֵ
					p -= normal * (dot(normal, p) - dist);
					positionattribs(i).set(ptoff, p);
				}
				//ѭ�����з�����������
				for (exint i = 0; i < normalattribs.size(); ++i)
				{
					//�õ�������������
					UT_Vector3 n = normalattribs(i).get(ptoff);
					//����������������Ϊ�������
					//����Ƿ��������-��normal
					if (dot(normal, n) < 0)
						n = -normal;
					else
						//�������ֱ��ʹ��normal
						n = normal;
					normalattribs(i).set(ptoff, n);
				}
				//ѭ������ʸ����������
				for (exint i = 0; i < vectorattribs.size(); ++i)
				{
					UT_Vector3 v = vectorattribs(i).get(ptoff);
					//��set posһ����ֻ��ȥ����ƫ��ֵ
					v -= normal * dot(normal, v);
					vectorattribs(i).set(ptoff, v);
				}

			}
		}
	}
	//���ñ��ر���
	//�����ĵ���дsop�µ�sop�еľֲ���������û�п�����
	resetLocalVarRefs();
	return error();
}
//��дGUIDE
OP_ERROR SOP_Myflatten::cookMyGuide1(OP_Context &context)
{
	//�ֶ���
	const int divs = 5;
	//�Զ��������Ρ��Զ�����˼�����������return��ʱ����Զ�����
	OP_AutoLockInputs inputs(this);
	//�������ȫ�����γ��ִ����򷵻ش���
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();
	//��ȡʱ��
	float now = context.getTime();
	//�����MyGuide1
	//����ÿ�ε������ظ�����guide
	myGuide1->clearAndDestroy();
	//���dist����
	float dist = DIST(now);
	//Ĭ��Ϊz��
	float nx = 0;
	float ny = 0;
	float nz = 1;
	//���û��ʹ��usedir���ж�orientģʽ�µ�ƽ��
	if (!DIRPOP())
	{
		switch (ORIENT())
		{
		case 0: // XY Plane
			nx = 0; ny = 0; nz = 1;
			break;
		case 1: // YZ Plane
			nx = 1; ny = 0; nz = 0;
			break;
		case 2: // XZ Plane
			nx = 0; ny = 1; nz = 0;
			break;
		}
	}
	//������usedir��ֵ
	else
	{
		nx = NX(now); ny = NY(now); nz = NZ(now);
	}
	//�ж��Ƿ��д���
	if (error() >= UT_ERROR_ABORT)
		return error();
	//��÷����ҹ��
	UT_Vector3 normal(nx, ny, nz);
	normal.normalize();
	//һ��boundbox����
	UT_BoundingBox bbox;
	//�õ�bbox�߿��С
	inputGeo(0, context)->getBBox(&bbox);
	//�����size
	float sx = bbox.sizeX();
	float sy = bbox.sizeY();
	float sz = bbox.sizeZ();
	float size = SYSsqrt(sx*sx + sy*sy + sz*sz);
	//��񻯺��normal��ƫ��ֵ
	float cx = normal.x() * dist;
	float cy = normal.y() * dist;
	float cz = normal.z() * dist;
	//����meshGrid���͵�Guideָ���ֶ�����size
	myGuide1->meshGrid(divs, divs, size, size);
	//���Z������ת��normal�ľ���
	UT_Vector3 zaxis(0, 0, 1);
	UT_Matrix3 mat3;
	mat3.dihedral(zaxis, normal);
	//����λ����Ϣ
	UT_Matrix4 xform;
	xform = mat3;
	xform.translate(cx, cy, cz);
	//����guideλ��
	myGuide1->transform(xform);

	return error();
}
//���������ڶ˿��ϵ���ʾ��Ϣ
const char *
SOP_Myflatten::inputLabel(unsigned) const
{
	return "Geometry to Flatten";
}
