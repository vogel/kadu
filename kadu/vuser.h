#ifndef VUSER_H
#define VUSER_H

#include <qstring.h>
#include <qobject.h>
#include <qprocess.h>
#include "libgadu.h"

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

class VUserScript : public QObject
{
Q_OBJECT
private:
	vuVoidReturner* msg_handler;
public slots:
	void shellProcessOutputReady();
	void shellProcessFinished();
public:
	QString str_reg;
	QMap<QString,int> variables;
	QProcess* shell_process;
	uin_t shell_process_uin;
	int shell_process_class;
	VUserScript(QString filename);
	void eventMsg(uin_t sender,int msgclass,QString msg);
//	void eventChangeStatus(QString user,int status);
};

//extern VUserScript script;

#endif
