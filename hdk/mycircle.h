#include <SOP\SOP_Circle.h>
#include <OP/OP_Parameters.h>

class GEO_PrimParticle;

//写在头文件内的是声明部分，即没有实现部分
// 一个新的类（节点）MyCircle继承于SOP_Circle
class SOP_MyCircle : public SOP_Circle {
// 公有的
public:
	//构造函数
	SOP_MyCircle(OP_Network *net, const char *, OP_Operator *op);
	// static修饰的类成员函数属于类本身，不属于对象（即实例），因此static类成员函数是没有this指针的。
	// this指针是指向本对象的指针。正因为没有this指针，所以static类成员函数不能访问非static的类成员。
	// 只能访问 static修饰的类成员。
	// 构造器函数
	static OP_Node *MyConstructor(OP_Network *net, const char *name, OP_Operator *op);
	// 参数表
	static OP_TemplatePair *buildTemplatePair();

protected:
	OP_ERROR cookMySop(OP_Context &context);
private:
	fpreal	COLORX(fpreal t) { return evalFloat("color", 0, 0); }
	fpreal	COLORY(fpreal t) { return evalFloat("color", 1, 0); }
	fpreal	COLORZ(fpreal t) { return evalFloat("color", 2, 0); }
};