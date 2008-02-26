/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtextcodec.h>
//Added by qt3to4:
#include <Q3CString>

#include "chat_manager.h"
#include "debug.h"
#include "gadu.h"
#include "misc.h"
#include "pending_msgs.h"
#include "userlist.h"

PendingMsgs::Element::Element() : users(), proto(), msg(), msgclass(0), time(0)
{
}

PendingMsgs::PendingMsgs(QObject *parent, const char *name) : QObject(parent, name), msgs()
{
}

void PendingMsgs::deleteMsg(int index)
{
	kdebugm(KDEBUG_INFO, "PendingMsgs::(pre)deleteMsg(%d), count=%d\n", index, count());
	UserListElement e = msgs[index].users[0];
	msgs.remove(msgs.at(index));
	writeToFile();
	kdebugm(KDEBUG_INFO, "PendingMsgs::deleteMsg(%d), count=%d\n", index, count());
	emit messageFromUserDeleted(e);
}

bool PendingMsgs::pendingMsgs(UserListElement user) const
{
//	kdebugf();

//	what is it for?
//	if (uin == 0)
//		return pendingMsgs();

	CONST_FOREACH(msg, msgs)
		if((*msg).users[0] == user)
		{
//			kdebugf2();
			return true;
		}
//	kdebugf2();
	return false;
}

bool PendingMsgs::pendingMsgs() const
{
	return !msgs.isEmpty();
}

int PendingMsgs::count() const
{
	return msgs.count();
}

PendingMsgs::Element &PendingMsgs::operator[](int index)
{
	return msgs[index];
}

void PendingMsgs::addMsg(QString protocolName, UserListElements users, QString msg, int msgclass, time_t time)
{
	Element e;
	e.users = users;
	e.proto = protocolName;
	e.msg = msg;
	e.msgclass = msgclass;
	e.time = time;
	msgs.append(e);
	writeToFile();
	emit messageFromUserAdded(users[0]);
}

void PendingMsgs::writeToFile()
{
	QString path = ggPath("kadu.msgs");
	QFile f(path);
	if(!f.open(QIODevice::WriteOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Cannot open file kadu.msgs\n");
		return;
	}
	// first we write number of messages
	int t = msgs.count();
	f.writeBlock((char*)&t,sizeof(int));
	// next for each message
	CONST_FOREACH(i, msgs)
	{
		// saving uins, first - number of
		t=(*i).users.size();
		f.writeBlock((char*)&t,sizeof(int));
		// uins
		CONST_FOREACH(j, (*i).users)
		{
			UinType uin = (*j).ID("Gadu").toUInt();
			f.writeBlock((char*)&uin, sizeof(UinType));
		}
		// message size
		t=(*i).msg.length();
		f.writeBlock((char*)&t,sizeof(int));
		// message content
		Q3CString cmsg = codec_latin2->fromUnicode((*i).msg);
		f.writeBlock(cmsg, cmsg.length());
		// message class
		f.writeBlock((char*)&(*i).msgclass,sizeof(int));
		// and time
		f.writeBlock((char*)&(*i).time,sizeof(time_t));
	}
	// close file
	f.close();
}

bool PendingMsgs::loadFromFile()
{
	QString path = ggPath("kadu.msgs");
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly)) {
		kdebugmf(KDEBUG_WARNING, "Cannot open file kadu.msgs\n");
		return false;
	}

	// first we read number of messages
	int msgs_size;
	if (f.readBlock((char*)&msgs_size,sizeof(int)) <= 0) {
		kdebugmf(KDEBUG_ERROR, "kadu.msgs is corrupted\n");
		return false;
	}

	// next for each message
	for (int i = 0; i < msgs_size; ++i)
	{
		Element e;

		// reading uins, first number of
		int uins_size;
		if (f.readBlock((char*)&uins_size, sizeof(int)) <= 0) {
			--msgs_size;
			return false;
		}

		// uins
		for (int j = 0; j < uins_size; ++j)
		{
			int uin;
			if (f.readBlock((char*)&uin, sizeof(UinType)) <= 0) {
				--msgs_size;
				return false;
			}
			e.users.append(userlist->byID("Gadu", QString::number(uin)));
		}

		// message size
		int msg_size;
		if (f.readBlock((char*)&msg_size, sizeof(int)) <= 0) {
			--msgs_size;
			return false;
		}

		// message content
		char *buf = new char[msg_size + 1];
		if (f.readBlock(buf, msg_size) <= 0) {
			--msgs_size;
			delete [] buf;
			return false;
		}
		buf[msg_size] = 0;
		e.msg = codec_latin2->toUnicode(buf);
		delete[] buf;

		// message class
		if (f.readBlock((char*)&e.msgclass, sizeof(int)) <= 0) {
			--msgs_size;
			delete [] buf;
			return false;
		}

		// and time
		if (f.readBlock((char*)&e.time, sizeof(time_t)) <= 0) {
			--msgs_size;
			delete [] buf;
			return false;
		}

		// appending to list
		msgs.append(e);

		emit messageFromUserAdded(e.users[0]);
	}

	// and closing file
	f.close();
	return true;
}

void PendingMsgs::openMessages()
{
	chat_manager->openPendingMsgs();
}

PendingMsgs pending(NULL, "pending_msgs");
