#include <SOP/SOP_Node.h>
#include <OP/OP_Parameters.h>
#include <PRM/PRM_Include.h>

class SOP_ImportImpacts : public SOP_Node{
public:
	SOP_ImportImpacts(OP_Network *net, const char *name, OP_Operator *op);
	virtual ~SOP_ImportImpacts();
	static OP_Node *MyConstructor(OP_Network *net, const char *name, OP_Operator *op);
	static PRM_Template myParmTemplates[];

private:
	void PARM_DOPNET(UT_String &str) { evalString(str, "dopnet", 0, 0); }
	void PARM_OBJMAK(UT_String &str) { evalString(str, "objmask", 0, 0); }

protected:
	OP_ERROR cookMySop(OP_Context &context);
};