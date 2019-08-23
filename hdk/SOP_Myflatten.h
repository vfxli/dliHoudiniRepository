
//��ֹ�ظ�����������
#ifndef __SOP_Myflatten_h__
#define __SOP_Myflatten_h__
//SOP�ڵ������̳�
#include <SOP/SOP_Node.h>
//�����ռ��ֹ�ظ�����
namespace HDK_Sample {
class SOP_Myflatten : public SOP_Node
{
public:
	SOP_Myflatten(OP_Network* net, const char* name, OP_Operator* op);
	virtual ~SOP_Myflatten();
	//������Ĳ���
	virtual OP_ERROR cookInputGroups(OP_Context &context, int alone = 0);
	//��������
	static OP_Node *myConstructor(OP_Network*, const char*, OP_Operator*);
	//������
	static PRM_Template myTemplateList[];
protected:
	virtual bool updateParmsFlags();
	//�����const��ʾ������������޸ĸ�����κγ�Ա���ݵ�ֵ
	//ǰ���const����const��*ǰ�����Ա�ʾ����ͨ����ָ���޸�ָ�����ݵ�ֵ
	virtual const char *inputLabel(unsigned idx) const;
	//�����ڵ����ݵĺ���
	virtual OP_ERROR cookMySop(OP_Context &context);
	//��дһ��guide
	virtual OP_ERROR cookMyGuide1(OP_Context &context);
private:
	void	getGroups(UT_String &str) { evalString(str, "GROUP", 0, 0); }
	fpreal	DIST(fpreal t)			  { return evalFloat("dist", 0, t); }
	int		DIRPOP()				  { return evalInt("usedir", 0, 0); }
	//��������������idx��time��
	int		ORIENT()				  { return evalInt("orient", 0, 0); }
	fpreal	NX(fpreal t)			  { return evalFloat("dir", 0, t); }
	fpreal	NY(fpreal t)			  { return evalFloat("dir", 1, t); }
	fpreal	NZ(fpreal t)			  { return evalFloat("dir", 2, t); }
	//������
	const GA_PointGroup *myGroup;
};
} //END HDK_Sample namespace

#endif
