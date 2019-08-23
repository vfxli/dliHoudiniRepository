#include"sop_mycone.h"  
#include<OP/OP_Operator.h>  
#include<OP/OP_OperatorTable.h>  
#include<PRM/PRM_Include.h>//PRM相关的头文件貌似都包含在这里了  
#include<UT/UT_DSOVersion.h>//有这个houdini里面才能找到你编译好的东西  
#include<SYS/SYS_Math.h>  
#include<SYS/SYS_NTMath.h>//这里主要是是取了个圆周率M_PI  
#include<GEO/GEO_Primpoly.h>//创建poly多边形  
#include<GU/GU_Detail.h>//对象指针gdp在这里  
  
//newSopOperator是Houdini从这个DLL抓取并调用注册SOP的钩子。在这种情况下，我们将自己添加到指定的OperatorTable中  
void newSopOperator(OP_OperatorTable *table)  
{  
    table->addOperator(new OP_Operator("mycone", "Cone",  
        SOP_MyCone::myConstructor,//创建新的对象  
        SOP_MyCone::myTemplateList,//参数模板列表  
        0, 0,//最小最大输入端口数  
        nullptr,//本地变量  
        OP_FLAG_GENERATOR//标识为常规  
    ));  
}  
//创建锥形对象  
OP_Node *SOP_MyCone::myConstructor(OP_Network *net, const char *name, OP_Operator *op)  
{  
    return new SOP_MyCone(net, name, op);  
}  
//参数模板  
PRM_Template SOP_MyCone::myTemplateList[] = {  
    PRM_Template(PRM_XYZ, PRM_Template::PRM_EXPORT_TBX, 3, &PRMcenterName),  
    PRM_Template(PRM_FLT, 1, &PRMheightName, PRMoneDefaults),  
    PRM_Template(PRM_FLT, 1, &PRMradiusName, PRMoneDefaults),  
    PRM_Template(PRM_FLT, 1, &PRMscaleName, PRMoneDefaults),  
    PRM_Template(PRM_INT, 1, &PRMdivName, PRMthreeDefaults, 0, &PRMdivision2Range),  
    PRM_Template()  
};  
//构造  
SOP_MyCone::SOP_MyCone(OP_Network *net, const char *name, OP_Operator *op) :SOP_Node(net, name, op)  
{  
    mySopFlags.setManagesDataIDs(true);  
}  
//析构,啥都不干  
SOP_MyCone::~SOP_MyCone() {};  
  
//如何创建一个3边形polygon  
void addPolygon(GU_Detail* gdp, exint v1, exint v2, exint v3)  
{  
    //追加一个新的多边形primitive,原始类型在GEO_PrimType.h中定义  
    GEO_PrimPoly *prim_poly_ptr = (GEO_PrimPoly *)gdp->appendPrimitive(GA_PRIMPOLY);  
    //追加形成这个primitive的vertices,点偏移  
    prim_poly_ptr->appendVertex(v1);  
    prim_poly_ptr->appendVertex(v2);  
    prim_poly_ptr->appendVertex(v3);  
    prim_poly_ptr->close();//最后封闭primitive使其成为多边形而不是多段线。  
}  
//锥形的底部需要封闭起来,函数重载(跟上面的创建3变形函数名相同,但是参数不同)  
void addPolygon(GU_Detail* gdp, exint vstart, exint nv)  
{  
    GEO_PrimPoly *prim_poly_ptr = (GEO_PrimPoly *)gdp->appendPrimitive(GA_PRIMPOLY);  
    //追加形成这个primitive的vertices,我们利用索引图的单调性保证来避免保留单独的添加点偏移数组  
    for (vstart; vstart <= nv; ++vstart)  
    {  
        prim_poly_ptr->appendVertex(vstart);  
    }  
    prim_poly_ptr->close();//封闭primitive使其成为多边形而不是多段线。  
}  
  
//sop 锥形形状核心计算  
OP_ERROR SOP_MyCone::cookMySop(OP_Context &context)  
{  
    fpreal now = context.getTime();  
    UT_Vector3 center(CENTERX(now), CENTERY(now), CENTERZ(now));  
    fpreal height = HEIGHT(now);  
    fpreal rad = RADIUS(now);  
    fpreal scale = SCALE(now);  
    int divsion = DIVSION(now);  
    //这里例子是生产一个锥形的节点,没有任何输入端,所以需要销毁所有可能的数据  
    gdp->clearAndDestroy();  
  
    //形成锥形的点位置  
    for (exint i = 0; i < divsion + 1; i++)  
    {  
        GA_Offset ptoff = gdp->appendPoint();  
        UT_Vector3 pos;  
        if (i == 0)//锥形的顶部点,我把它作为起始点  
        {  
            pos(0) = 0; pos(1) = height * scale; pos(2) = 0;  
        }  
        else {  
            pos(0) = SYScos((float)(i - 1) * 2 * M_PI / (float)divsion) * rad * scale;  
            pos(1) = 0;  
            pos(2) = SYSsin((float)(i - 1) * 2 * M_PI / (float)divsion) * rad * scale;  
        }  
        pos += center;  
        gdp->setPos3(ptoff, pos);  
    }  
    //形成一个多边形至少需要3个点,所以i初始为2  
    for (exint i = 2; i < divsion + 1; i++)  
    {  
    	addPolygon(gdp, 0, i, i-1);  
        if(i>2)  
            addPolygon(gdp, 0, 1, divsion);  
        if(i>2 && i==divsion)  
            addPolygon(gdp, 1, divsion);  
    }  
    return error();  
}  