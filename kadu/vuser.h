
#include <qstring.h>



class VUserScript;

class vuTreeNode
{
protected:
	VUserScript* script;
public:
	vuTreeNode(VUserScript* script)
	{
		vuTreeNode::script=script;
	};
};

class vuVoidReturner : public vuTreeNode
{
	public:
		vuVoidReturner(VUserScript* script) : vuTreeNode(script) {};
		virtual bool eval()=0;
		static vuVoidReturner* parse(VUserScript* script,QString& s);
};

class VUserScript
{
private:
	vuVoidReturner* msg_handler;
public:
	QString str_reg;
	QMap<QString,int> variables;
	VUserScript(QString filename);
	void eventMsg(uin_t sender,int msgclass,QString msg);
//	void eventChangeStatus(QString user,int status);
};

extern VUserScript script;
