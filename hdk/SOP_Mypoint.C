#include "SOP_Mypoint.h"

#include <UT/UT_DSOVersion.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <GEO/GEO_PrimPoly.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Include.h>

//注册新节点
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
//构造函数记得初始化父类
SOP_Mypoint::SOP_Mypoint(OP_Network *net, const char *name, OP_Operator *op): SOP_Node(net, name, op) {}
SOP_Mypoint::~SOP_Mypoint() {}
//构建函数返回该类对象
OP_Node* SOP_Mypoint::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Mypoint(net, name, op);
}
//参数
static PRM_Name create_npoints("npoints", "Number Points");
static PRM_Name step("step", "Step");
static PRM_Name     colorName("color", "Color");
//参数默认值
static PRM_Default default_create_npoints(10);
static PRM_Default default_step(0.1);
static PRM_Default  colorDefaults[] = {
    PRM_Default(1), //r
    PRM_Default(0), //g
    PRM_Default(0) //b
};
//参数范围
static PRM_Range range_create_npoints(PRM_RANGE_RESTRICTED,1,PRM_RANGE_RESTRICTED,300000000);
static PRM_Range range_step(PRM_RANGE_RESTRICTED,0.001,PRM_RANGE_RESTRICTED,1);
//把参数给到参数表
PRM_Template SOP_Mypoint::myTemplateList[] = {
    PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_TBX, 1, &create_npoints, &default_create_npoints, 0, &range_create_npoints),
    PRM_Template(PRM_FLT,1, &step, &default_step, 0, &range_step),
    PRM_Template(PRM_RGB,   3, &colorName, colorDefaults),
    PRM_Template()};
//cookMySop
OP_ERROR SOP_Mypoint::cookMySop(OP_Context &context) {
    //获取时间
    //因为get参数的时候需要传入一个时间对象
    fpreal now = context.getTime();
    gdp->clearAndDestroy();
    //获取参数
    int _create_npoints = CREATE_NPOINTS(now);
    float _step = STEP(now);
    fpreal clr_r = COLORX(now);
    fpreal clr_g = COLORY(now);
    fpreal clr_b = COLORZ(now);
    //添加颜色属性并拿到手柄
    GA_RWHandleV3 h(gdp->addFloatTuple(GA_ATTRIB_POINT,"Cd",3));
    //创建prim
    //GEO_PrimPoly *prim_poly_ptr = (GEO_PrimPoly *)gdp->appendPrimitive(GA_PRIMPOLY);  
    //循环创建点
    //循环次数就是点数量来自于参数面板
    for (int i = 0; i <_create_npoints; i++)
    {
        //创建点
        GA_Offset ptoff = gdp->appendPointOffset();
        //设置点位置
        gdp->setPos3(ptoff, UT_Vector3(0,(float)i*_step,0));
        //连成线
        //prim_poly_ptr->appendVertex(ptoff);
        //设置颜色
        h.set(ptoff, UT_Vector3(clr_r, clr_g, clr_b));
    }
    //prim_poly_ptr->open();

    //该类必须返回一个error
    return error(); 
}