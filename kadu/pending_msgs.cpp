
PendingMsgs::PendingMsgs()
{
};

void PendingMsgs::deleteMsg(int index)
{
	msgs.remove(msgs.at(index));	
};

bool PendingMsgs::pendingMsgs(int uin)
{
	if(uin==-1)
		return pendingMsgs();
	for(Iterator i=msgs.begin(); i!=msgs.end(); i++)
		if((*i).uins[0]==uin)
			return true;
	return false;
};

bool PendingMsgs::pendingMsgs(int uin)
{
	return (msgs.size()>0);
};

PendingMsgs::Element& PendingMsgs::accessNextMsg(uin_t uin)
{
	for(Iterator i=msgs.begin(); i!=msgs.end(); i++)
		if((*i).uins[0]==uin)
			return (*i);
	fprintf(stderr, "KK PendingMsgs::accessNextMsg(): Panic!\n");
	// Kadu Panic :) What we should do here???
};

PendingMsgs::Element& PendingMsgs::accessNextMsg()
{
	if(msgs.size()>0)
		return (*msgs.begin());
	fprintf(stderr, "KK PendingMsgs::accessNextMsg(): Panic!\n");
	// Kadu Panic :) What we should do here???
};

void PendingMsgs::deleteNextMsg(uin_t uin)
{
	for(Iterator i=msgs.begin(); i!=msgs.end(); i++)
		if((*i).uins[0]==uin)
		{
			msgs.remove(i);
			break;
		};
};

void PendingMsgs::deleteNextMsg()
{
	if(msgs.size()>0)
		msgs.remove(msgs.begin());
};

void PendingMsgs::addMsg(uin_t sender,QString msg,int msgclass,time_t time)
{
	Element e;
	e.uins.append(sender);
	e.msg=msg;
	e.msgclass=msgclass;
	e.time=time;
	msgs.append(e);
};

void PendingMsgs::addUin(uin_t uin)
{
	if(msgs.size()>0)
		(*msgs.end()).uins.append(uin);
};
