#include <SOP/SOP_Node.h>

//从SOP_Node继承
class SOP_Mypoint : public SOP_Node
{
public:
    //构建函数
    static OP_Node *myConstructor(OP_Network*, const char*, OP_Operator*);
    //参数表
    static PRM_Template myTemplateList[];
protected:
    SOP_Mypoint(OP_Network* net, const char* name, OP_Operator* op);
    virtual ~SOP_Mypoint();
    //操作节点数据的函数
    virtual OP_ERROR cookMySop(OP_Context &context);
private:
    //获取参数表
    //创建点数量
    fpreal CREATE_NPOINTS(fpreal t) { return evalFloat("npoints", 0, t); }
    //创建点的间距
    fpreal STEP(fpreal t) { return evalFloat("step", 0, t); }
    //颜色
    fpreal  COLORX(fpreal t) { return evalFloat("color", 0, 0); }
    fpreal  COLORY(fpreal t) { return evalFloat("color", 1, 0); }
    fpreal  COLORZ(fpreal t) { return evalFloat("color", 2, 0); }
};