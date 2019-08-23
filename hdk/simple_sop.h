#include <SOP\SOP_Node.h>

// ����Simple_SOP��̳���SOP_Node
class Simple_SOP : public SOP_Node {
// ���з���
public:
	// �����׶����Բ�������д����
	static OP_Node *myConstructor(OP_Network*, const char *, OP_Operator *);	//how to build the sop
	static PRM_Template myTemplateList[];

protected:
	// ������������ͬ�ĺ����������캯��������ʵ����ʱ����Զ�ִ����һЩ��ʼ���Ĺ�����
	Simple_SOP(OP_Network *net, const char *name, OP_Operator *op);
	// ������������ʵ�������ٵ�ʱ���Զ�ִ�еĺ������˴�Ϊ�ա�
	virtual ~Simple_SOP();
	virtual OP_ERROR cookMySop(OP_Context &context);
};