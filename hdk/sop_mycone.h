///*这个例子创建一个简单的圆锥形状*/  
#ifndef __sop_mycone_h__  
#define __sop_mycone_h__  
#include<SOP/SOP_Node.h>  
  
class SOP_MyCone : public SOP_Node  
{  
public:  
    static OP_Node *myConstructor(OP_Network *, const char *, OP_Operator *);//创建sop节点  
    static PRM_Template myTemplateList[];//节点参数面板需要的参数  
protected:  
    SOP_MyCone(OP_Network *net, const char *name, OP_Operator *op);//构造  
    virtual ~SOP_MyCone();//虚析构  
    virtual OP_ERROR cookMySop(OP_Context &context);//sop核心计算  
private:  
    //参数面板的参数属于该节点私有  
    fpreal CENTERX(fpreal t) { return evalFloat("t", 0, t); }  
    fpreal CENTERY(fpreal t) { return evalFloat("t", 1, t); }  
    fpreal CENTERZ(fpreal t) { return evalFloat("t", 2, t); }  
    fpreal HEIGHT(fpreal t) { return evalFloat("height", 0, t); }  
    fpreal RADIUS(fpreal t) { return evalFloat("rad", 0, t); }  
    fpreal SCALE(fpreal t) { return evalFloat("s", 0, t); }  
    int DIVSION(fpreal t) { return evalInt("divs", 0, t); }  
};  
  
#endif  