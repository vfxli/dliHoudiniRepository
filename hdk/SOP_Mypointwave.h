
#ifndef __SOP_Mypointwave_h__
#define __SOP_Mypointwave_h__

#include <SOP/SOP_Node.h>

namespace HDK_Sample
{
class SOP_Mypointwave : public SOP_Node
{
public:
	SOP_Mypointwave(OP_Network* net, const char* name, OP_Operator* op);
	~SOP_Mypointwave();

	static PRM_Template myTemplateList[];
	static OP_Node *myConstructor(OP_Network*, const char*, OP_Operator*);
	//这里alone案例给的是0，但是这个函数需要的是一个布尔值，所以可以改成false
	virtual OP_ERROR cookInputGroups(OP_Context &context, int alone = false);
protected:
	virtual OP_ERROR cookMySop(OP_Context &context);
private:
	//这个getGroup只是写在了这里。这个案例并没有实现它
	void getGroup(UT_String &str) { evalString(str, "group", 0, 0); }
	fpreal	AMP(fpreal t) { return evalFloat("amp", 0, t); }
	fpreal	PHASE(fpreal t) { return evalFloat("phase", 0, t); }
	fpreal	PERIOD(fpreal t) { return evalFloat("period", 0, t); }
	const GA_PointGroup *myGroup;
};

}//end namespace

#endif