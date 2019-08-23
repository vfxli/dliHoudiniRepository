///*������Ӵ���һ���򵥵�Բ׶��״*/  
#ifndef __sop_mycone_h__  
#define __sop_mycone_h__  
#include<SOP/SOP_Node.h>  
  
class SOP_MyCone : public SOP_Node  
{  
public:  
    static OP_Node *myConstructor(OP_Network *, const char *, OP_Operator *);//����sop�ڵ�  
    static PRM_Template myTemplateList[];//�ڵ���������Ҫ�Ĳ���  
protected:  
    SOP_MyCone(OP_Network *net, const char *name, OP_Operator *op);//����  
    virtual ~SOP_MyCone();//������  
    virtual OP_ERROR cookMySop(OP_Context &context);//sop���ļ���  
private:  
    //�������Ĳ������ڸýڵ�˽��  
    fpreal CENTERX(fpreal t) { return evalFloat("t", 0, t); }  
    fpreal CENTERY(fpreal t) { return evalFloat("t", 1, t); }  
    fpreal CENTERZ(fpreal t) { return evalFloat("t", 2, t); }  
    fpreal HEIGHT(fpreal t) { return evalFloat("height", 0, t); }  
    fpreal RADIUS(fpreal t) { return evalFloat("rad", 0, t); }  
    fpreal SCALE(fpreal t) { return evalFloat("s", 0, t); }  
    int DIVSION(fpreal t) { return evalInt("divs", 0, t); }  
};  
  
#endif  