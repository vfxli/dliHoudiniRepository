#include <SOP\SOP_Node.h>

// 声明Simple_SOP类继承于SOP_Node
class Simple_SOP : public SOP_Node {
// 公有方法
public:
	// 声明阶段所以参数不用写名字
	static OP_Node *myConstructor(OP_Network*, const char *, OP_Operator *);	//how to build the sop
	static PRM_Template myTemplateList[];

protected:
	// 和类名名字相同的函数叫做构造函数，在类实例的时候会自动执行做一些初始化的工作。
	Simple_SOP(OP_Network *net, const char *name, OP_Operator *op);
	// 析构函数，在实例被销毁的时候自动执行的函数，此处为空。
	virtual ~Simple_SOP();
	virtual OP_ERROR cookMySop(OP_Context &context);
};