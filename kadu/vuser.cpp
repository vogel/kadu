#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include "../libgadu/lib/libgadu.h"
#include "kadu.h"
#include "vuser.h"

class vuIntReturner : public vuTreeNode
{
	public:
		vuIntReturner(VUserScript* script) : vuTreeNode(script) {};
		virtual bool eval(int& i)=0;
		static vuIntReturner* parse(VUserScript* script,QString& s);
};

class vuStrReturner : public vuTreeNode
{
	public:
		vuStrReturner(VUserScript* script) : vuTreeNode(script) {};	
		virtual bool eval(QString& s)=0;
		static vuStrReturner* parse(VUserScript* script,QString& s);
};

class vuVariable : public vuIntReturner
{
private:
	QString name;
public:
	vuVariable(VUserScript* script,QString name) : vuIntReturner(script)
	{
		vuVariable::name=name;
	};
	virtual bool eval(int& i)
	{
		i=script->variables[name];
		fprintf(stderr,"VUSER Variable %s=%i\n",name.local8Bit().data(),i);
		return true;
	};
	static vuIntReturner* parse(VUserScript* script,QString& s)
	{
		s=s.stripWhiteSpace();
		if(s[0].isLetter())
		{
			QString name;
			for(int i=0; i<s.length()&&(!s[i].isSpace()); i++)
				name+=s[i];		
			s=s.right(s.length()-name.length());
			fprintf(stderr,"VUSER Parsed variable:\n");
			return new vuVariable(script,name);
		}
		else
			return NULL;
	};
};

class vuStrConst : public vuStrReturner
{
private:
	QString data;
public:
	vuStrConst(VUserScript* script,QString data) : vuStrReturner(script)
	{
		vuStrConst::data=data;
	};	
	virtual bool eval(QString& s)
	{
		s=data;
		return true;
	};
	static vuStrReturner* parse(VUserScript* script,QString& s)
	{
		s=s.stripWhiteSpace();
		if(s[0]=='"')
		{
			QString data;
			for(int i=1; i<s.length()&&s[i]!='"'; i++)
				data+=s[i];
			s=s.right(s.length()-data.length()-2);
			fprintf(stderr,"VUSER Parsed const:\n");
			return new vuStrConst(script,data);
		}
		else
			return NULL;
	};
};

class vuStrReg : public vuStrReturner
{
public:
	vuStrReg(VUserScript* script) : vuStrReturner(script) {};	
	virtual bool eval(QString& s)
	{
		s=script->str_reg;
		return true;
	};
	static vuStrReturner* parse(VUserScript* script,QString& s)
	{
		fprintf(stderr,"VUSER Parsing strreg: %s\n",s.local8Bit().data());
		s=s.stripWhiteSpace();
		if(s.startsWith("MSG"))
		{
			s=s.right(s.length()-3);
			fprintf(stderr,"VUSER Parsed strreg:\n");
			return new vuStrReg(script);
		}
		else
			return NULL;
	};	
};

class vuSendMsg : public vuVoidReturner
{
	private:
		vuIntReturner* uin;
		vuIntReturner* msgclass;
		vuStrReturner* msg;
	public:
		vuSendMsg(VUserScript* script,vuIntReturner* uin,vuIntReturner* msgclass,vuStrReturner* msg)
			: vuVoidReturner(script)
		{
			vuSendMsg::uin=uin;
			vuSendMsg::msgclass=msgclass;
			vuSendMsg::msg=msg;			
		};
		virtual bool eval()
		{
			int u; int c; QString m;
			fprintf(stderr,"VUSER send start\n");
			uin->eval(u);
			msgclass->eval(c);
			msg->eval(m);
			m="V-USER: "+m;
			fprintf(stderr,"VUSER before send\n");
			gg_send_message(&sess,c,u,(const unsigned char*)m.local8Bit().data());
			return true;
		};		
		static vuVoidReturner* parse(VUserScript* script,QString& s)
		{
			fprintf(stderr,"VUSER Parsing send: %s\n",s.local8Bit().data());
			s=s.stripWhiteSpace();
			if(s.startsWith("send"))
			{
				s=s.right(s.length()-4);
				vuIntReturner* uin=vuIntReturner::parse(script,s);
				if(uin==NULL) return NULL;
				vuIntReturner* msgclass=vuIntReturner::parse(script,s);
				if(msgclass==NULL)
				{
					delete uin;
					return NULL;
				};
				vuStrReturner* msg=vuStrReturner::parse(script,s);
				if(msg==NULL)
				{
					delete uin;
					delete msgclass;
					return NULL;
				};
				return new vuSendMsg(script,uin,msgclass,msg);
			}
			else
				return NULL;
		};
};

class vuIf : public vuVoidReturner
{
	private:
		vuIntReturner* exp;
		vuVoidReturner* cmd;
		vuVoidReturner* els;		
	public:
		vuIf(VUserScript* script,vuIntReturner* exp,vuVoidReturner* cmd,vuVoidReturner* els)
			: vuVoidReturner(script)
		{
			vuIf::exp=exp;
			vuIf::cmd=cmd;
			vuIf::els=els;
		};
		virtual bool eval()
		{
			int e;
			exp->eval(e);
			if(e)
			{
				cmd->eval();
				fprintf(stderr,"VUSER If true\n");
			}
			else
			{
				if(els!=NULL)
				{
					els->eval();
					fprintf(stderr,"VUSER If false\n");
				};
			};
			return true;
		};		
		static vuVoidReturner* parse(VUserScript* script,QString& s)
		{
			s=s.stripWhiteSpace();
			if(s.startsWith("if"))
			{
				fprintf(stderr,"VUSER Parsing inside if\n");
				s=s.right(s.length()-2);
				vuIntReturner* exp=vuIntReturner::parse(script,s);
				if(exp==NULL) return NULL;
				vuVoidReturner* cmd=vuVoidReturner::parse(script,s);
				if(cmd==NULL)
				{
					delete exp;
					return NULL;
				};
				vuVoidReturner* els=NULL;
				s=s.stripWhiteSpace();				
				if(s.startsWith("else"))
				{
					s=s.right(s.length()-4);
					els=vuVoidReturner::parse(script,s);
					if(els==NULL)
					{
						delete exp;
						delete cmd;
						return NULL;
					};					
				};
				return new vuIf(script,exp,cmd,els);
			}
			else
				return NULL;
		};

};

class vuContains : public vuIntReturner
{
	private:
		vuStrReturner* str;
		vuStrReturner* pat;		
	public:
		vuContains(VUserScript* script,vuStrReturner* str,vuStrReturner* pat)
			: vuIntReturner(script)
		{
			vuContains::str=str;
			vuContains::pat=pat;
		};
		virtual bool eval(int& i)
		{
			QString s; QString p;
			str->eval(s);
			pat->eval(p);
			i=s.contains(p);
			fprintf(stderr,"VUSER Contains %i\n",i);
			return true;
		};		
		static vuIntReturner* parse(VUserScript* script,QString& s)
		{
			fprintf(stderr,"VUSER Parsing contains: %s\n",s.local8Bit().data());
			s=s.stripWhiteSpace();
			fprintf(stderr,"VUSER Parsing contains stripped: %s\n",s.local8Bit().data());			
			if(s.startsWith("contains"))
			{
				fprintf(stderr,"VUSER Parsing inside contains\n");
				s=s.right(s.length()-8);
				fprintf(stderr,"VUSER Parsing contains 1\n");				
				vuStrReturner* str=vuStrReturner::parse(script,s);
				if(str==NULL) return NULL;
				fprintf(stderr,"VUSER Parsing contains 2\n");				
				vuStrReturner* pat=vuStrReturner::parse(script,s);
				if(pat==NULL)
				{
					delete str;
					return NULL;
				};
				fprintf(stderr,"VUSER Parsed contains\n");
				return new vuContains(script,str,pat);
			}
			else
				return NULL;
		};
};



VUserScript::VUserScript(QString filename)
{
//	status_handler=NULL;
	QString path=QString("vuser/")+filename;
	QFile f(preparePath(path.local8Bit().data()));
	if(!f.open(IO_ReadOnly))
	{
		fprintf(stderr,"VUSER: Cannot open script file\n");
		return;
	};
	QTextStream t(&f);
	QString s=t.read();
	f.close();
	msg_handler=vuVoidReturner::parse(this,s);
	if(msg_handler==NULL)
		fprintf(stderr,"VUSER: SYNTAX ERROR\n");
};

void VUserScript::eventMsg(uin_t sender,int msgclass,QString msg)
{
	if(msg_handler==NULL)
		return;
	variables.insert("SENDER",sender);
	variables.insert("CLASS",msgclass);
	str_reg=msg;
	//runScript(msg_handler_index);
	msg_handler->eval();
};

/*
void VUserScript::eventChangeStatus(QString user,int status)
{
	if(status_handler_index<0)
		return;
	str_reg=
	runScript(status_handler_index);
};
*/

VUserScript script("script");

vuVoidReturner* vuVoidReturner::parse(VUserScript* script,QString& s)
{
	vuVoidReturner* r=NULL;
	if((r=vuSendMsg::parse(script,s))==NULL)
		if((r=vuIf::parse(script,s))==NULL)
			return NULL;
	return r;
};	

vuIntReturner* vuIntReturner::parse(VUserScript* script,QString& s)
{
	vuIntReturner* r=NULL;
	if((r=vuContains::parse(script,s))==NULL)
		if((r=vuVariable::parse(script,s))==NULL)
			return NULL;
	return r;
};	

vuStrReturner* vuStrReturner::parse(VUserScript* script,QString& s)
{
	vuStrReturner* r=NULL;
	if((r=vuStrConst::parse(script,s))==NULL)
		if((r=vuStrReg::parse(script,s))==NULL)
			return NULL;
	return r;
};	
