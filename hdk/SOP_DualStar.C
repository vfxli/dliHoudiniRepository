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
		//����û�У������Ӧ���Ǹ�һ����Ա��������SOP_Star::myVariables
        0,      // Local variables
		//�����ʾ�ýڵ���Ҫ����һ���µļ�����
        OP_FLAG_GENERATOR,        // Flag it as generator
	//**inputlabels
	//Note: The 'inputlabels' array is expected to be null-terminated
	0,
	//���������
	2));	// Outputs.
}
//���� ��ѡ�� �Ƿ�ʹ�ø�ֵ�뾶
static PRM_Name		negativeName("nradius", "Negative Radius");
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version
//Ĭ��ֵ ϸ����
static PRM_Default	fiveDefault(5);		// Default to 5 divisions
//Ĭ��ֵ ����뾶
static PRM_Default	radiiDefaults[] = {
			   PRM_Default(1),		// Outside radius
			   PRM_Default(0.3)		// Inside radius
			};
//������
PRM_Template
SOP_DualStar::myTemplateList[] = {
	//ϸ����
    PRM_Template(PRM_INT,			// Integer parameter.//��������
		//No extended type given but export level given
		//����������͵ľ�����;����֪��
		 PRM_Template::PRM_EXPORT_TBX,	// Export to top of viewer
						// when user selects this node
		 1, 		// One integer in this row/parameter//����Ԫ������/���μ�1��ʸ����3
		 &PRMdivName,   // Name of this parameter - must be static//Ĭ��ֵ����Ҫ�����ַ//����ʹ��ģ����
		 &fiveDefault,  // Default for this parameter - ditto//����ʹ�����涨���Ĭ��ֵ
		 0,		// Menu for this parameter//�˵���ʽ//����Ϊ��
		 &PRMdivision2Range // Valid range//�ֶ�����Χ//����ʹ��ģ��//��ģ�巶Χ��2-50
		 ),
	// �뾶
    PRM_Template(PRM_XYZ,	2, &PRMradiusName, radiiDefaults),
	//�Ƿ�ʹ�ø�ֵ�뾶
    PRM_Template(PRM_TOGGLE,	1, &negativeName),
	//���ĵ�
    PRM_Template(PRM_XYZ,       3, &PRMcenterName),
	//����
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
	//�����������ݣ���Ҫָ������˿�
    duplicateSource(0, context);

    // Start the interrupt server
	//�������ԭ�е�gdp�����������������뼸����Ļ����Ϲ�����
	//����˵ֱ����ʾ�ڵ��ͬʱ��ʾ����������
    buildStar(gdp, context);

    return error();
}

void
SOP_DualStar::buildStar(GU_Detail *dst, OP_Context &context)
{
	//get����
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
	//�ֶ���С��4���������ѷֶ�������Ϊ4
	//warning���ֶ�������
    if (divisions < 4)
    {
        // With the range restriction we have on the divisions, this
        // is actually impossible, but it shows how to add an error
        // message or warning to the SOP.
        addWarning(SOP_MESSAGE, "Invalid divisions");
        divisions = 4;
    }
	//�ж���Ϊ
    // Start the interrupt server
    UT_AutoInterrupt boss("Building Star");
    if (boss.wasInterrupted())
    {
        return;
    }

    // Build a polygon
	//�����dst�Ǵ�������gdp��div�����˴������ٸ��㡣Ȼ��ָ��ģ��Ϊ��ա�������ֱ��set��λ�þͺ�
    GEO_PrimPoly *poly = GEO_PrimPoly::build(dst, divisions, GU_POLY_CLOSED);
    float tinc = M_PI*2 / (float)divisions;

    // Now, set all the points of the polygon
    for (int i = 0; i < divisions; i++)
    {
		//�û��ж�
        // Check to see if the user has interrupted us...
        if (boss.wasInterrupted())
            break;

        // Since we expect the local variables to be used in specifying
        // the radii, we have to evaluate the channels INSIDE the loop
        // through the points...
		//����ѭ�������趨temp
        float tmp = (float)i * tinc;
		//�á���λ�͡������ж�ѭ����ż����һ��һ�ھ������ǵ���״
        float rad = (i & 1) ? XRADIUS(now) : YRADIUS(now);
		//���û�����ø�ֵ�뾶������°뾶С��0�ǾͰѰ뾶����Ϊ0
        if (!negradius && rad < 0)
            rad = 0;
		//�����xyzcoord���������xyzλ�á��������š��������switchȷ��������
        UT_Vector3 pos;
		//�����λ�á�cos�õ�λ�ó˰뾶�����ĵ�
        pos(xcoord) = SYScos(tmp) * rad + tx;
        pos(ycoord) = SYSsin(tmp) * rad + ty;
		//0�����ĵ�
        pos(zcoord) = 0 + tz;
		//���õ�λ��
        GA_Offset ptoff = poly->getPointOffset(i);
        dst->setPos3(ptoff, pos);
    }

}
//���õڶ��������
GU_DetailHandle
SOP_DualStar::cookMySopOutput(OP_Context &context, int outputidx, SOP_Node *interests)
{
	//����һ���µ�gdh
    GU_DetailHandle	result;
	//�����µ�gdp
    GU_Detail		*dst = new GU_Detail();
	//����gdh����ָ�������gdp
    result.allocateAndSet(dst);
	//���µ�gdh�������Ρ������ӻᶪ������ļ�����
    buildStar(dst, context);
	//����gdh
    return result;
}