#include "SOP_Mypoint.h"

#include <UT/UT_DSOVersion.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <GEO/GEO_PrimPoly.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Include.h>

//ע���½ڵ�
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "mypoint","Mypoint",
        SOP_Mypoint::myConstructor,
        SOP_Mypoint::myTemplateList,
        0,
        0,
        nullptr,
        OP_FLAG_GENERATOR));
}
//���캯���ǵó�ʼ������
SOP_Mypoint::SOP_Mypoint(OP_Network *net, const char *name, OP_Operator *op): SOP_Node(net, name, op) {}
SOP_Mypoint::~SOP_Mypoint() {}
//�����������ظ������
OP_Node* SOP_Mypoint::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Mypoint(net, name, op);
}
//����
static PRM_Name create_npoints("npoints", "Number Points");
static PRM_Name step("step", "Step");
static PRM_Name     colorName("color", "Color");
//����Ĭ��ֵ
static PRM_Default default_create_npoints(10);
static PRM_Default default_step(0.1);
static PRM_Default  colorDefaults[] = {
    PRM_Default(1), //r
    PRM_Default(0), //g
    PRM_Default(0) //b
};
//������Χ
static PRM_Range range_create_npoints(PRM_RANGE_RESTRICTED,1,PRM_RANGE_RESTRICTED,300000000);
static PRM_Range range_step(PRM_RANGE_RESTRICTED,0.001,PRM_RANGE_RESTRICTED,1);
//�Ѳ�������������
PRM_Template SOP_Mypoint::myTemplateList[] = {
    PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_TBX, 1, &create_npoints, &default_create_npoints, 0, &range_create_npoints),
    PRM_Template(PRM_FLT,1, &step, &default_step, 0, &range_step),
    PRM_Template(PRM_RGB,   3, &colorName, colorDefaults),
    PRM_Template()};
//cookMySop
OP_ERROR SOP_Mypoint::cookMySop(OP_Context &context) {
    //��ȡʱ��
    //��Ϊget������ʱ����Ҫ����һ��ʱ�����
    fpreal now = context.getTime();
    gdp->clearAndDestroy();
    //��ȡ����
    int _create_npoints = CREATE_NPOINTS(now);
    float _step = STEP(now);
    fpreal clr_r = COLORX(now);
    fpreal clr_g = COLORY(now);
    fpreal clr_b = COLORZ(now);
    //������ɫ���Բ��õ��ֱ�
    GA_RWHandleV3 h(gdp->addFloatTuple(GA_ATTRIB_POINT,"Cd",3));
    //����prim
    //GEO_PrimPoly *prim_poly_ptr = (GEO_PrimPoly *)gdp->appendPrimitive(GA_PRIMPOLY);  
    //ѭ��������
    //ѭ���������ǵ����������ڲ������
    for (int i = 0; i <_create_npoints; i++)
    {
        //������
        GA_Offset ptoff = gdp->appendPointOffset();
        //���õ�λ��
        gdp->setPos3(ptoff, UT_Vector3(0,(float)i*_step,0));
        //������
        //prim_poly_ptr->appendVertex(ptoff);
        //������ɫ
        h.set(ptoff, UT_Vector3(clr_r, clr_g, clr_b));
    }
    //prim_poly_ptr->open();

    //������뷵��һ��error
    return error(); 
}