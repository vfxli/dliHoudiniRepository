#include <SOP/SOP_Node.h>

//��SOP_Node�̳�
class SOP_Mypoint : public SOP_Node
{
public:
    //��������
    static OP_Node *myConstructor(OP_Network*, const char*, OP_Operator*);
    //������
    static PRM_Template myTemplateList[];
protected:
    SOP_Mypoint(OP_Network* net, const char* name, OP_Operator* op);
    virtual ~SOP_Mypoint();
    //�����ڵ����ݵĺ���
    virtual OP_ERROR cookMySop(OP_Context &context);
private:
    //��ȡ������
    //����������
    fpreal CREATE_NPOINTS(fpreal t) { return evalFloat("npoints", 0, t); }
    //������ļ��
    fpreal STEP(fpreal t) { return evalFloat("step", 0, t); }
    //��ɫ
    fpreal  COLORX(fpreal t) { return evalFloat("color", 0, 0); }
    fpreal  COLORY(fpreal t) { return evalFloat("color", 1, 0); }
    fpreal  COLORZ(fpreal t) { return evalFloat("color", 2, 0); }
};