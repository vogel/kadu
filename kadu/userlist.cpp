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
#include "userbox.h"
#include "debug.h"

DnsHandler::DnsHandler(UserListElement &ule) : Ule(ule)
{
	kdebugf();

	if (ule.ip() == QHostAddress())
		kdebugm(KDEBUG_WARNING, "DnsHandler::DnsHandler(): NULL ip address!\n");

	connect(&DnsResolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	DnsResolver.setRecordType(QDns::Ptr);
	DnsResolver.setLabel(Ule.ip());
	++counter;

	kdebugm(KDEBUG_FUNCTION_END, "DnsHandler::DnsHandler(): counter = %d\n", counter);
}

DnsHandler::~DnsHandler()
{
	--counter;
}

void DnsHandler::resultsReady()
{
	kdebugf();

	if (DnsResolver.hostNames().count())
		Ule.setDnsName(DnsResolver.hostNames()[0]);
	else
		Ule.setDnsName(QString::null);

	deleteLater();
	kdebugf2();
}

int DnsHandler::counter = 0;

UserListElement::UserListElement(UserList* parent)
{
	kdebugf();
	Parent = parent;
	Version = 0;
	MaxImageSize = 0;
	Port = 0;
	Blocking = false;
	OfflineTo = false;
	Notify = true;
	Anonymous = false;
	Uin = 0;
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
	kdebugf2();
}

UserListElement::UserListElement(const UserListElement &copyMe)
{
	kdebugf();
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
	*this = copyMe;
	kdebugf2();
}

UserListElement::UserListElement()
{
	kdebugf();
	Parent = NULL;
	Version = 0;
	MaxImageSize = 0;
	Port = 0;
	Ip.setAddress("0.0.0.0");
	Blocking = false;
	OfflineTo = false;
	Notify = true;
	Anonymous = false;
	Uin = 0;
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
	kdebugf2();
}

UserListElement::~UserListElement()
{
	delete Stat;
}

QString UserListElement::group() const
{
	return Group;
}

void UserListElement::setGroup(const QString& group)
{
	//gdzie¶ tu siê sypie :|
	//ale jak na z³o¶æ po dodaniu debuga nie udaje siê powtórzyæ segfaulta
	kdebugf();
	kdebugm(KDEBUG_INFO, "group: '%s' Parent:%p\n", group.local8Bit().data(), Parent);
	UserListElement old = *this;
	Group = (group == QT_TR_NOOP("All")) ? "" : group;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
	kdebugf2();
}

QString UserListElement::firstName() const
{
	return FirstName;
}

void UserListElement::setFirstName(const QString &firstName)
{
	if (firstName == FirstName)
		return;

	UserListElement old = *this;
	FirstName = firstName;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

QString UserListElement::lastName() const
{
	return LastName;
}

void UserListElement::setLastName(const QString &lastName)
{
	if (lastName == LastName)
		return;

	UserListElement old = *this;
	LastName = lastName;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

QString UserListElement::nickName() const
{
	return NickName;
}

void UserListElement::setNickName(const QString &nickName)
{
	if (nickName == NickName)
		return;

	UserListElement old = *this;
	NickName = nickName;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

QString UserListElement::altNick() const
{
	return AltNick;
}

void UserListElement::setAltNick(const QString &altNick)
{
	if (altNick == AltNick)
		return;

	UserListElement old = *this;
	AltNick = altNick;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

QString UserListElement::mobile() const
{
	return Mobile;
}

void UserListElement::setMobile(const QString &mobile)
{
	if (mobile == Mobile)
		return;

	UserListElement old = *this;
	Mobile = mobile;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

QString UserListElement::email() const
{
	return Email;
}

void UserListElement::setEmail(const QString &email)
{
	if (email == Email)
		return;

	UserListElement old = *this;
	Email = email;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

UinType UserListElement::uin() const
{
	return Uin;
}

void UserListElement::setUin(const UinType &uin)
{
	if (uin == Uin)
		return;

	UserListElement old = *this;
	Uin = uin;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

const UserStatus & UserListElement::status() const
{
	return *Stat;
}

UserStatus & UserListElement::status()
{
	return *Stat;
}

int UserListElement::maxImageSize() const
{
	return MaxImageSize;
}

void UserListElement::setMaxImageSize(const int maxImageSize)
{
	if (maxImageSize == MaxImageSize)
		return;

	UserListElement old = *this;
	MaxImageSize = maxImageSize;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

bool UserListElement::isAnonymous() const
{
	return Anonymous;
}

void UserListElement::setAnonymous(const bool anonymous)
{
	if (anonymous == Anonymous)
		return;

	UserListElement old = *this;
	Anonymous = anonymous;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

const QHostAddress & UserListElement::ip() const
{
	return Ip;
}

QHostAddress & UserListElement::ip()
{
	return Ip;
}

QString UserListElement::dnsName() const
{
	return DnsName;
}

void UserListElement::setDnsName(const QString &dnsName)
{
	if (dnsName == DnsName)
		return;

	UserListElement old = *this;
	DnsName = dnsName;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

short UserListElement::port() const
{
	return Port;
}

void UserListElement::setPort(const short port)
{
	if (port == Port)
		return;

	UserListElement old = *this;
	Port = port;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

int UserListElement::version() const
{
	return Version;
}

void UserListElement::setVersion(const int version)
{
	if (Version == version)
		return;

	UserListElement old = *this;
	Version = version;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

bool UserListElement::blocking() const
{
	return Blocking;
}

void UserListElement::setBlocking(const bool blocking)
{
	if (blocking == Blocking)
		return;

	UserListElement old = *this;
	Blocking = blocking;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

bool UserListElement::offlineTo() const
{
	return OfflineTo;
}

void UserListElement::setOfflineTo(const bool offlineTo)
{
	if (offlineTo == OfflineTo)
		return;

	UserListElement old = *this;
	OfflineTo = offlineTo;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

bool UserListElement::notify() const
{
	return Notify;
}

void UserListElement::setNotify(const bool notify)
{
	if (notify == Notify)
		return;

	UserListElement old = *this;
	Notify = notify;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

void UserListElement::refreshDnsName()
{
	if (!(Ip == QHostAddress()))
		new DnsHandler(*this);
}

void UserListElement::operator = (const UserListElement &copyMe)
{
	kdebugf();

	Parent = copyMe.Parent;
	Version = copyMe.Version;
	MaxImageSize = copyMe.MaxImageSize;
	Port = copyMe.Port;
	Ip = copyMe.Ip;
	Blocking = copyMe.Blocking;
	OfflineTo = copyMe.OfflineTo;
	Notify = copyMe.Notify;
	Anonymous = copyMe.Anonymous;

	kdebugm(KDEBUG_DUMP, "setting status (%p <- %p)\n", Stat, copyMe.Stat);
	Stat->setStatus(*(copyMe.Stat));
	kdebugm(KDEBUG_DUMP, "done\n");

	Group = copyMe.Group;
	FirstName = copyMe.FirstName;
	LastName = copyMe.LastName;
	NickName = copyMe.NickName;
	AltNick = copyMe.AltNick;
	Mobile = copyMe.Mobile;
	Email = copyMe.Email;
	Uin = copyMe.Uin;
	kdebugf2();
}

UserList::UserList(const UserList &source) : QObject(NULL, "userlist"), QMap<QString,UserListElement>()
{
	for(const_iterator i = source.begin(); i != source.end(); ++i)
	{
		insert(i.key(),i.data());
		emit userDataChanged(NULL, &(i.data()));
	}
}

UserList::UserList() : QObject(NULL, "userlist"), QMap<QString,UserListElement>()
{
}

UserList::~UserList()
{
	/* Ciekawa zagadka... ten kdebug przy zamykaniu powodowa³ naruszenie
		pamiêci obojêtnie, co mia³ w parametrach, debugowanie tego nie jest
		konieczne wiêc zakomentowa³em.
		kdebugm(KDEBUG_INFO, "UserList::~UserList(): dnslookups.count() = %d\n", dnslookups.count());
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
	/*
		joi: obiekt userlist klasy UserList jest statyczny, wiêc jego destruktor
		jest wywo³ywany po wyj¶ciu z main(), a kdebug u¿ywa przecie¿ QMutexów,
		które korzystaj± z QApplication, a jego obiektu ju¿ nie ma...
	*/
}

UserListElement& UserList::byUin(UinType  uin)
{
	for(iterator i=begin(); i!=end(); ++i)
		if((*i).uin() == uin)
			return (*i);
	kdebugm(KDEBUG_PANIC, "UserList::byUin(): Panic!\n");
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byNick(const QString& nickname)
{
	for (iterator i = begin(); i != end(); ++i)
		if ((*i).nickName().lower() == nickname.lower())
			return (*i);
	kdebugm(KDEBUG_PANIC, "UserList::byNick(): Panic! %s not exists\n",
		nickname.lower().local8Bit().data());
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byAltNick(const QString& altnick)
{
	QString altnick_norm = altnick.lower();
	if(contains(altnick_norm))
		return (*this)[altnick_norm];
	kdebugm(KDEBUG_PANIC, "UserList::byAltNick(): Panic! %s not exists\n",
		altnick_norm.local8Bit().data());
	return *((UserListElement*)NULL);
}

UserListElement UserList::byUinValue(UinType uin)
{
	for (iterator i = begin(); i != end(); ++i)
		if ((*i).uin() == uin)
			return (*i);
	UserListElement ule;
	ule.setUin(uin);
	ule.setAltNick(QString::number(uin));
	ule.setAnonymous(true);
	return ule;
}

bool UserList::containsUin(UinType uin) const
{
	for (const_iterator i = begin(); i != end(); ++i)
		if ((*i).uin() == uin)
			return true;
	kdebugm(KDEBUG_INFO, "UserList::containsUin(): userlist doesn't contain %d\n", uin);
	return false;
}

bool UserList::containsAltNick(const QString &altnick) const
{
	QString altnick_norm = altnick.lower();
	if(contains(altnick_norm))
		return true;
	kdebugm(KDEBUG_INFO, "UserList::containsAltNick(): userlist doesn't contain %s\n",
		altnick_norm.local8Bit().data());
	return false;
}

void UserList::addUser(const UserListElement& ule)
{
	kdebugf();
	UserListElement e(this);
	e = ule;

	if (e.group() != tr("All"))
		e.setGroup(ule.Group);
	else
		e.setGroup("");

	insert(e.altNick().lower(), e);

	emit userDataChanged(NULL, &e);
	emit modified();
	kdebugf2();
}

void UserList::addAnonymous(UinType uin)
{
	kdebugf();
	QString tmp = QString::number(uin);
	UserListElement e;
	e.setFirstName("");
	e.setLastName("");
	e.setNickName(tmp);
	e.setAltNick(tmp);
	e.setMobile("");
	e.setUin(uin);
	e.setGroup("");
	e.setEmail("");
	e.setAnonymous(true);
	addUser(e);
	kdebugf2();
}

void UserList::removeUser(const QString &altnick)
{
	kdebugf();
	QString altnick_norm = altnick.lower();
	Iterator elem=find(altnick_norm);
	if (elem!=end())
	{
		emit userDataChanged(&(*elem), NULL);
		remove(elem);
		emit modified();
	}
	
	kdebugf2();
}

void UserList::changeUserInfo(const QString& old_altnick, const UserListElement& new_data)
{
	kdebugf();
	UserListElement e = byAltNick(old_altnick);
	remove(old_altnick.lower());

	UserListElement old_data = e;
	e = new_data;
	insert(e.altNick().lower(), e);

	UserBox::all_renameUser(old_altnick, e.altNick());
	UserBox::all_refresh();

	emit userDataChanged(&old_data, &new_data);
	emit modified();
	kdebugf2();
}

bool UserList::writeToFile(QString filename)
{
	kdebugf();
	QString faname;
	QString tmp;

	tmp = ggPath("");
	mkdir(tmp.local8Bit().data(), 0700);

	if (!filename.length())
		filename = ggPath("userlist");

	faname = ggPath("userattribs");

	kdebugm(KDEBUG_INFO, "UserList::writeToFile(): %s\n", filename.local8Bit().data());

	QFile f(filename);

	if (!f.open(IO_WriteOnly))
	{
		kdebugm(KDEBUG_ERROR, "UserList::writeToFile(): Error opening file :(\n");
		return false;
	}

	QString s;
	QCString str;
	for (Iterator i = begin(); i != end(); ++i)
	{
		s.truncate(0);
		s.append((*i).firstName())
			.append(QString(";"))
			.append((*i).lastName())
			.append(QString(";"))
			.append((*i).nickName())
			.append(QString(";"))
			.append((*i).altNick())
			.append(QString(";"))
			.append((*i).mobile())
			.append(QString(";"));
		tmp = (*i).group();
		tmp.replace(QRegExp(","), ";");
		s.append(tmp)
			.append(QString(";"));
		if ((*i).uin())
			s.append(QString::number((*i).uin()));
		s.append(QString(";"))
			.append((*i).email())
			.append(QString("\r\n"));

		if (!(*i).isAnonymous())
		{
			kdebugm(KDEBUG_INFO, "%s", s.local8Bit().data());
			str = QTextCodec::codecForName("ISO 8859-2")->fromUnicode(s);
			f.writeBlock(str, str.length());
		}
	}
	f.close();

	QFile fa(faname);

	if (!fa.open(IO_WriteOnly))
	{
		kdebugm(KDEBUG_ERROR, "UserList::writeToFile(): Error opening file :(\n");
		return false;
	}

	for (Iterator i = begin(); i != end(); ++i)
	{
		s.truncate(0);
		s.append(QString::number((*i).uin()))
			.append(QString(";"))
			.append((*i).blocking() ? QString("true") : QString("false"))
			.append(QString(";"))
			.append((*i).offlineTo() ? QString("true") : QString("false"))
			.append(QString(";"))
			.append((*i).notify() ? QString("true") : QString("false"))
			.append(QString("\r\n"));

		if (!(*i).isAnonymous() && (*i).uin())
		{
			kdebugm(KDEBUG_INFO, "%s", s.local8Bit().data());
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
	kdebugm(KDEBUG_INFO, "UserList::readFromFile(): Opening userattribs file: %s\n",
		path.local8Bit().data());
	QFile fa(path);
	if (!fa.open(IO_ReadOnly))
		kdebugm(KDEBUG_ERROR, "UserList::readFromFile(): Error opening userattribs file\n");
	else
	{
		QTextStream s(&fa);
		while ((line = s.readLine()).length())
		{
			QStringList slist;
			slist = QStringList::split(';', line);
			if (slist.count() == 4)
				ualist.append(slist);
		}
		fa.close();
	}

	path = ggPath("userlist");
	kdebugm(KDEBUG_INFO, "UserList::readFromFile(): Opening userlist file: %s\n",
		path.local8Bit().data());
	QFile f(path);
	if (!f.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "UserList::readFromFile(): Error opening userlist file");
		return false;
	}

	kdebugm(KDEBUG_INFO, "UserList::readFromFile(): File opened successfuly\n");

	clear();

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	while ((line = t.readLine()).length())
	{
		if (line[0] == '#')
			continue;

		if (line.find(';') < 0)
		{
			QString nickname = line.section(' ',0,0);
			QString uin = line.section(' ',1,1);
			if (uin == "")
				continue;
			e.setFirstName("");
			e.setLastName("");
			e.setNickName(nickname);
			e.setAltNick(nickname);
			e.setMobile("");
			e.setUin(uin.toUInt(&ok));
			if (!ok)
				e.setUin(0);
			e.setGroup("");
			e.setEmail("");
			e.setBlocking(false);
			e.setOfflineTo(false);
			e.setNotify(true);
			e.status().setOffline();
			addUser(e);
		}
		else
		{
			userattribs = QStringList::split(";", line, true);
			kdebugm(KDEBUG_INFO, "UserList::readFromFile(): userattribs = %d\n", userattribs.count());
			if (userattribs.count() >= 12)
				groups = userattribs.count() - 11;
			else
				groups = userattribs.count() - 7;
			e.setFirstName(userattribs[0]);
			e.setLastName(userattribs[1]);
			e.setNickName(userattribs[2]);
			e.setAltNick(userattribs[3]);
			e.setMobile(userattribs[4]);
			groupnames.clear();
			for (i = 0; i < groups; ++i)
				groupnames.append(userattribs[5 + i]);
			e.setGroup(groupnames.join(","));
			e.setUin(userattribs[5 + groups].toUInt(&ok));
			if (!ok)
				e.setUin(0);
			e.setEmail(userattribs[6 + groups]);

			if (e.altNick() == "")
				if (e.nickName() == "")
					e.setAltNick(e.firstName());
				else
					e.setAltNick(e.nickName());

			QValueList<QStringList>::Iterator i = ualist.begin();
			while ((*i)[0].toUInt() != e.uin() && i != ualist.end())
				++i;
			if (i != ualist.end())
			{
				e.setBlocking(((*i)[1] == "true"));
				e.setOfflineTo(((*i)[2] == "true"));
				e.setNotify(((*i)[3] == "true"));
			}
			else
			{
				e.setBlocking(false);
				e.setOfflineTo(false);
				e.setNotify(false);
			}

			e.status().setOffline();
			addUser(e);
		}
	}

	f.close();
	emit modified();
	kdebugf2();
	return true;
}

// co z tym zrobiæ ??
UserList& UserList::operator=(const UserList& userlist)
{
	kdebugf();
	for (const_iterator i = begin(); i != end(); ++i)
		emit userDataChanged(&(i.data()), NULL);

	QMap<QString,UserListElement>::operator=(userlist);
	for (Iterator i = begin(); i != end(); ++i)
		(*i).Parent = this;

	for (const_iterator i = begin(); i != end(); ++i)
		emit userDataChanged(NULL, &(i.data()));

	emit modified();
	kdebugf2();
	return *this;
}

void UserList::merge(const UserList &userlist)
{
	kdebugf();
	UserListElement e(this);

	for (const_iterator i = begin(); i != end(); ++i)
		emit userDataChanged(&(i.data()), NULL);

	for (ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
	{
		Iterator j=begin();
		if ((*i).uin())
			while (j != end() && (*j).uin() != (*i).uin())
				++j;
		else
			while (j != end() && (*j).mobile() != (*i).mobile())
				++j;

		if (j != end())
			remove((*j).altNick().lower());

		e = *i; // to jest na pewno potrzebne ?? //j:wydaje siê ¿e tak, ¿eby pole Parent dobrze wskazywa³o
		insert(e.altNick().lower(), e);
	}

	for (const_iterator i = begin(); i != end(); ++i)
		emit userDataChanged(NULL, &(i.data()));

	emit modified();
	kdebugf2();
}

UserList userlist;

