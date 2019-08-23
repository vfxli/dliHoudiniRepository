#include "SOP_Spiro.h"

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
        "spiro","Spiro",
        SOP_Spiro::myConstructor,
        SOP_Spiro::myTemplateList,
        0,
        0,
        nullptr,
        OP_FLAG_GENERATOR));
}
//构造函数记得初始化父类
SOP_Spiro::SOP_Spiro(OP_Network *net, const char *name, OP_Operator *op): SOP_Node(net, name, op) {}
SOP_Spiro::~SOP_Spiro() {}
//构建函数返回该类对象
OP_Node* SOP_Spiro::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Spiro(net, name, op);
}
//参数
static PRM_Name create_npoints("npoints", "Number Points");
static PRM_Name     colorName("color", "Color");
static PRM_Name     frName("fixed_radius", "Fixed Radius");
static PRM_Name     mrName("movable_radius", "Movable Radius");
static PRM_Name     spName("step", "Step");
static PRM_Name     scName("spiro_scale", "Spiro Scale");
static PRM_Name     poName("pen_offset", "Pen Offset");
//参数默认值
static PRM_Default default_create_npoints(60000);
static PRM_Default  colorDefaults[] = {
    PRM_Default(1), //r
    PRM_Default(0), //g
    PRM_Default(0) //b
};
static PRM_Default default_frName(1.7);
static PRM_Default default_mrName(0.9);
static PRM_Default default_spName(0.001);
static PRM_Default default_scName(1);
static PRM_Default default_poName(2.8);
//参数范围
static PRM_Range range_create_npoints(PRM_RANGE_RESTRICTED,1,PRM_RANGE_RESTRICTED,300000000);
static PRM_Range range_frName(PRM_RANGE_RESTRICTED,0.01,PRM_RANGE_RESTRICTED,3);
static PRM_Range range_mrName(PRM_RANGE_RESTRICTED,0.01,PRM_RANGE_RESTRICTED,3);
static PRM_Range range_spName(PRM_RANGE_RESTRICTED,0.001,PRM_RANGE_RESTRICTED,0.01);
static PRM_Range range_scName(PRM_RANGE_RESTRICTED,0.1,PRM_RANGE_RESTRICTED,1);
static PRM_Range range_poName(PRM_RANGE_RESTRICTED,-3,PRM_RANGE_RESTRICTED,3);
//把参数给到参数表
PRM_Template SOP_Spiro::myTemplateList[] = {
    PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_TBX, 1, &create_npoints, &default_create_npoints, 0, &range_create_npoints),
    PRM_Template(PRM_RGB,   3, &colorName, colorDefaults),
    PRM_Template(PRM_FLT,   1, &frName, &default_frName, 0,  &range_frName),
    PRM_Template(PRM_FLT,   1, &mrName, &default_mrName, 0,  &range_mrName),
    PRM_Template(PRM_FLT,   1, &spName, &default_spName, 0,  &range_spName),
    PRM_Template(PRM_FLT,   1, &scName, &default_scName, 0,  &range_scName),
    PRM_Template(PRM_FLT,   1, &poName, &default_poName, 0,  &range_poName),
    PRM_Template()};
//cookMySop
OP_ERROR SOP_Spiro::cookMySop(OP_Context &context) {
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
    fpreal fr = FR(now);
    fpreal mr = MR(now);
    fpreal sp = SP(now);
    fpreal sc = SC(now);
    fpreal po = PO(now);
    //添加颜色属性并拿到手柄
    GA_RWHandleV3 h(gdp->addFloatTuple(GA_ATTRIB_POINT,"Cd",3));
    //创建prim
    GEO_PrimPoly *prim_poly_ptr = (GEO_PrimPoly *)gdp->appendPrimitive(GA_PRIMPOLY);  
    //循环创建点
    //循环次数就是点数量来自于参数面板
    for (int i = 0; i <_create_npoints; i++)
    {
        //创建点
        GA_Offset ptoff = gdp->appendPointOffset();
        //计算点位置
        UT_Vector3 pos;
        float _sp = (float)i * sp;
        pos(0) = ((fr+mr)*cos(_sp) - (mr*po)*cos(((fr+mr)/mr)*_sp))*sc;
        pos(1) = ((fr+mr)*sin(_sp) - (mr*po)*sin(((fr+mr)/mr)*_sp))*sc;
        pos(2) = 0;
        //设置点位置
        gdp->setPos3(ptoff, pos);
        //连成线
        prim_poly_ptr->appendVertex(ptoff);
        //设置颜色
        h.set(ptoff, UT_Vector3(clr_r, clr_g, clr_b));
    }
    prim_poly_ptr->open();

    //该类必须返回一个error
    return error(); 
}