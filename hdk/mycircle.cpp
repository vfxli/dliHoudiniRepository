#include <UT/UT_DSOVersion.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Operator.h>
#include <OP/OP_Context.h>
#include <OP/OP_Network.h>
#include <SOP/SOP_Node.h>
#include "mycircle.h"
#include <stdio.h>

// PRM_Name来自于OP_OperatorTable，（名字，标签）
static PRM_Name		colorName("color", "Color");

static PRM_Default	colorDefaults[] = {
	PRM_Default(1),	//r
	PRM_Default(0),	//g
	PRM_Default(2) //b
};
// 数组元素就是新的参数
static PRM_Template newParmsTemplates[] = {
	// PRM_Template必须给一个PRM_Name对象的地址
	PRM_Template(PRM_RGB, 3, &colorName, colorDefaults),
	// 数组最后要放一个空的PRM_Template对象不然会H会崩溃
	PRM_Template()
};
// 构造器函数实现部分。返回一个new出来的SOP_MyCircle对象
OP_Node *SOP_MyCircle::MyConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
	return new SOP_MyCircle(net, name, op);
}
// 构造函数的实现部分。为空即可。但是一定要在初始化列表里面初始化父类
SOP_MyCircle::SOP_MyCircle(OP_Network *net, const char *name, OP_Operator *op):SOP_Circle(net, name, op)
{
}
// OP_TemplatePair来自于OP/Operator.h
// 实现修改参数部分
OP_TemplatePair *SOP_MyCircle::buildTemplatePair() {
	// 定义两个新参数表对象
	OP_TemplatePair *old, *my;
	// my初始化
	my = new OP_TemplatePair(newParmsTemplates, 0);
	// old初始化附加my
	old = new OP_TemplatePair(SOP_Circle::myTemplateList, my);
	// 返回 old
	return old;
}

// 错误处理，用父类的方法去处理
OP_ERROR 
SOP_MyCircle::cookMySop(OP_Context &context) {
	gdp->clearAndDestroy();  
	//SOP_Circle::cookMySop(context);
	fpreal now = context.getTime();
	fpreal clr_r = COLORX(now);
	fpreal clr_g = COLORY(now);
	fpreal clr_b = COLORZ(now);
	GA_Offset ptoff = 0;

	GA_RWHandleV3 handle(gdp->addFloatTuple(GA_ATTRIB_POINT,"Cd",3));
	handle.set(ptoff, UT_Vector3(clr_r, clr_g, clr_b));
	for (int i = 0; i < 100; i++)
	{
		GA_Offset ptoff = SOP_Circle::gdp->appendPoint();
		UT_Vector3 pos(0,2,0);
		SOP_Circle::gdp->setPos3(ptoff, pos);  
	}

	return error(); 
}
// 注册新节点
void newSopOperator(OP_OperatorTable *table)
{
	table->addOperator(
		new OP_Operator("myCircle",
		"myCircle",
		SOP_MyCircle::myConstructor,
		SOP_MyCircle::buildTemplatePair(),
		0,
		0,
		OP_FLAG_GENERATOR));
}