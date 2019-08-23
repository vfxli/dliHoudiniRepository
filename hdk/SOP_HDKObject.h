//������
#ifndef __SOP_HDKObject_h__
#define __SOP_HDKObject_h__

#include <CH/CH_ExprLanguage.h>
//����eval�йصĺ����Ƕ�����OP_Parameters�����
//SOP_Node��OP_Network������OP_Network��OP_Node������
//OP_Node�Ƕ��ؼ̳е��ࡣ���ĸ������OP_Parameters��
//������SOP_Node����Ӧ���а������㼶��include
#include <SOP/SOP_Node.h>
//�����ռ�
namespace HDK_Sample {
class SOP_HDKObject : public SOP_Node
{
public:
	    SOP_HDKObject(OP_Network *net, const char *name, 
			  OP_Operator *entry);
    virtual ~SOP_HDKObject();
	//��̬parms
    virtual bool		 updateParmsFlags();
	//addOperator��ʱ����Ҫ����myConstructor�������
	//����ֵ�Ǳ����һ��ʵ��
    static OP_Node		*myConstructor(OP_Network *net, const char *name, OP_Operator *entry);
    static PRM_Template		 myTemplateList[];
    static PRM_Template		 myObsoleteList[];

    // Because the object merge can reference an op chain which has a 
    // subnet with different D & R we must follow all our node's
    // d & r status
    virtual int			 getDandROpsEqual();
    virtual int			 updateDandROpsEqual(int = 1) 
				 { return getDandROpsEqual(); }

    int 	NUMOBJ() { return evalInt("numobj", 0, 0.0f); }
    void	setNUMOBJ(int num_obj)
		    { setInt("numobj", 0, 0.0f, num_obj); }

    int		ENABLEMERGE(int i)
		{
		    return evalIntInst("enable#", &i, 0, 0.0f);
		}
    void	setENABLEMERGE(int i, int val)
		{
		    setIntInst(val, "enable#", &i, 0, 0.0f);
		}

    void	SOPPATH(UT_String &str, int i, fpreal t)
		{
		    evalStringInst("objpath#", &i, str, 0, t);
		}
    void	setSOPPATH( UT_String &str, CH_StringMeaning meaning,
			    int i, fpreal t)
		{
		    setStringInst(str, meaning, "objpath#", &i, 0, t);
		}

    void	XFORMPATH(UT_String &str, fpreal t)
		{
		    evalString(str, "xformpath", 0, t);
		}
protected:
    virtual OP_ERROR		 cookMySop(OP_Context &context);
};
} // End HDK_Sample namespace

#endif
