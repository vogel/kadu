/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qhostaddress.h>
#include <qdns.h>
#include <qptrlist.h>

#include "userlist.h"
#include "misc.h"
#include "kadu.h"
#include "userbox.h"
#include "debug.h"

DnsHandler::DnsHandler(uin_t uin) : uin(uin) {
	UserListElement &ule = userlist.byUin(uin);
	completed = false;
	connect(&dnsresolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	dnsresolver.setRecordType(QDns::Ptr);
	dnsresolver.setLabel(ule.ip);
	counter++;
	kdebug("DnsHandler::DnsHandler(): counter = %d\n", counter);
}

DnsHandler::~DnsHandler() {
	counter--;
/* patrz ~Userlist()
	kdebug("DnsHandler::~DnsHandler(): counter = %d\n", counter);
*/
}

void DnsHandler::resultsReady() {
	if (dnsresolver.hostNames().count())
		userlist.setDnsName(uin, dnsresolver.hostNames()[0]);
	else
		userlist.setDnsName(uin, QString::null);
	completed = true;
}

int DnsHandler::counter = 0;

bool DnsHandler::isCompleted() {
	return completed;
}

UserListElement::UserListElement(UserList* parent)
{
	Parent = parent;
	version = 0;
};

UserListElement::UserListElement()
{
	Parent = NULL;
	version = 0;
};

QString UserListElement::group()
{
	return Group;
};

void UserListElement::setGroup(const QString& group)
{
	if (group == i18n("All"))
		Group="";
	else
		Group=group;
	emit Parent->modified();
};

UserList::UserList() : QObject(), QValueList<UserListElement>()
{
	dnslookups.setAutoDelete(true);
};

UserList::~UserList()
{
	/* Ciekawa zagadka... ten kdebug przy zamykaniu powodowa³ naruszenie
		pamiêci obojêtnie, co mia³ w parametrach, debugowanie tego nie jest
		konieczne wiêc zakomentowa³em.
		kdebug("UserList::~UserList(): dnslookups.count() = %d\n", dnslookups.count());
	*/
	/* chilek: Mysle, ze nie jest to takie mocno ciekawe, zwazywszy na to, ze pare linii
		wyzej mamy w konstruktorze dnslookups ustawiony na autoDelete ;)
		Po prostu tu w destruktorze klasy UserList pewnie juz obiekt dnslookups zostal
		zniszczony i stad problem z wyswietleniem pewnych jego atrybutow.
	*/
}

void UserList::addDnsLookup(uin_t uin, const QHostAddress &ip) {
	DnsHandler *dnshandler;
	dnshandler = dnslookups.first();
	while (dnshandler) {
		if (dnshandler->isCompleted()) {
			dnslookups.remove();
			dnshandler = dnslookups.current();
			}
		else
			dnshandler = dnslookups.next();
		}
	if (!containsUin(uin))
		return;
	UserListElement &ule = byUin(uin);
	dnshandler = new DnsHandler(uin);
	dnslookups.append(dnshandler);
}

void UserList::setDnsName(uin_t uin, const QString &name) {
	if (!containsUin(uin))
		return;
	UserListElement &ule = byUin(uin);
	if (ule.dnsname != name) {
		ule.dnsname = name;
		kdebug("UserList::setDnsName(): dnsname for uin %d: %s\n", uin, name.latin1());
		emit dnsNameReady(uin);
		}
}

UserListElement& UserList::byUin(uin_t uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return (*i);
	kdebug("UserList::byUin(): Panic!\n");
	// Kadu Panic :) What we should do here???
};

UserListElement& UserList::byNick(const QString& nickname)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).nickname==nickname)
			return (*i);
	kdebug("UserList::byNick(): Panic! %s not exists\n",(const char*)nickname.local8Bit());
	// Kadu Panic :) What we should do here???
};

UserListElement& UserList::byAltNick(const QString& altnick)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).altnick==altnick)
			return (*i);
	kdebug("UserList::byAltNick(): Panic! %s not exists\n",(const char*)altnick.local8Bit());
	// Kadu Panic :) What we should do here???
};

//Zwraca elementy userlisty, jezeli nie mamy danego
//uin na liscie, zwracany jest UserListElement tylko z uin i altnick == uin
UserListElement UserList::byUinValue(uin_t uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return (*i);
	UserListElement ule;
	ule.uin=uin;
	ule.altnick=QString::number(uin);
	ule.anonymous=true;
	return ule;
};

bool UserList::containsUin(uin_t uin) {
	for (iterator i = begin(); i != end(); i++)
		if ((*i).uin == uin)
			return true;
	kdebug("UserList::containsUin(): userlist doesnt contain %d\n", uin);
	return false;
}

bool UserList::containsAltNick(const QString &altnick) {
	for (iterator i = begin(); i != end(); i++)
		if ((*i).altnick == altnick)
			return true;
	kdebug("UserList::containsAltNick(): userlist doesnt contain %s\n", (const char *)altnick.local8Bit());
	return false;
}

void UserList::addUser(const QString &FirstName,const QString &LastName,
	const QString &NickName,const QString &AltNick,
	const QString &Mobile,const QString &Uin,const int Status,
	const int Image_size,
	const bool Blocking, const bool Offline_to_user, const bool Notify,
	const QString &Group,const QString &Description, const QString &Email,
	const bool Anonymous)
{
	UserListElement e(this);
	e.first_name = FirstName;
	e.last_name = LastName;
	e.nickname = NickName;
	e.altnick = AltNick;
	e.mobile = Mobile;
	e.uin = Uin.toUInt();
	e.status = Status;
	e.image_size = Image_size;
	e.blocking = Blocking;
	e.offline_to_user = Offline_to_user;
	e.notify = Notify;
	if (Group != i18n("All"))
		e.Group = Group;
	else
		e.Group = "";
	e.description = Description;
	e.email = Email;
	e.anonymous = Anonymous;
	e.port = 0;
	append(e);
	emit modified();
};

void UserList::changeUserInfo(const QString &OldAltNick,
	const QString &FirstName, const QString &LastName,
	const QString &NickName, const QString &AltNick,
	const QString &Mobile, const QString &Uin, int Status,
	const int Image_size,
	const bool Blocking, const bool Offline_to_user, const bool Notify,
	const QString &Group, const QString &Email)
{
	UserListElement &e = byAltNick(OldAltNick);
	e.first_name = FirstName;
	e.last_name = LastName;
	e.nickname = NickName;
	e.altnick = AltNick;
	e.mobile = Mobile;
	e.email = Email;
	bool ok;
	uin_t uin;
	uin = Uin.toUInt(&ok);
	if (ok)
		e.uin = uin;
	e.anonymous = false;
	e.status = Status;
	e.image_size = Image_size;
	e.blocking = Blocking;
	e.offline_to_user = Offline_to_user;
	e.notify = Notify;
	if (Group != i18n("All"))
		e.Group = Group;
	else
		e.Group = "";
	if (AltNick != OldAltNick) {
		UserBox::all_renameUser(OldAltNick,AltNick);
		UserBox::all_refresh();			
		}
	emit modified();
};

void UserList::changeUserStatus(const uin_t uin, const unsigned int status)
{
	UserListElement &e = byUin(uin);
	if (status != e.status) {
		e.status = status;
//		UserBox::all_refresh();			
		emit statusModified(&e);
		}
};

void UserList::removeUser(const QString &altnick)
{
	for (Iterator i = begin(); i != end(); i++)
		if((*i).altnick == altnick)
		{ 
			remove(i);
			emit modified();
			break;
		};
};

bool UserList::writeToFile(QString filename)
{
	QString faname;
	QString tmp;

	tmp = ggPath("");
	mkdir(tmp.local8Bit(), 0700);

	if (!filename.length()) {
		filename = ggPath("userlist");
		}

	faname = ggPath("userattribs");

	kdebug("UserList::writeToFile(): %s\n", (const char *)filename.local8Bit());

	QFile f(filename);

	if (!f.open(IO_WriteOnly)) {
		kdebug("UserList::writeToFile(): Error opening file :(\n");
		return false;
		}

	QString s;
	QCString str;
	for (Iterator i = begin(); i != end(); i++) {
		s.truncate(0);
		s.append((*i).first_name);
		s.append(QString(";"));
		s.append((*i).last_name);
		s.append(QString(";"));
		s.append((*i).nickname);
		s.append(QString(";"));
		s.append((*i).altnick);
		s.append(QString(";"));
		s.append((*i).mobile);
		s.append(QString(";"));
		s.append((*i).group());
		s.append(QString(";"));
		if ((*i).uin)
			s.append(QString::number((*i).uin));
		s.append(QString(";"));
		s.append((*i).email);
		s.append(QString("\r\n"));
		
		if (!(*i).anonymous) {
			kdebug(s.local8Bit());
			str = QTextCodec::codecForName("ISO 8859-2")->fromUnicode(s);
			f.writeBlock(str, str.length());
			}
		}
	f.close();

	QFile fa(faname);

	if (!fa.open(IO_WriteOnly)) {
		kdebug("UserList::writeToFile(): Error opening file :(\n");
		return false;
		}

	for (Iterator i = begin(); i != end(); i++) {
		s.truncate(0);
		s.append(QString::number((*i).uin));
		s.append(QString(";"));
		s.append((*i).blocking ? QString("true") : QString("false"));
		s.append(QString(";"));
		s.append((*i).offline_to_user ? QString("true") : QString("false"));
		s.append(QString(";"));
		s.append((*i).notify ? QString("true") : QString("false"));
		s.append(QString("\r\n"));
		
		if (!(*i).anonymous && (*i).uin) {
			kdebug(s.local8Bit());
			str = QTextCodec::codecForName("ISO 8859-2")->fromUnicode(s);
			fa.writeBlock(str, str.length());
			}
		}
	fa.close();
		
	return true;
}

bool UserList::readFromFile()
{
	QString path;
	QValueList<QStringList> ualist;
	QString line;

	path = ggPath("userattribs");
	kdebug("UserList::readFromFile(): Opening userattribs file: %s\n",
		(const char *)path.local8Bit());
	QFile fa(path);
	if (!fa.open(IO_ReadOnly)) {
		kdebug("UserList::readFromFile(): Error opening userattribs file\n");
		}
	else {
		QTextStream s(&fa);
		while ((line = s.readLine()).length()) {
			QStringList slist;
			slist = QStringList::split(';', line);
			if (slist.count() == 4)
				ualist.append(slist);
			}
		fa.close();
		}

	path = ggPath("userlist");
	kdebug("UserList::readFromFile(): Opening userlist file: %s\n",
		(const char *)path.local8Bit());
	QFile f(path);
	if (!f.open(IO_ReadOnly)) {
		kdebug("UserList::readFromFile(): Error opening userlist file");
		return false;
		}

	kdebug("UserList::readFromFile(): File opened successfuly\n");

	clear();

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	while ((line = t.readLine()).length()) {
		if (line[0] == '#')
			continue;

		if (line.find(';') < 0) {
			QString nickname = line.section(' ',0,0);
			QString uin = line.section(' ',1,1);
			if (uin == "")
				continue;
			addUser("" , "", nickname, nickname,
				"", uin, GG_STATUS_NOT_AVAIL, 0, "", "");
			}
		else {	    
			QString first_name = line.section(';', 0, 0);
			QString last_name = line.section(';', 1, 1);
			QString nickname = line.section(';', 2, 2);
			QString altnick = line.section(';', 3, 3);
			QString mobile = line.section(';', 4, 4);
			QString group = line.section(';', 5, 5);
			QString uin = line.section(';', 6, 6);
			QString email = line.section(';', 7, 7);

			if (uin == "")
				uin = "0";
				
			if (altnick == "") {
				if (nickname == "")
					altnick = first_name;
				else
					altnick = nickname;
				}

			bool blocking, offline_to_user, notify;

			QValueList<QStringList>::Iterator i = ualist.begin();
			while ((*i)[0] != uin && i != ualist.end())
				i++;
			if (i != ualist.end()) {
				blocking = ((*i)[1] == "true" ? true : false);
				offline_to_user = ((*i)[2] == "true" ? true : false);
				notify = ((*i)[3] == "true" ? true : false);
				}
			else {
				blocking = false;
				offline_to_user = false;
				notify = true;
				}

			addUser(first_name, last_name, nickname, altnick,
				mobile, uin, GG_STATUS_NOT_AVAIL, 0, blocking, offline_to_user,
				notify, group, "", email);
			}
		}

	f.close();
	emit modified();
    	return true;
}

UserList& UserList::operator=(const UserList& userlist)
{
	QValueList<UserListElement>::operator=(userlist);
	emit modified();
	return *this;
};

UserList userlist;
