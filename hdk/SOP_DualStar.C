/*
 * Copyright (c) 2018
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * The Dual Star SOP
 */

#include "SOP_DualStar.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_Include.h>
#include <CH/CH_LocalVariable.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

using namespace HDK_Sample;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//


///
/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case we add ourselves
/// to the specified operator table.
///
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "hdk_dualstar",                 // Internal name
        "Dual Star",                     // UI name
        SOP_DualStar::myConstructor,    // How to build the SOP
        SOP_DualStar::myTemplateList,   // My parameters
        1,                          // Min # of sources
        1,                          // Max # of sources
		//这里没有，如果有应该是给一个成员数据例如SOP_Star::myVariables
        0,      // Local variables
		//这里表示该节点需要生成一个新的几何体
        OP_FLAG_GENERATOR,        // Flag it as generator
	//**inputlabels
	//Note: The 'inputlabels' array is expected to be null-terminated
	0,
	//输出端数量
	2));	// Outputs.
}
//参数 复选框 是否使用负值半径
static PRM_Name		negativeName("nradius", "Negative Radius");
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version
//默认值 细分数
static PRM_Default	fiveDefault(5);		// Default to 5 divisions
//默认值 内外半径
static PRM_Default	radiiDefaults[] = {
			   PRM_Default(1),		// Outside radius
			   PRM_Default(0.3)		// Inside radius
			};
//参数表
PRM_Template
SOP_DualStar::myTemplateList[] = {
	//细分数
    PRM_Template(PRM_INT,			// Integer parameter.//参数类型
		//No extended type given but export level given
		//这个导出类型的具体用途还不知道
		 PRM_Template::PRM_EXPORT_TBX,	// Export to top of viewer
						// when user selects this node
		 1, 		// One integer in this row/parameter//参数元素数量/整形即1，矢量即3
		 &PRMdivName,   // Name of this parameter - must be static//默认值，需要传入地址//这里使用模板类
		 &fiveDefault,  // Default for this parameter - ditto//这里使用上面定义的默认值
		 0,		// Menu for this parameter//菜单样式//这里为空
		 &PRMdivision2Range // Valid range//分段数范围//这里使用模板//该模板范围是2-50
		 ),
	// 半径
    PRM_Template(PRM_XYZ,	2, &PRMradiusName, radiiDefaults),
	//是否使用负值半径
    PRM_Template(PRM_TOGGLE,	1, &negativeName),
	//中心点
    PRM_Template(PRM_XYZ,       3, &PRMcenterName),
	//方向
    PRM_Template(PRM_ORD,	1, &PRMorientName, 0, &PRMplaneMenu),
    PRM_Template()
};


OP_Node *
SOP_DualStar::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_DualStar(net, name, op);
}

SOP_DualStar::SOP_DualStar(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
{
    // We do not manage our ids, so do not set the flag.
}

SOP_DualStar::~SOP_DualStar() {}

OP_ERROR
SOP_DualStar::cookMySop(OP_Context &context)
{
    // We must lock our inputs before we try to access their geometry.
    // OP_AutoLockInputs will automatically unlock our inputs when we return.
    // NOTE: Don't call unlockInputs yourself when using this!
    OP_AutoLockInputs inputs(this);
    if (inputs.lock(context) >= UT_ERROR_ABORT)
        return error();
	//复制输入数据，需要指定输入端口
    duplicateSource(0, context);

    // Start the interrupt server
	//这里调用原有的gdp构建几何体是在输入几何体的基础上构建的
	//就是说直接显示节点会同时显示两个几何体
    buildStar(gdp, context);

    return error();
}

void
SOP_DualStar::buildStar(GU_Detail *dst, OP_Context &context)
{
	//get参数
    fpreal now = context.getTime();
    int divisions  = DIVISIONS(now)*2;  // We need twice our divisions of points

    int plane     = ORIENT();
    int negradius = NEGRADIUS();
    float tx      = CENTERX(now);
    float ty      = CENTERY(now);
    float tz      = CENTERZ(now);

    int xcoord, ycoord, zcoord;
    switch (plane)
    {
        case 0:         // XY Plane
            xcoord = 0;
            ycoord = 1;
            zcoord = 2;
            break;
        case 1:         // YZ Plane
            xcoord = 1;
            ycoord = 2;
            zcoord = 0;
            break;
        case 2:         // XZ Plane
            xcoord = 0;
            ycoord = 2;
            zcoord = 1;
            break;
    }
	//分段数小于4给出警告后把分段数设置为4
	//warning是手动给出的
    if (divisions < 4)
    {
        // With the range restriction we have on the divisions, this
        // is actually impossible, but it shows how to add an error
        // message or warning to the SOP.
        addWarning(SOP_MESSAGE, "Invalid divisions");
        divisions = 4;
    }
	//中断行为
    // Start the interrupt server
    UT_AutoInterrupt boss("Building Star");
    if (boss.wasInterrupted())
    {
        return;
    }

    // Build a polygon
	//这里的dst是传进来的gdp。div决定了创建多少个点。然后指定模型为封闭。接下来直接set点位置就好
    GEO_PrimPoly *poly = GEO_PrimPoly::build(dst, divisions, GU_POLY_CLOSED);
    float tinc = M_PI*2 / (float)divisions;

    // Now, set all the points of the polygon
    for (int i = 0; i < divisions; i++)
    {
		//用户中断
        // Check to see if the user has interrupted us...
        if (boss.wasInterrupted())
            break;

        // Since we expect the local variables to be used in specifying
        // the radii, we have to evaluate the channels INSIDE the loop
        // through the points...
		//根据循环次数设定temp
        float tmp = (float)i * tinc;
		//用‘按位和’运算判断循环奇偶数。一外一内就是星星的形状
        float rad = (i & 1) ? XRADIUS(now) : YRADIUS(now);
		//如果没有启用负值半径的情况下半径小于0那就把半径设置为0
        if (!negradius && rad < 0)
            rad = 0;
		//这里的xyzcoord不是字面的xyz位置。是索引号。在上面的switch确定下来的
        UT_Vector3 pos;
		//计算点位置。cos得到位置乘半径加中心点
        pos(xcoord) = SYScos(tmp) * rad + tx;
        pos(ycoord) = SYSsin(tmp) * rad + ty;
		//0加中心点
        pos(zcoord) = 0 + tz;
		//设置点位置
        GA_Offset ptoff = poly->getPointOffset(i);
        dst->setPos3(ptoff, pos);
    }

}
//设置第二个输出端
GU_DetailHandle
SOP_DualStar::cookMySopOutput(OP_Context &context, int outputidx, SOP_Node *interests)
{
	//创建一个新的gdh
    GU_DetailHandle	result;
	//创建新的gdp
    GU_Detail		*dst = new GU_Detail();
	//设置gdh将其指向给定的gdp
    result.allocateAndSet(dst);
	//用新的gdh创建几何。这样子会丢掉输入的几何体
    buildStar(dst, context);
	//返回gdh
    return result;
}
