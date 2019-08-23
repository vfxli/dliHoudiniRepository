
#include "stdio.h"
#include "SOP_HDKObject.h"
//在SOP_Node :: cookMySop期间，您有一个名为gdp的本地成员变量，类型为GU_Detail
#include <GU/GU_Detail.h>
#include <OP/OP_Director.h>
//创建一个新节点就是new一个新的OP_Operator对象放到OP_OperatorTable里面
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_Parm.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_DirUtil.h>

using namespace HDK_Sample;

// Define the new sop operator...
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "proto_objectmerge",
        "HDK Object Merge",
        SOP_HDKObject::myConstructor,
        SOP_HDKObject::myTemplateList,
        0, 0, 0, OP_FLAG_GENERATOR));
}
//参数表name列表
//关联到头文件功能实现部分
static PRM_Name objnames[] =
{
    PRM_Name("numobj",	"Number of Objects"),
    PRM_Name("objpath#","Object #"),
    PRM_Name("xformpath","Transform Object"),
    PRM_Name("enable#", "Enable Merge #"),
    PRM_Name(0)
};

static PRM_Template	theObjectTemplates[] = {
    PRM_Template(PRM_TOGGLE, 1, &objnames[3], PRMoneDefaults),
    PRM_Template(PRM_STRING,	PRM_TYPE_DYNAMIC_PATH, 1,
		 &objnames[1], 0, 0,
		 0, 0, &PRM_SpareData::sopPath),
    PRM_Template()
};
//参数表静态部分
PRM_Template
SOP_HDKObject::myTemplateList[] = 
{
	//obj数量
    PRM_Template(PRM_MULTITYPE_LIST, theObjectTemplates, 2, &objnames[0],
		    PRMoneDefaults),
	//transform obj
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &objnames[2],
		0, 0, 0, 0, &PRM_SpareData::objPath),
    PRM_Template()
};

OP_Node *
SOP_HDKObject::myConstructor(OP_Network *net, const char *name,
			    OP_Operator *entry)
{
    return new SOP_HDKObject(net, name, entry);
}


SOP_HDKObject::SOP_HDKObject(OP_Network *net, const char *name, OP_Operator *entry)
    : SOP_Node(net, name, entry)
{
    // NOTE: GEO_Detail::copy(), when GEO_COPY_ONCE is used, will copy all
    //       data IDs from the source, and GEO_Detail::transform() will
    //       bump data IDs of any transformed attributes, as well as the
    //       primitive list data ID if there are any transforming primitives
    //       that were transformed.
	//检查上游数据变化（来自于水哥博客）还是没理解
    mySopFlags.setManagesDataIDs(true);
}
SOP_HDKObject::~SOP_HDKObject() {}

bool
SOP_HDKObject::updateParmsFlags()
{
    bool changed = false;

    int n = NUMOBJ();
    for (int i = 1; i <= n; i++)
    {
        changed |= enableParmInst(objnames[1].getToken(), &i, ENABLEMERGE(i));
    }

    return changed;
}

int
SOP_HDKObject::getDandROpsEqual()
{
    // don't do anything if we're locked
    if (flags().getHardLocked())
        return 1;

    int numobj = NUMOBJ();

    // Determine if any of our SOPs are evil.
    for (int objindex = 1; objindex <= numobj; objindex++)
    {
        if (!ENABLEMERGE(objindex)) // Ignore disabled ones.
            continue;

        UT_String objname;
        SOPPATH(objname, objindex, 0.0f);

        OP_Network *objptr = (OP_Network *)findNode((const char *)objname);
        if (!objptr)
            continue;

        // Self-referential nodes are assumed to have equal DandR ops,
        // as it doesn't matter as they will be ignored and flagged
        // as errors during cook anyways.
        if (objptr == this)
        {
            continue;
        }

        if (!objptr->getDandROpsEqual())
        {
            return 0;
        }
    }
    // None of our interests was unequal, thus we are equal!
    return 1;
}
/*C++提供了 传递变量的引用。形参是引用变量，和实参是
一个变量，调用函数时，形参(引用变量)指向实参变量
单元。这种通过形参引用可以改变实参的值。*/
//这里context可能是被修改的数据？所以传入了引用？
OP_ERROR
SOP_HDKObject::cookMySop(OP_Context &context)
{
	//用context获得time
    fpreal t = context.getTime();

    // Get our xform object, if any.
    UT_String objname;
    XFORMPATH(objname, t);
	//OP_Network是OP_Node的子类。是SOP_Node的父类，所以他可以容纳节点数据
	//这里是找到那个要merge过来的节点
    OP_Network *xformobjptr = (OP_Network *)findNode(objname);
	//如果xformobjptr有值表示找到了节点并且用xformobjptr也就是OP_Network下的方法判断节点类型是否是SOP节点
    if (xformobjptr && xformobjptr->getOpTypeID() == SOP_OPTYPE_ID)
    {
        // The user pointed to the SOP.  We silently promote it to the
        // containing object.  This allows the intuitive "." to be used
        // for the path to transform relative to our own op (rather than
        // having to track up an arbitrary number of paths)

		//getCreator is similar to getParentNetwork but it climbs the tree to find the network that originally created this network type. 
		//This means that if you're down in a sub-network you climb right up to the parent of all the sub-nets. 
		//For SOPs this will return the object
		//getCreator类似于getParentNetwork，但它爬上树以找到最初创建此网络类型的网络。 
		//这意味着，如果您在子网络中，您可以直接爬到所有子网的父级。 
		//对于SOP，这将返回对象
		//
        xformobjptr = xformobjptr->getCreator();
    }

    // We must explicitly cast down as OBJ_Node * is unknown here.
    // We also do the cast here instead of in findNode as we want
    // to allow people to reference SOPs.
	//
    xformobjptr = (OP_Network *)CAST_OBJNODE(xformobjptr);
	//如果xformobjptr不正确
    if (!xformobjptr && objname.isstring())
    {
        // We didn't get an xform object, but they had something typed,
        // badmerge.
		//输出错误路径
        addError(SOP_BAD_SOP_MERGED, objname);
    }
	//得到numobj数量
    int numobj = NUMOBJ();

    // Peek as to whether we are a render cook or not.
	//getCreator()来自于OP_NetworkBoxItem。是OP_Node多重继承除了OP_Parameters以外的另一个父类
	//isCookingRender是检查节点是否在刷新。这里作用是初始化变量状态
    int cookrender = getCreator()->isCookingRender();

    bool copiedfirst = false;
    bool copiedlast = false;

    for (int objindex = 1; objindex <= numobj; objindex++)
    {
		//如果没勾enable merge就跳过
        if (!ENABLEMERGE(objindex))             // Ignore disabled ones.
            continue;

        UT_String sopname;
        SOPPATH(sopname, objindex, t);
		//如果sopname为空跳过
        if (!sopname.isstring())
            continue;                           // Blank means ignore.
		//get到sopname描述的节点
        SOP_Node *sopptr = getSOPNode(sopname, 1); // We want extra inputs.
		//如果是自己就给出警告然后继续执行下面的代码
        if (sopptr == this)
        {
            // Self-reference.  Special brand of evil.
            addWarning(SOP_ERR_SELFMERGE);
            continue;
        }
		//如果sopptr没有值就报错
        if (!sopptr)
        {
            // Illegal merge.  Just warn so we don't abort everything.
            addWarning(SOP_BAD_SOP_MERGED, sopname);
            continue;
        }
		//返回的是上游的所有节点
        // Get the creator, which is our objptr.
        OP_Network *objptr = sopptr->getCreator();

        // Change over so any subnet evaluation will properly track...
        int savecookrender = objptr->isCookingRender();
        objptr->setCookingRender(cookrender);

        // Actually cook...
        const GU_Detail *cookedgdp = sopptr->getCookedGeo(context);

        // Restore the cooking render state.
        objptr->setCookingRender(savecookrender);

        if (!cookedgdp)
        {
            // Something went wrong with the cooking. Warn the hapless user.
            addWarning(SOP_BAD_SOP_MERGED, sopname);
            continue;
        }

        // Now add the extra inputs...
        addExtraInput(objptr, OP_INTEREST_DATA);
        // The sop extra inputs were set by the getSOPNode

        bool firstmerge = !copiedfirst;

        // Choose the best copy method we can
        GEO_CopyMethod copymethod = GEO_COPY_ADD;
        if (!copiedfirst)
        {
            copymethod = GEO_COPY_START;
            copiedfirst = true;
            if (objindex == numobj)
            {
                copymethod = GEO_COPY_ONCE;
                copiedlast = true;
            }
        }
        else if (objindex == numobj)
        {
            copymethod = GEO_COPY_END;
            copiedlast = true;
        }

        // Mark where the new prims and points start
        GA_IndexMap::Marker pointmarker(gdp->getPointMap());
        GA_IndexMap::Marker primmarker(gdp->getPrimitiveMap());

        // Don't copy internal groups!
        // Accumulation of internal groups may ensue.
        gdp->copy(*cookedgdp, copymethod, true, false, GA_DATA_ID_CLONE);

        // Apply the transform.
        if (xformobjptr)
        {
            // GEO_Detail::transform supports double-precision,
            // so we might as well use double-precision transforms.
            UT_Matrix4D xform, xform2;

            if (!objptr->getWorldTransform(xform, context))
                addTransformError(*objptr, "world");
            if (!xformobjptr->getIWorldTransform(xform2, context))
                addTransformError(*xformobjptr, "inverse world");
            xform *= xform2;

            if (firstmerge)
            {
                // The first object we merge we can just do a full gdp transform
                // rather than building the subgroup.
                gdp->transform(xform);
            }
            else
            {
                gdp->transform(xform, primmarker.getRange(), pointmarker.getRange(), false);
            }
        }
    }

    if (xformobjptr)
    {
        addExtraInput(xformobjptr, OP_INTEREST_DATA);
    }

    // Finish & clean up the copy procedure, if not already done.
    if (!copiedfirst)
    {
        gdp->clearAndDestroy();
    }
    else if (!copiedlast)
    {
        GU_Detail blank_gdp;
        gdp->copy(blank_gdp, GEO_COPY_END, true, false, GA_DATA_ID_CLONE);
    }

    // Set the node selection for this primitive. This will highlight all
    // the primitives of the node, but only if the highlight flag for this node
    // is on and the node is selected.
    if (error() < UT_ERROR_ABORT)
	select(GA_GROUP_PRIMITIVE);

    return error();
}
