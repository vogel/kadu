
#include <qfile.h>
#include <qtextcodec.h>

#include "pending_msgs.h"
#include "debug.h"
#include "kadu.h"
#include "config_file.h"
#include "chat.h"

PendingMsgs::PendingMsgs(): QObject()
{
}

void PendingMsgs::deleteMsg(int index)
{
	kdebug("PendingMsgs::(pre)deleteMsg(%d), count=%d\n", index, count());
	msgs.remove(msgs.at(index));
	writeToFile();
	kdebug("PendingMsgs::deleteMsg(%d), count=%d\n", index, count());
	emit messageDeleted();
}

bool PendingMsgs::pendingMsgs(UinType uin)
{
	if(uin==0)
		return pendingMsgs();
	for(PendingMsgsList::iterator i=msgs.begin(); i!=msgs.end(); i++)
		if((*i).uins[0]==uin)
			return true;
	return false;
}

bool PendingMsgs::pendingMsgs()
{
	return (msgs.size()>0);
}

int PendingMsgs::count()
{
	return msgs.count();
}

PendingMsgs::Element &PendingMsgs::operator[](int index)
{
	return msgs[index];
}

void PendingMsgs::addMsg(UinsList uins, QString msg, int msgclass, time_t time)
{
	Element e;
	e.uins = uins;
	e.msg = msg;
	e.msgclass = msgclass;
	e.time = time;
	msgs.append(e);
	writeToFile();
	emit messageAdded();
}

void PendingMsgs::writeToFile()
{
	QString path = ggPath("kadu.msgs");
	QFile f(path);
	if(!f.open(IO_WriteOnly))
	{
		kdebug("PendingMsgs::saveToFile(): Cannot open file kadu.msgs\n");
		return;
	}
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
			f.writeBlock((char*)&(*j),sizeof(UinType));
		// nastepnie wiadomosc - dlugosc
		t=(*i).msg.length();
		f.writeBlock((char*)&t,sizeof(int));
		// i tresc
		QCString cmsg = QTextCodec::codecForName("ISO 8859-2")->fromUnicode((*i).msg);
		f.writeBlock(cmsg, cmsg.length());
		// na koniec jeszcze klase wiadomosci
		f.writeBlock((char*)&(*i).msgclass,sizeof(int));
		// i czas
		f.writeBlock((char*)&(*i).time,sizeof(time_t));
	}
	// I zamykamy plik
	f.close();
}

bool PendingMsgs::loadFromFile()
{
	QString path = ggPath("kadu.msgs");
	QFile f(path);
	if (!f.open(IO_ReadOnly)) {
		kdebug("PendingMsgs::loadFromFile(): Cannot open file kadu.msgs\n");
		return false;
		}
	// Najpierw wczytujemy ilosc wiadomosci
	int msgs_size;
	if (f.readBlock((char*)&msgs_size,sizeof(int)) <= 0) {
		kdebug("PendingMsgs::loadFromFile(): kadu.msgs is corrupted\n");
		return false;
		}
	// Teraz w petli dla kazdej wiadomosci
	for (int i = 0; i < msgs_size; i++)
	{
		Element e;
		// wczytujemy uiny - najpierw ilosc
		int uins_size;
		if (f.readBlock((char*)&uins_size, sizeof(int)) <= 0) {
			msgs_size--;
			return false;
			}
		// teraz dane
		for (int j = 0; j < uins_size; j++)
		{
			int uin;
			if (f.readBlock((char*)&uin, sizeof(UinType)) <= 0) {
				msgs_size--;
				return false;
				}
			e.uins.append(uin);
		}
		// nastepnie wiadomosc - dlugosc
		int msg_size;
		if (f.readBlock((char*)&msg_size, sizeof(int)) <= 0) {
			msgs_size--;
			return false;
			}		
		// i tresc
		char *buf = new char[msg_size + 1];
		if (f.readBlock(buf, msg_size) <= 0) {
			msgs_size--;
			delete [] buf;
			return false;
			}				
		buf[msg_size] = 0;
		e.msg = QTextCodec::codecForName("ISO 8859-2")->toUnicode(buf);
		delete[] buf;
		// na koniec jeszcze klase wiadomosci
		if (f.readBlock((char*)&e.msgclass, sizeof(int)) <= 0) {
			msgs_size--;
			delete [] buf;
			return false;
			}						
		// i czas
		if (f.readBlock((char*)&e.time, sizeof(time_t)) <= 0) {
			msgs_size--;
			delete [] buf;
			return false;
			}						
		// dodajemy do listy
		msgs.append(e);
	}
	// I zamykamy plik
	f.close();
	return true;
}

void PendingMsgs::openMessages() {
	chat_manager->openPendingMsgs();
}

PendingMsgs pending;
