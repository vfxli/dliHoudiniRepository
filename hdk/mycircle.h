#include <SOP\SOP_Circle.h>
#include <OP/OP_Parameters.h>

class GEO_PrimParticle;

//д��ͷ�ļ��ڵ����������֣���û��ʵ�ֲ���
// һ���µ��ࣨ�ڵ㣩MyCircle�̳���SOP_Circle
class SOP_MyCircle : public SOP_Circle {
// ���е�
public:
	//���캯��
	SOP_MyCircle(OP_Network *net, const char *, OP_Operator *op);
	// static���ε����Ա���������౾�������ڶ��󣨼�ʵ���������static���Ա������û��thisָ��ġ�
	// thisָ����ָ�򱾶����ָ�롣����Ϊû��thisָ�룬����static���Ա�������ܷ��ʷ�static�����Ա��
	// ֻ�ܷ��� static���ε����Ա��
	// ����������
	static OP_Node *MyConstructor(OP_Network *net, const char *name, OP_Operator *op);
	// ������
	static OP_TemplatePair *buildTemplatePair();

protected:
	OP_ERROR cookMySop(OP_Context &context);
private:
	fpreal	COLORX(fpreal t) { return evalFloat("color", 0, 0); }
	fpreal	COLORY(fpreal t) { return evalFloat("color", 1, 0); }
	fpreal	COLORZ(fpreal t) { return evalFloat("color", 2, 0); }
};