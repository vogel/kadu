/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qdns.h>
#include <qregexp.h>

#include <sys/stat.h>

#include "userlist.h"
#include "misc.h"
#include "userbox.h"
#include "debug.h"

DnsHandler::DnsHandler(UinType uin) : uin(uin) {
	kdebugf();
	UserListElement &ule = userlist.byUin(uin);
	completed = false;
	connect(&dnsresolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	dnsresolver.setRecordType(QDns::Ptr);
	dnsresolver.setLabel(ule.ip);
	counter++;
	kdebug_mask(KADU_DEBUG_INFO, "DnsHandler::DnsHandler(): counter = %d\n", counter);
}

DnsHandler::~DnsHandler() {
	counter--;
/* patrz ~Userlist()
	kdebug("DnsHandler::~DnsHandler(): counter = %d\n", counter);
*/
}

void DnsHandler::resultsReady() {
	kdebugf();
	if (dnsresolver.hostNames().count())
		userlist.setDnsName(uin, dnsresolver.hostNames()[0]);
	else
		userlist.setDnsName(uin, QString::null);
	completed = true;
	kdebugf2();
}

int DnsHandler::counter = 0;

bool DnsHandler::isCompleted() {
	return completed;
}

UserListElement::UserListElement(UserList* parent)
{
	Parent = parent;
	version = 0;
	status = GG_STATUS_NOT_AVAIL;
	image_size = 0;
	port = 0;
	blocking = false;
	offline_to_user = false;
	notify = true;
	anonymous = false;
}

UserListElement::UserListElement()
{
	Parent = NULL;
	version = 0;
	status = GG_STATUS_NOT_AVAIL;
	image_size = 0;
	port = 0;
	QHostAddress ipaddress;
	ipaddress.setAddress("0.0.0.0");
	ip = ipaddress;
	blocking = false;
	offline_to_user = false;
	notify = true;
	anonymous = false;
}

QString UserListElement::group() const
{
	return Group;
}

void UserListElement::setGroup(const QString& group)
{
	if (group == QT_TR_NOOP("All"))
		Group="";
	else
		Group=group;
	if (Parent)
		emit Parent->modified();
}

UserList::UserList(const UserList& source)
{
	for(const_iterator i=source.begin(); i!=source.end(); i++)
		append(*i);
}

UserList::UserList() : QObject(), QValueList<UserListElement>()
{
	dnslookups.setAutoDelete(true);
}

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
	/*
		mast3r: jak dobrze pamietam to obojetnie co sie wpisalo to sie wywalalo przy zamykaniu
			winna bylo cos w kdebug
	*/
}

void UserList::addDnsLookup(UinType  uin, const QHostAddress &ip) {
	kdebugf();
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
	//UserListElement &ule = byUin(uin);
	dnshandler = new DnsHandler(uin);
	dnslookups.append(dnshandler);
	kdebugf2();
}

void UserList::setDnsName(UinType  uin, const QString &name) {
	if (!containsUin(uin))
		return;
	UserListElement &ule = byUin(uin);
	if (ule.dnsname != name) {
		ule.dnsname = name;
		kdebug_mask(KADU_DEBUG_INFO, "UserList::setDnsName(): dnsname for uin %d: %s\n", uin, name.latin1());
		emit dnsNameReady(uin);
	}
}

UserListElement& UserList::byUin(UinType  uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return (*i);
	kdebug_mask(KADU_DEBUG_PANIC, "UserList::byUin(): Panic!\n");
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byNick(const QString& nickname)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).nickname.lower() == nickname.lower())
			return (*i);
	kdebug_mask(KADU_DEBUG_PANIC, "UserList::byNick(): Panic! %s not exists\n",
		(const char*)nickname.lower().local8Bit());
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byAltNick(const QString& altnick)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).altnick.lower() == altnick.lower())
			return (*i);
	kdebug_mask(KADU_DEBUG_PANIC, "UserList::byAltNick(): Panic! %s not exists\n",
		(const char*)altnick.lower().local8Bit());
	return *((UserListElement*)NULL);
}

//Zwraca elementy userlisty, jezeli nie mamy danego
//uin na liscie, zwracany jest UserListElement tylko z uin i altnick == uin
UserListElement UserList::byUinValue(UinType  uin)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).uin == uin)
			return (*i);
	UserListElement ule;
	ule.uin = uin;
	ule.altnick = QString::number(uin);
	ule.anonymous = true;
	return ule;
}

bool UserList::containsUin(UinType  uin) const {
	for (const_iterator i = begin(); i != end(); i++)
		if ((*i).uin == uin)
			return true;
	kdebug_mask(KADU_DEBUG_INFO, "UserList::containsUin(): userlist doesn't contain %d\n", uin);
	return false;
}

bool UserList::containsAltNick(const QString &altnick) const {
	for (const_iterator i = begin(); i != end(); i++)
		if ((*i).altnick.lower() == altnick.lower())
			return true;
	kdebug_mask(KADU_DEBUG_INFO, "UserList::containsAltNick(): userlist doesn't contain %s\n",
		(const char *)altnick.lower().local8Bit());
	return false;
}

void UserList::addUser(UserListElement& ule)
{
	kdebugf();
	UserListElement e(this);
	e.first_name = ule.first_name;
	e.last_name = ule.last_name;
	e.nickname = ule.nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.uin = ule.uin;
	e.status = ule.status;
	e.image_size = ule.image_size;
	e.blocking = ule.blocking;
	e.offline_to_user = ule.offline_to_user;
	e.notify = ule.notify;
	if (e.group() != tr("All"))
		e.setGroup(ule.Group);
	else
		e.setGroup("");
	e.description = ule.description;
	e.email = ule.email;
	e.anonymous = ule.anonymous;
	e.ip = ule.ip;
	e.port = ule.port;
	append(e);
	emit userAdded(e);
	emit modified();
	kdebugf2();
}

void UserList::addAnonymous(UinType uin)
{
	kdebugf();
	QString tmp = QString::number(uin);
	UserListElement e;
	e.first_name = "";
	e.last_name = "";
	e.nickname = tmp;
	e.altnick = tmp;
	e.mobile = "";
	e.uin = uin;
	e.setGroup("");
	e.description = "";
	e.email = "";
	e.anonymous = true;
	addUser(e);
	kdebugf2();
}

void UserList::changeUserInfo(const QString oldaltnick, UserListElement &ule)
{
	kdebugf();
	UserListElement &e = byAltNick(oldaltnick);

	e.first_name = ule.first_name;
	e.last_name = ule.last_name;
	e.nickname = ule.nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.email = ule.email;
	e.uin = ule.uin;
//	bool wasAnonymous = e.anonymous;
	e.anonymous = false;
	e.status = ule.status;
	e.image_size = ule.image_size;
	e.blocking = ule.blocking;
	e.offline_to_user = ule.offline_to_user;
	e.notify = ule.notify;
	e.Group = ule.Group;

	UserBox::all_renameUser(oldaltnick, ule.altnick);
	UserBox::all_refresh();

	emit modified();
	kdebugf2();
}

void UserList::changeUserStatus(const UinType uin, const unsigned int status)
{
	kdebugf();
	UserListElement &e = byUin(uin);
	if (status != e.status) {
		emit changingStatus(uin, e.status, status);
		e.status = status;
//		UserBox::all_refresh();			
		emit statusModified(&e);
	}
	kdebugf2();
}

void UserList::removeUser(const QString &altnick)
{
	kdebugf();
	for (Iterator i = begin(); i != end(); i++)
		if((*i).altnick == altnick)
		{ 
			remove(i);
			emit modified();
			break;
		}
	kdebugf2();
}

bool UserList::writeToFile(QString filename)
{
	kdebugf();
	QString faname;
	QString tmp;

	tmp = ggPath("");
	mkdir(tmp.local8Bit(), 0700);

	if (!filename.length()) {
		filename = ggPath("userlist");
		}

	faname = ggPath("userattribs");

	kdebug_mask(KADU_DEBUG_INFO, "UserList::writeToFile(): %s\n", (const char *)filename.local8Bit());

	QFile f(filename);

	if (!f.open(IO_WriteOnly)) {
		kdebug_mask(KADU_DEBUG_ERROR, "UserList::writeToFile(): Error opening file :(\n");
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
		tmp = (*i).group();
		tmp.replace(QRegExp(","), ";");
		s.append(tmp);
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
		kdebug_mask(KADU_DEBUG_ERROR, "UserList::writeToFile(): Error opening file :(\n");
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
	
	kdebugf2();
	return true;
}

bool UserList::readFromFile()
{
	kdebugf();
	QString path;
	QValueList<QStringList> ualist;
	QStringList userattribs,groupnames;
	QString line;
	UserListElement e;
	int groups, i;
	bool ok;

	path = ggPath("userattribs");
	kdebug_mask(KADU_DEBUG_INFO, "UserList::readFromFile(): Opening userattribs file: %s\n",
		(const char *)path.local8Bit());
	QFile fa(path);
	if (!fa.open(IO_ReadOnly)) {
		kdebug_mask(KADU_DEBUG_ERROR, "UserList::readFromFile(): Error opening userattribs file\n");
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
	kdebug_mask(KADU_DEBUG_INFO, "UserList::readFromFile(): Opening userlist file: %s\n",
		(const char *)path.local8Bit());
	QFile f(path);
	if (!f.open(IO_ReadOnly)) {
		kdebug_mask(KADU_DEBUG_ERROR, "UserList::readFromFile(): Error opening userlist file");
		return false;
		}

	kdebug_mask(KADU_DEBUG_INFO, "UserList::readFromFile(): File opened successfuly\n");

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
			e.first_name = "";
			e.last_name = "";
			e.nickname = nickname;
			e.altnick = nickname;
			e.mobile = "";
			e.uin = uin.toUInt(&ok);
			if (!ok)
				e.uin = 0;
			e.setGroup("");
			e.description = "";
			e.email = "";
			e.blocking = false;
			e.offline_to_user = false;
			e.notify = true;
			addUser(e);
			}
		else {
			userattribs = QStringList::split(";", line, TRUE);
			kdebug_mask(KADU_DEBUG_INFO, "UserList::readFromFile(): userattribs = %d\n", userattribs.count());
			if (userattribs.count() >= 12)
				groups = userattribs.count() - 11;
			else
				groups = userattribs.count() - 7;
			e.first_name = userattribs[0];
			e.last_name = userattribs[1];
			e.nickname = userattribs[2];
			e.altnick = userattribs[3];
			e.mobile = userattribs[4];
			groupnames.clear();
			for (i = 0; i < groups; i++)
				groupnames.append(userattribs[5 + i]);
			e.setGroup(groupnames.join(","));
			e.uin = userattribs[5 + groups].toUInt(&ok);
			if (!ok)
				e.uin = 0;
			e.email = userattribs[6 + groups];

			if (e.altnick == "") {
				if (e.nickname == "")
					e.altnick = e.first_name;
				else
					e.altnick = e.nickname;
				}

			QValueList<QStringList>::Iterator i = ualist.begin();
			while ((*i)[0].toUInt() != e.uin && i != ualist.end())
				i++;
			if (i != ualist.end()) {
				e.blocking = ((*i)[1] == "true");
				e.offline_to_user = ((*i)[2] == "true");
				e.notify = ((*i)[3] == "true");
				}
			else {
				e.blocking = false;
				e.offline_to_user = false;
				e.notify = true;
				}

			e.description = "";
			addUser(e);
			}
		}

	f.close();
	emit modified();
	kdebugf2();
	return true;
}

UserList& UserList::operator=(const UserList& userlist)
{
	QValueList<UserListElement>::operator=(userlist);
	for (Iterator i = begin(); i != end(); i++)
		(*i).Parent = this;
	emit modified();
	return *this;
}

void UserList::merge(UserList &userlist) {
	kdebugf();
	UserListElement e(this);

	for (Iterator i = userlist.begin(); i != userlist.end(); i++) {
		Iterator j;
		if ((*i).uin) {
			j = begin();
			while (j != end() && (*j).uin != (*i).uin)
				j++;
			}
		else {
			j = begin();
			while (j != end() && (*j).mobile != (*i).mobile)
				j++;
			}
		if (j != end()) {
			(*j).first_name = (*i).first_name;
			(*j).last_name = (*i).last_name;
			(*j).nickname = (*i).nickname;
			(*j).altnick = (*i).altnick;
			if ((*i).uin)
				(*j).mobile = (*i).mobile;
			else
				(*j).uin = (*i).uin;
//			(*j).status = (*i).status;
//			(*j).image_size = (*i).image_size;
			(*j).setGroup((*i).group());
//			(*j).description = (*i).description;
			(*j).email = (*i).email;
			}
		else {
			e.first_name = (*i).first_name;
			e.last_name = (*i).last_name;
			e.nickname = (*i).nickname;
			e.altnick = (*i).altnick;
			e.mobile = (*i).mobile;
			e.uin = (*i).uin;
			e.status = (*i).status;
			e.image_size = (*i).image_size;
			e.blocking = (*i).blocking;
			e.offline_to_user = (*i).offline_to_user;
			e.notify = (*i).notify;
			e.setGroup((*i).group());
			e.description = (*i).description;
			e.email = (*i).email;
			e.anonymous = (*i).anonymous;
			e.port = (*i).port;
			append(e);
			}
		}
	emit modified();
	kdebugf2();
}

UserList userlist;

