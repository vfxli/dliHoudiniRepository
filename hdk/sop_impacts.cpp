#include <UT/UT_DSOVersion.h> 
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Node.h>
#include <DOP/DOP_Node.h>
#include <OBJ/OBJ_Node.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Impacts.h>
#include <SIM/SIM_ObjectArray.h>
#include <OBJ/OBJ_DopNet.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_SpareData.h>
#include "sop_impacts.h"

#include <iostream>

//������ע��
void
newSopOperator(OP_OperatorTable *table)
{
	table->addOperator(
		new OP_Operator("rbdpoints",
		"RBDPoints",
		SOP_ImportImpacts::MyConstructor,
		SOP_ImportImpacts::myParmTemplates,
		0,
		0,
		nullptr,
		OP_FLAG_GENERATOR));
}
static PRM_Name doppath("dopnet", "Dopnet");
static PRM_Name objmask("objmask", "Objmask");
PRM_Template
SOP_ImportImpacts::myParmTemplates[] = {
	PRM_Template(PRM_STRING_OPREF, PRM_TYPE_DYNAMIC_PATH, 1, &doppath, 0,0,0,0, &PRM_SpareData::dopPath),
	PRM_Template(PRM_STRING_E, 1, &objmask),
	PRM_Template(),
};
SOP_ImportImpacts::SOP_ImportImpacts(OP_Network *net, const char *name, OP_Operator *op)
:SOP_Node(net, name, op)
{
}
SOP_ImportImpacts::~SOP_ImportImpacts() {};
OP_Node *SOP_ImportImpacts::MyConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
	return new SOP_ImportImpacts(net, name, op);
}

OP_ERROR
SOP_ImportImpacts::cookMySop(OP_Context &context)
{
	//���������
	gdp->clearAndDestroy();
	//����SOP����ʱ�䣬��ʱ��ı����Ҫ����cook
	//flags().timeDep = 1;
	flags().setTimeDep(1);
	//dop�ڵ���
	UT_String dopnetstr;
	//���������
	UT_String objmask;
	//����ʱ��
	SIM_Time simtime;
	//dop�����б�
	SIM_ConstObjectArray simobjects;
	//get��dop�ڵ���
	PARM_DOPNET(dopnetstr);
	//get�����������
	PARM_OBJMAK(objmask);
	//�����dop�ڵ��������ַ������ش���
	if (!objmask.isstring())
		return error();
	//�ҵ��ڵ��õ�ָ��
	OP_Node *op = findNode(dopnetstr);
	//���û�ҵ�dop�ڵ㷵�ش���
	if (!op)
	{
		addError(SOP_ERR_BADNODE, "Dop network node should be selected");
		return error();
	}
	//��ȷ���ڵ�����Ϊdop�õ�ָ��
	OBJ_DopNet *dopnet = op->castToOBJNode()->castToOBJDopNet();
	//ʧ���˾ͷ��ش���/������ѡ�Ĳ���һ��dopnetwork�ڵ�
	if (!dopnet)
	{
		addError(SOP_ERR_BADNODE, "Dop network node should be selected");
		return error();
	}
	//�õ�engine�����������get��dop����
	const DOP_Engine &engine = dopnet->getEngine();
	//��¼����ʱ��
	engine.getGlobalTime(simtime);
	//�õ�dop��������
	engine.findAllObjectsFromString(objmask.c_str(), simobjects, simtime, false);
	//���ɶ��û�оͷ��ظ���
	if (simobjects.entries() == 0)
		return error();
	//�õ�sop����gdp��p
	GA_RWHandleV3 ph = gdp->getP();
	//��ӷ�������
	GA_RWHandleV3 nh = gdp->addNormalAttribute(GA_ATTRIB_POINT);
	//���time����
	GA_RWHandleF th = gdp->addFloatTuple(GA_ATTRIB_POINT, "time", 1);
	//���ѭ��ѭ��dop�����б��ÿ������
	for (int i = 0; i < simobjects.entries(); i++){
		//�õ���Ӧѭ������������
		const SIM_Object *simobj = simobjects(i);
		//�õ���������impact����
		const SIM_Data *sdata = simobj->getConstNamedSubData(SIM_IMPACTS_DATANAME);
		//������ݲ��ǿյ�
		if (sdata)
		{
			//�������õ�sim_dataָ����ָ���impact���ݵĵ�ַ������SIM_Impacts���͵�ָ���¼
			//�൱�ڰ�һ��������ͷ�Χ��С��һ��С�����ͷ�Χ
			//��Ҫ��Ϊ��ʹ��sim_impacts�����һЩ��������ȽϷ���
			const SIM_Impacts *impactData = (SIM_Impacts *)sdata;
			//����impact�ĵ�����ѭ��
			for (int idx = 0; idx < impactData->getNumImpacts(); idx++)
			{
				//�õ���λ��
				const UT_Vector3 impactPos = impactData->getPosition(idx);
				//�õ���������
				const UT_Vector3 impactN = impactData->getNormal(idx);
				//�õ�time����
				SIM_Time impactTime = impactData->getTime(idx);
				//SOP������
				GA_Offset ptoff = gdp->appendPointOffset();
				//��������
				ph.set(ptoff, impactPos);
				nh.set(ptoff, impactN);
				th.set(ptoff, impactTime);
			}
		}
	}
	return error();
}