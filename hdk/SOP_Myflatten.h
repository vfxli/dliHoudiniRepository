
//防止重复声明宏命令
#ifndef __SOP_Myflatten_h__
#define __SOP_Myflatten_h__
//SOP节点从这里继承
#include <SOP/SOP_Node.h>
//命名空间防止重复类名
namespace HDK_Sample {
class SOP_Myflatten : public SOP_Node
{
public:
	SOP_Myflatten(OP_Network* net, const char* name, OP_Operator* op);
	virtual ~SOP_Myflatten();
	//对于组的操作
	virtual OP_ERROR cookInputGroups(OP_Context &context, int alone = 0);
	//构建函数
	static OP_Node *myConstructor(OP_Network*, const char*, OP_Operator*);
	//参数表
	static PRM_Template myTemplateList[];
protected:
	virtual bool updateParmsFlags();
	//后面的const表示这个函数不会修改该类的任何成员数据的值
	//前面的const由于const在*前面所以表示不能通过该指针修改指向数据的值
	virtual const char *inputLabel(unsigned idx) const;
	//操作节点数据的函数
	virtual OP_ERROR cookMySop(OP_Context &context);
	//编写一个guide
	virtual OP_ERROR cookMyGuide1(OP_Context &context);
private:
	void	getGroups(UT_String &str) { evalString(str, "GROUP", 0, 0); }
	fpreal	DIST(fpreal t)			  { return evalFloat("dist", 0, t); }
	int		DIRPOP()				  { return evalInt("usedir", 0, 0); }
	//（参数名，参数idx，time）
	int		ORIENT()				  { return evalInt("orient", 0, 0); }
	fpreal	NX(fpreal t)			  { return evalFloat("dir", 0, t); }
	fpreal	NY(fpreal t)			  { return evalFloat("dir", 1, t); }
	fpreal	NZ(fpreal t)			  { return evalFloat("dir", 2, t); }
	//操作组
	const GA_PointGroup *myGroup;
};
} //END HDK_Sample namespace

#endif
