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

//操作符注册
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
	//清除几何体
	gdp->clearAndDestroy();
	//表明SOP依赖时间，既时间改变后需要重新cook
	//flags().timeDep = 1;
	flags().setTimeDep(1);
	//dop节点名
	UT_String dopnetstr;
	//物体过滤名
	UT_String objmask;
	//解算时间
	SIM_Time simtime;
	//dop物体列表
	SIM_ConstObjectArray simobjects;
	//get到dop节点名
	PARM_DOPNET(dopnetstr);
	//get到物体过滤名
	PARM_OBJMAK(objmask);
	//入如果dop节点名不是字符串返回错误
	if (!objmask.isstring())
		return error();
	//找到节点拿到指针
	OP_Node *op = findNode(dopnetstr);
	//如果没找到dop节点返回错误
	if (!op)
	{
		addError(SOP_ERR_BADNODE, "Dop network node should be selected");
		return error();
	}
	//精确到节点类型为dop拿到指针
	OBJ_DopNet *dopnet = op->castToOBJNode()->castToOBJDopNet();
	//失败了就返回错误/比如你选的不是一个dopnetwork节点
	if (!dopnet)
	{
		addError(SOP_ERR_BADNODE, "Dop network node should be selected");
		return error();
	}
	//拿到engine。用这个可以get到dop物体
	const DOP_Engine &engine = dopnet->getEngine();
	//记录解算时间
	engine.getGlobalTime(simtime);
	//拿到dop所有物体
	engine.findAllObjectsFromString(objmask.c_str(), simobjects, simtime, false);
	//如果啥都没有就返回个错
	if (simobjects.entries() == 0)
		return error();
	//拿到sop级别gdp的p
	GA_RWHandleV3 ph = gdp->getP();
	//添加法线属性
	GA_RWHandleV3 nh = gdp->addNormalAttribute(GA_ATTRIB_POINT);
	//添加time属性
	GA_RWHandleF th = gdp->addFloatTuple(GA_ATTRIB_POINT, "time", 1);
	//外层循环循环dop物体列表的每个物体
	for (int i = 0; i < simobjects.entries(); i++){
		//拿到对应循环次数的物体
		const SIM_Object *simobj = simobjects(i);
		//拿到这个物体的impact数据
		const SIM_Data *sdata = simobj->getConstNamedSubData(SIM_IMPACTS_DATANAME);
		//如果数据不是空的
		if (sdata)
		{
			//这里是拿到sim_data指针所指向的impact数据的地址并且用SIM_Impacts类型的指针记录
			//相当于把一个大的类型范围缩小到一个小的类型范围
			//主要是为了使用sim_impacts里面的一些方法，会比较方便
			const SIM_Impacts *impactData = (SIM_Impacts *)sdata;
			//按照impact的点数量循环
			for (int idx = 0; idx < impactData->getNumImpacts(); idx++)
			{
				//拿到点位置
				const UT_Vector3 impactPos = impactData->getPosition(idx);
				//拿到法线属性
				const UT_Vector3 impactN = impactData->getNormal(idx);
				//拿到time属性
				SIM_Time impactTime = impactData->getTime(idx);
				//SOP创建点
				GA_Offset ptoff = gdp->appendPointOffset();
				//设置属性
				ph.set(ptoff, impactPos);
				nh.set(ptoff, impactN);
				th.set(ptoff, impactTime);
			}
		}
	}
	return error();
}