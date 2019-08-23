
#include "SOP_Myflatten.h"
#include <UT/UT_DSOVersion.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <UT/UT_Interrupt.h>
#include <iostream>

using namespace HDK_Sample;

void
newSopOperator(OP_OperatorTable *table)
{
	//table frome <OP/OP_OperatorTable.h>
	table->addOperator(new OP_Operator(
		"hdk_myflatten",
		"Myflatten",
		SOP_Myflatten::myConstructor,
		SOP_Myflatten::myTemplateList,
		1,
		1,
		NULL));
}
//构建函数
OP_Node* SOP_Myflatten::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
	return new SOP_Myflatten(net, name, op);
}
//构造函数
SOP_Myflatten::SOP_Myflatten(OP_Network* net, const char* name, OP_Operator* op)
	: SOP_Node(net, name, op), myGroup(NULL)
{
	//手动操作数据ID
	//设置为true这个就是比较高级的做法
	//这允许我们检查一些可能存在的重复运算
	//当数据ID得到妥善处理的情况下可以显著的提高性能
	//比如当拓扑没变化的时候
	mySopFlags.setManagesDataIDs(true);
	//确保输入一个几何体
	mySopFlags.setNeedGuide1(true);
}
//析构函数
SOP_Myflatten::~SOP_Myflatten() {}
//参数名
//PRM_Name frome <OP/OP_OperatorTable.h> or "SOP_Myflatten.h"
static PRM_Name names[] = {
	PRM_Name("usedir", "Use Direction Vector"),
	PRM_Name("dist", "Distance"),
};
//参数表
PRM_Template SOP_Myflatten::myTemplateList[] = {
	//（类型，参数尺寸，参数名，默认值，字符串后面的向下的小箭头叫做选择列表，范围，回执，sparedata添加选组按钮）
	//使用的是重载的第一项No extended type given and no export level given.
	PRM_Template(PRM_STRING, 1, &PRMgroupName, 0, &SOP_Node::pointGroupMenu,
				 0, 0, SOP_Node::getGroupSelectButton(GA_GROUP_POINT)),
	//（类型，参数尺寸，参数名，默认值，选择列表为空，范围）
	PRM_Template(PRM_FLT_J,	1, &names[1], PRMzeroDefaults, 0, &PRMscaleRange),
	//（类型复选框，参数尺寸，参数名）
	PRM_Template(PRM_TOGGLE, 1, &names[0]),
	//（类型三平面下拉菜单，参数尺寸，参数名，默认值，选择列表）
	PRM_Template(PRM_ORD, 1, &PRMorientName, 0, &PRMplaneMenu),
	//（类型方向，尺寸3矢量，参数名，默认值Z为1）
	PRM_Template(PRM_DIRECTION, 3, &PRMdirectionName, PRMzaxisDefaults),
	//（空对象）
	PRM_Template(),
};
//设置参数状态是否可用
bool SOP_Myflatten::updateParmsFlags()
{
	bool changed;
	//（pi=parm iex，参数值）
	//（如果use direction vector勾选了设置orientation不可用）
	//（如果没勾选设置可用）
	changed  = enableParm(3, !DIRPOP());
	//（如果use direction vector勾选了设置orientation可用）
	//（如果没勾选设置不可用）
	changed |= enableParm(4, DIRPOP());
	return changed;
}
OP_ERROR SOP_Myflatten::cookInputGroups(OP_Context &context, int alone)
{
	//这个函数会查找PRMgroupName这个是和parameter直接绑定的
	return cookInputPointGroups(
		context,	//指定上下文，用于绑定到group parameter
		myGroup,	//该节点的操作组。
					//无论上游有多少组在这个节点只使用一个组就是这个myGroup
		alone,		//如果在cookMySop外调用那么这个将会是true
					//true意味着组将用于输入几何
					//flase意味着组将用于gdp
		true,		//允许手动选择设置为组
		0,			//(default)指定组的parameter索引
		-1,			//(default)指定组级别的parameter索引
					//-1因为没有这个参数，并且我们在设计节点的时候就只支持点组
		true,		//(default)true表示使用已经存在的组是允许的
					//false表示必须指定一个新组
		false,		//(default)false表示新的组应该是无序的
					//true表示新的组应该是有序的
		true,		// (default) true means that all new groups should be detached, so not owned by the detail;
					//           false means that new point and primitive groups on gdp will be owned by gdp.
					//下面这一条符合当前状态
					//If fetchgdp is false, you must supply a gdp to create groups on in the paramerer pgdp.
		0			//(default)表示使用哪个输入端如果是单一的
	);
}
OP_ERROR SOP_Myflatten::cookMySop(OP_Context &context)
{
	//锁定上游//这个函数会在return的时候自动解锁
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();
	//获取时间
	fpreal now = context.getTime();
	//复制输入的几何体
	duplicateSource(0, context);
	//绑定变量优先级(detail,prim,pt,vtx)
	setVariableOrder(3, 2, 0, 1);
	//绑定本地变量到第一个输入
	setCurGdh(0, myGdpHandle);
	//查找属性并且绑定到本地变量
	setupLocalVars();
	//确定要处理那些组
	//UT_ERROR_ABORT错误意味停止当前操作
	//如果当前没有错误并且输入组也没有错误并且myGroup不是空的
	if (error() < UT_ERROR_ABORT && cookInputGroups(context) < UT_ERROR_ABORT &&
		(!myGroup || !myGroup->isEmpty()))
	{
		//有组的情况，在组内的操作

		//允许用ESC,progress是终止句柄，随时用这个对象进行终止操作
		UT_AutoInterrupt progress("Flattening Points");
		//可以容纳指定属性类型的数组（只有一个pos属性）
		UT_Array<GA_RWHandleV3> positionattribs(1);
		UT_Array<GA_RWHandleV3> normalattribs;
		UT_Array<GA_RWHandleV3> vectorattribs;
		//属性类型变量用于从gdp上面获取属性暂时存到这里
		GA_Attribute *attrib;
		//获取gdp上的属性并且同时操作所有属性
		//按照属性确定的循环次数并且每次循环拿到不同属性的指针
		GA_FOR_ALL_POINT_ATTRIBUTES(gdp, attrib)
		{
			//std::cout << "for = " << std::endl;
			//如果是浮点属性则跳过本次循环
			//用于减少不必要的计算
			if (!attrib->needsTransform())
				continue;
			//获得属性类型
			GA_TypeInfo typeinfo = attrib->getTypeInfo();
			if (typeinfo == GA_TYPE_POINT || typeinfo == GA_TYPE_HPOINT)
			{
				//得到属性操作句柄
				GA_RWHandleV3 handle(attrib);
				//确定属性是有效的
				if (handle.isValid())
				{
					//把属性操作手柄添加到数组内
					positionattribs.append(handle);
					//因为设置了mySopFlags.setManagesDataIDs(true)
					//刷新该属性的数据id
					attrib->bumpDataId();
				}
			}
			else if (typeinfo == GA_TYPE_NORMAL)
			{
				GA_RWHandleV3 handle(attrib);
				if (handle.isValid())
				{
					normalattribs.append(handle);
					attrib->bumpDataId();
				}
			}
			else if (typeinfo == GA_TYPE_VECTOR)
			{
				GA_RWHandleV3 handle(attrib);
				if (handle.isValid())
				{
					vectorattribs.append(handle);
					attrib->bumpDataId();
				}
			}
		}

		GA_Offset start;
		GA_Offset end;
		//1024个点为单位分块处理
		//初始值不重要了就是个it对象
		//it.blockAdvance第一次调用start为0，之后每次调用start+1024
		//这里start和end虽然是传值进去的但是it这个对象是有权限修改这两个变量的值的
		//it.blockAdvance每次调用也会返回一个布尔值决定是否进行下一次迭代
		//这样分块处理hdk会自动开启多线程么？？？？
		for (GA_Iterator it(gdp->getPointRange(myGroup)); it.blockAdvance(start, end);)
		{
			//std::cout << "*it=" << *it << std::endl;
			//std::cout << "start=" << start << std::endl;
			//std::cout << "end=" << end << std::endl;
			//esc取消操作
			if (progress.wasInterrupted())
				break;
			//对点进行循环
			for (GA_Offset ptoff = start; ptoff < end; ++ptoff)
			{
				//自动绑定局部变量
				myCurPtOff[0] = ptoff;
				//获得方向参数
				float dist = DIST(now);
				//用于在orient模式下控制方向
				UT_Vector3 normal;
				//如果没有勾选使用usedir
				if (!DIRPOP())
				{
					switch (ORIENT())
					{
					case 0:
						//xy平面
						normal.assign(0, 0, 1);
						break;
					case 1:
						//yz平面
						normal.assign(1, 0, 0);
						break;
					case 2:
						//xz平面
						normal.assign(0, 1, 0);
						break;
					}
				}
				//如果勾选了使用dist
				else
				{
					//拿到参数
					normal.assign(NX(now), NY(now), NZ(now));
					//规格化//c++的库还是保留了一些常用操作的。比如normalize
					normal.normalize();
				}
				//第二层循环对属性进行循环
				//pos只有一个。只循环一次
				for (exint i = 0; i < positionattribs.size(); ++i)
				{
					//拿到pos属性
					UT_Vector3 p = positionattribs(i).get(ptoff);
					//假设使用orient模式y轴压平那么就是xz平面
					//dot(normal, p)会得到y轴的偏移值*normal会把normalx和z轴归零
					//如果没有勾选dist那么P减去这个值就会把y轴归0
					//dist就是个偏移值
					p -= normal * (dot(normal, p) - dist);
					positionattribs(i).set(ptoff, p);
				}
				//循环所有法线类型属性
				for (exint i = 0; i < normalattribs.size(); ++i)
				{
					//拿到发现类型属性
					UT_Vector3 n = normalattribs(i).get(ptoff);
					//法线类型属性设置为单方向的
					//如果是反方向就是-的normal
					if (dot(normal, n) < 0)
						n = -normal;
					else
						//正轴向既直接使用normal
						n = normal;
					normalattribs(i).set(ptoff, n);
				}
				//循环所有矢量类型属性
				for (exint i = 0; i < vectorattribs.size(); ++i)
				{
					UT_Vector3 v = vectorattribs(i).get(ptoff);
					//和set pos一样。只是去掉了偏移值
					v -= normal * dot(normal, v);
					vectorattribs(i).set(ptoff, v);
				}

			}
		}
	}
	//重置本地变量
	//帮助文档编写sop下的sop中的局部变量，并没有看懂。
	resetLocalVarRefs();
	return error();
}
//编写GUIDE
OP_ERROR SOP_Myflatten::cookMyGuide1(OP_Context &context)
{
	//分段数
	const int divs = 5;
	//自动锁定上游。自动的意思是这个函数在return的时候会自动解锁
	OP_AutoLockInputs inputs(this);
	//如果锁定全部上游出现错误则返回错误
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();
	//获取时间
	float now = context.getTime();
	//清除掉MyGuide1
	//避免每次调参数重复生成guide
	myGuide1->clearAndDestroy();
	//获得dist参数
	float dist = DIST(now);
	//默认为z轴
	float nx = 0;
	float ny = 0;
	float nz = 1;
	//如果没有使用usedir就判断orient模式下的平面
	if (!DIRPOP())
	{
		switch (ORIENT())
		{
		case 0: // XY Plane
			nx = 0; ny = 0; nz = 1;
			break;
		case 1: // YZ Plane
			nx = 1; ny = 0; nz = 0;
			break;
		case 2: // XZ Plane
			nx = 0; ny = 1; nz = 0;
			break;
		}
	}
	//否则用usedir赋值
	else
	{
		nx = NX(now); ny = NY(now); nz = NZ(now);
	}
	//判断是否有错误
	if (error() >= UT_ERROR_ABORT)
		return error();
	//获得方向并且规格化
	UT_Vector3 normal(nx, ny, nz);
	normal.normalize();
	//一个boundbox对象
	UT_BoundingBox bbox;
	//得到bbox边框大小
	inputGeo(0, context)->getBBox(&bbox);
	//计算出size
	float sx = bbox.sizeX();
	float sy = bbox.sizeY();
	float sz = bbox.sizeZ();
	float size = SYSsqrt(sx*sx + sy*sy + sz*sz);
	//规格化后的normal乘偏移值
	float cx = normal.x() * dist;
	float cy = normal.y() * dist;
	float cz = normal.z() * dist;
	//创建meshGrid类型的Guide指定分段数和size
	myGuide1->meshGrid(divs, divs, size, size);
	//求出Z轴向旋转到normal的矩阵
	UT_Vector3 zaxis(0, 0, 1);
	UT_Matrix3 mat3;
	mat3.dihedral(zaxis, normal);
	//附加位置信息
	UT_Matrix4 xform;
	xform = mat3;
	xform.translate(cx, cy, cz);
	//设置guide位置
	myGuide1->transform(xform);

	return error();
}
//设置鼠标放在端口上的提示信息
const char *
SOP_Myflatten::inputLabel(unsigned) const
{
	return "Geometry to Flatten";
}
