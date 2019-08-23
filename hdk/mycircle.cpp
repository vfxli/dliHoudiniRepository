#include <UT/UT_DSOVersion.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Operator.h>
#include <OP/OP_Context.h>
#include <OP/OP_Network.h>
#include <SOP/SOP_Node.h>
#include "mycircle.h"
#include <stdio.h>

// PRM_Name������OP_OperatorTable�������֣���ǩ��
static PRM_Name		colorName("color", "Color");

static PRM_Default	colorDefaults[] = {
	PRM_Default(1),	//r
	PRM_Default(0),	//g
	PRM_Default(2) //b
};
// ����Ԫ�ؾ����µĲ���
static PRM_Template newParmsTemplates[] = {
	// PRM_Template�����һ��PRM_Name����ĵ�ַ
	PRM_Template(PRM_RGB, 3, &colorName, colorDefaults),
	// �������Ҫ��һ���յ�PRM_Template����Ȼ��H�����
	PRM_Template()
};
// ����������ʵ�ֲ��֡�����һ��new������SOP_MyCircle����
OP_Node *SOP_MyCircle::MyConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
	return new SOP_MyCircle(net, name, op);
}
// ���캯����ʵ�ֲ��֡�Ϊ�ռ��ɡ�����һ��Ҫ�ڳ�ʼ���б������ʼ������
SOP_MyCircle::SOP_MyCircle(OP_Network *net, const char *name, OP_Operator *op):SOP_Circle(net, name, op)
{
}
// OP_TemplatePair������OP/Operator.h
// ʵ���޸Ĳ�������
OP_TemplatePair *SOP_MyCircle::buildTemplatePair() {
	// ���������²��������
	OP_TemplatePair *old, *my;
	// my��ʼ��
	my = new OP_TemplatePair(newParmsTemplates, 0);
	// old��ʼ������my
	old = new OP_TemplatePair(SOP_Circle::myTemplateList, my);
	// ���� old
	return old;
}

// �������ø���ķ���ȥ����
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
// ע���½ڵ�
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