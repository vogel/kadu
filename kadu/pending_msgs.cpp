
#include <qfile.h>
#include "pending_msgs.h"
#include "misc.h"

PendingMsgs::PendingMsgs()
{
};

void PendingMsgs::deleteMsg(int index)
{
	msgs.remove(msgs.at(index));	
};

bool PendingMsgs::pendingMsgs(uin_t uin)
{
	if(uin==-1)
		return pendingMsgs();
	for(PendingMsgsList::iterator i=msgs.begin(); i!=msgs.end(); i++)
		if((*i).uins[0]==uin)
			return true;
	return false;
};

bool PendingMsgs::pendingMsgs()
{
	return (msgs.size()>0);
};

int PendingMsgs::count()
{
	return msgs.count();
};

PendingMsgs::Element &PendingMsgs::operator[](int index)
{
	return msgs[index];
};

void PendingMsgs::addMsg(UinsList uins, QString msg, int msgclass, time_t time)
{
	Element e;
	e.uins = uins;
	e.msg = msg;
	e.msgclass = msgclass;
	e.time = time;
	msgs.append(e);
};

void PendingMsgs::writeToFile()
{
	char* path=preparePath("kadu.msgs");
	QFile f(path);
	if(!f.open(IO_WriteOnly))
	{
		fprintf(stderr,"KK PendingMsgs::saveToFile(): Cannot open file kadu.msgs\n");
		return;
	};
	// Najpierw zapisujemy ilosc wiadomosci
	int t=msgs.count();
	f.writeBlock((char*)&t,sizeof(int));
	// Teraz w petli dla kazdej wiadomosci
	for(PendingMsgsList::iterator i=msgs.begin(); i!=msgs.end(); i++)
	{
		// zapisujemy uiny - najpierw ilosc
		t=(*i).uins.size();
		f.writeBlock((char*)&t,sizeof(int));
		// teraz dane
		for(UinsList::iterator j=(*i).uins.begin(); j!=(*i).uins.end(); j++)
			f.writeBlock((char*)&(*j),sizeof(uin_t));
		// nastepnie wiadomosc - dlugosc
		t=(*i).msg.length();
		f.writeBlock((char*)&t,sizeof(int));
		// i tresc
		f.writeBlock((*i).msg.local8Bit(),t);
		// na koniec jeszcze klase wiadomosci
		f.writeBlock((char*)&(*i).msgclass,sizeof(int));
		// i czas
		f.writeBlock((char*)&(*i).time,sizeof(time_t));
	};
	// I zamykamy plik
	f.close();
};

bool PendingMsgs::loadFromFile()
{
	char* path=preparePath("kadu.msgs");
	QFile f(path);
	if(!f.open(IO_ReadOnly))
	{
		fprintf(stderr,"KK PendingMsgs::loadFromFile(): Cannot open file kadu.msgs\n");
		return false;
	};
	// Najpierw wczytujemy ilosc wiadomosci
	int msgs_size;
	f.readBlock((char*)&msgs_size,sizeof(int));
	// Teraz w petli dla kazdej wiadomosci
	for(int i=0; i<msgs_size; i++)
	{
		Element e;
		// wczytujemy uiny - najpierw ilosc
		int uins_size;
		f.readBlock((char*)&uins_size,sizeof(int));
		// teraz dane
		for(int j=0; j<uins_size; j++)
		{
			int uin;
			f.readBlock((char*)&uin,sizeof(uin_t));
			e.uins.append(uin);
		};
		// nastepnie wiadomosc - dlugosc
		int msg_size;
		f.readBlock((char*)&msg_size,sizeof(int));
		// i tresc
		char* buf=new char[msg_size+1];
		f.readBlock(buf,msg_size);
		buf[msg_size]=0;
		e.msg = __c2q(buf);
		delete[] buf;
		// na koniec jeszcze klase wiadomosci
		f.readBlock((char*)&e.msgclass,sizeof(int));
		// i czas
		f.readBlock((char*)&e.time,sizeof(time_t));
		// dodajemy do listy
		msgs.append(e);
	};
	// I zamykamy plik
	f.close();
	return true;
};
