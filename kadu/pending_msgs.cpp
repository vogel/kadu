
#include "pending_msgs.h"
#include "misc.h"

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

void PendingMsgs::saveToFile()
{
	char* path=preparePath("kadu.msgs");
	QFile f(path);
	if(!f.open(IO_WriteOnly))
	{
		fprintf(stderr,"KK PendingMsgs::saveToFile(): Cannot open file kadu.msgs");
		return;
	};
	// Najpierw zapisujemy ilosc wiadomosci
	int t=msgs.size()
	f.writeBlock(&t,sizeof(int));
	// Teraz w petli dla kazdej wiadomosci
	for(Iterator i=msgs.begin(); i!=msgs.end(); i++)
	{
		// zapisujemy uiny - najpierw ilosc
		t=(*i).uins.size();
		f.writeBlock(&t,sizeof(int));
		// teraz dane
		for(Iterator j=(*i).uins.begin(); j!=(*i).uins.end(); j++)
			f.writeBlock(&(*j),sizeof(uin_t));
		// nastepnie wiadomosc - dlugosc
		t=(*i).msg.length();
		f.writeBlock(&t,sizeof(int));
		// i tresc
		f.writeBlock((*i).msg.local8Bit(),t);
		// na koniec jeszcze klase wiadomosci
		f.writeBlock(&(*i).msgclass,sizeof(int));
		// i czas
		f.writeBlock(&(*i).time,sizeof(time_t));
	};
	// I zamykamy plik
	f.close();
};

void PendingMsgs::loadFromFile()
{
	char* path=preparePath("kadu.msgs");
	QFile f(path);
	if(!f.open(IO_ReadOnly))
	{
		fprintf(stderr,"KK PendingMsgs::loadFromFile(): Cannot open file kadu.msgs");
		return;
	};
	// Najpierw wczytujemy ilosc wiadomosci
	int msgs_size;
	f.readBlock(&msgs_size,sizeof(int));
	// Teraz w petli dla kazdej wiadomosci
	for(int i=0; i<msgs_size; i++)
	{
		PendingMsgsElement e;
		// wczytujemy uiny - najpierw ilosc
		int uins_size;
		f.readBlock(&uins_size,sizeof(int));
		// teraz dane
		for(int j=0; j<uins_size; j++)
		{
			int uin;
			f.readBlock(&uin,sizeof(uin_t));
			e.uins.append(uin);
		};
		// nastepnie wiadomosc - dlugosc
		int msg_size;
		f.readBlock(&msg_size,sizeof(int));
		// i tresc
		char* buf=new char[msg_size+1];
		f.readBlock(buf,msg_size);
		buf[msg_size]=0;
		e.msg=buf;
		delete[] buf;
		// na koniec jeszcze klase wiadomosci
		f.readBlock(&e.msgclass,sizeof(int));
		// i czas
		f.readBlock(&e.time,sizeof(time_t));
	};
	// I zamykamy plik
	f.close();
};
