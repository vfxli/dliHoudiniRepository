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
* The PointWave SOP.  This SOP PointWaves the geometry onto a plane.
*/

#include "SOP_Mypointwave.h"

#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <SYS/SYS_Math.h>
#include <stddef.h>

using namespace HDK_Sample;

void
newSopOperator(OP_OperatorTable *table)
{
	table->addOperator(new OP_Operator(
		"hdk_mypointwave",
		"Mypoint Wave",
		SOP_Mypointwave::myConstructor,
		SOP_Mypointwave::myTemplateList,
		1,
		1,
		0));
}

static PRM_Name names[] = {
	PRM_Name("amp",     "Amplitude"),
	PRM_Name("phase",   "Phase"),
	PRM_Name("period",  "Period"),
};

PRM_Template
SOP_Mypointwave::myTemplateList[] = {
	//（类型，参数尺寸，参数名，默认值，字符串参数样式，范围，回执，sparedata添加选组按钮）
	PRM_Template(PRM_STRING,    1, &PRMgroupName, 0, &SOP_Node::pointGroupMenu,
	0, 0, SOP_Node::getGroupSelectButton(
		GA_GROUP_POINT)),
	PRM_Template(PRM_FLT_J,	1, &names[0], PRMoneDefaults, 0,
		&PRMscaleRange),
	PRM_Template(PRM_FLT_J,	1, &names[1], PRMzeroDefaults),
	PRM_Template(PRM_FLT_J,	1, &names[2], PRMoneDefaults),
	PRM_Template(),
};


OP_Node *
SOP_Mypointwave::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
	return new SOP_Mypointwave(net, name, op);
}

SOP_Mypointwave::SOP_Mypointwave(OP_Network *net, const char *name, OP_Operator *op)
	: SOP_Node(net, name, op), myGroup(NULL)
{
	// This indicates that this SOP manually manages its data IDs,
	// so that Houdini can identify what attributes may have changed,
	// e.g. to reduce work for the viewport, or other SOPs that
	// check whether data IDs have changed.
	// By default, (i.e. if this line weren't here), all data IDs
	// would be bumped after the SOP cook, to indicate that
	// everything might have changed.
	// If some data IDs don't get bumped properly, the viewport
	// may not update, or SOPs that check data IDs
	// may not cook correctly, so be *very* careful!
	mySopFlags.setManagesDataIDs(true);
}

SOP_Mypointwave::~SOP_Mypointwave() {}

OP_ERROR
SOP_Mypointwave::cookInputGroups(OP_Context &context, int alone)
{
	// The SOP_Node::cookInputPointGroups() provides a good default
	// implementation for just handling a point selection.
	return cookInputPointGroups(
		context, // This is needed for cooking the group parameter, and cooking the input if alone.
		myGroup, // The group (or NULL) is written to myGroup if not alone.
		alone,   // This is true iff called outside of cookMySop to update handles.
				 // true means the group will be for the input geometry.
				 // false means the group will be for gdp (the working/output geometry).
		true,    // (default) true means to set the selection to the group if not alone and the highlight flag is on.
		0,       // (default) Parameter index of the group field
		-1,      // (default) Parameter index of the group type field (-1 since there isn't one)
		true,    // (default) true means that a pointer to an existing group is okay; false means group is always new.
		false,   // (default) false means new groups should be unordered; true means new groups should be ordered.
		true,    // (default) true means that all new groups should be detached, so not owned by the detail;
				 //           false means that new point and primitive groups on gdp will be owned by gdp.
		0        // (default) Index of the input whose geometry the group will be made for if alone.
	);
}

OP_ERROR
SOP_Mypointwave::cookMySop(OP_Context &context)
{
	// We must lock our inputs before we try to access their geometry.
	// OP_AutoLockInputs will automatically unlock our inputs when we return.
	// NOTE: Don't call unlockInputs yourself when using this!
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();

	// Duplicate our incoming geometry with the hint that we only
	// altered points.  Thus, if our input was unchanged, we can
	// easily roll back our changes by copying point values.
	duplicatePointSource(0, context);

	fpreal t = context.getTime();

	// We evaluate our parameters outside the loop for speed.  If we
	// wanted local variable support, we'd have to do more setup
	// (see SOP_Flatten) and also move these inside the loop.
	float phase = PHASE(t);
	float amp = AMP(t);
	float period = PERIOD(t);

	if (error() >= UT_ERROR_ABORT)
		return error();

	// Here we determine which groups we have to work on.  We only
	// handle point groups.
	if (cookInputGroups(context) >= UT_ERROR_ABORT)
		return error();

	GA_Offset ptoff;
	GA_FOR_ALL_GROUP_PTOFF(gdp, myGroup, ptoff)
	{
		UT_Vector3 p = gdp->getPos3(ptoff);

		p.y() += SYSsin((p.x() / period + phase) * M_PI * 2) * amp;

		gdp->setPos3(ptoff, p);
	}

	// If we've modified P, and we're managing our own data IDs,
	// we must bump the data ID for P.
	if (!myGroup || !myGroup->isEmpty())
		gdp->getP()->bumpDataId();

	return error();
}
