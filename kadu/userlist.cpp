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
#include <qregexp.h>

#include <sys/stat.h>

#include "debug.h"
#include "userbox.h"
#include "userlist.h"

DnsHandler::DnsHandler(const QHostAddress &address)
{
	kdebugf();

	if (address == QHostAddress())
		kdebugmf(KDEBUG_WARNING, "NULL ip address!\n");

	connect(&DnsResolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	DnsResolver.setRecordType(QDns::Ptr);
	DnsResolver.setLabel(address);
	++counter;

	kdebugmf(KDEBUG_FUNCTION_END, "counter = %d\n", counter);
}

DnsHandler::~DnsHandler()
{
	--counter;
}

void DnsHandler::resultsReady()
{
	kdebugf();

	if (DnsResolver.hostNames().count())
		emit result(DnsResolver.hostNames()[0]);
	else
		emit result(QString::null);

	deleteLater();
	kdebugf2();
}

int DnsHandler::counter = 0;

UserListElement::UserListElement(UserList* parent)
{
//	kdebugf();
	Parent = parent;
	Version = 0;
	MaxImageSize = 0;
	Port = 0;
	Blocking = false;
	OfflineTo = false;
	Notify = true;
	Anonymous = false;
	Uin = 0;
	AliveSound = GLOBAL;
	MessageSound = GLOBAL;
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
//	kdebugf2();
}

UserListElement::UserListElement(const UserListElement &copyMe)
{
//	kdebugf();
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
	*this = copyMe;
//	kdebugf2();
}

UserListElement::UserListElement()
{
//	kdebugf();
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
	AliveSound = GLOBAL;
	MessageSound = GLOBAL;
	// TODO: zuniwersalizowaæ
	Stat = new GaduStatus();
//	kdebugf2();
}

UserListElement::~UserListElement()
{
	delete Stat;
}

const QString &UserListElement::homePhone() const
{
	return HomePhone;
}

void UserListElement::setHomePhone(const QString &phone)
{
	if (phone == HomePhone)
		return;

	UserListElement old = *this;
	HomePhone = phone;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

const QString &UserListElement::aliveSound(NotifyType &type) const
{
	type = AliveSound;
	return OwnAliveSound;
}

void UserListElement::setAliveSound(NotifyType type, const QString &file)
{
	if (type == AliveSound && file == OwnAliveSound)
		return;

	UserListElement old = *this;
	AliveSound = type;
	if (type == OWN)
		OwnAliveSound = file;
	else
		OwnAliveSound = QString::null;

	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

const QString &UserListElement::messageSound(NotifyType &type) const
{
	type = MessageSound;
	return OwnMessageSound;
}

void UserListElement::setMessageSound(NotifyType type, const QString &file)
{
	if (type == MessageSound && file == OwnMessageSound)
		return;

	UserListElement old = *this;
	MessageSound = type;
	if (type == OWN)
		OwnMessageSound = file;
	else
		OwnMessageSound = QString::null;

	if (Parent)
		emit Parent->userDataChanged(&old, this);
}

const QString &UserListElement::group() const
{
	return Group;
}

void UserListElement::setGroup(const QString& group)
{
	//gdzie¶ tu siê sypie :|
	kdebugmf(KDEBUG_INFO, "group: '%s' Parent:%p\n", group.local8Bit().data(), Parent);
	UserListElement old = *this;
	Group = (group == QT_TR_NOOP("All")) ? QString() : group;
	if (Parent)
		emit Parent->userDataChanged(&old, this);
	kdebugf2();
}

const QString &UserListElement::firstName() const
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

const QString &UserListElement::lastName() const
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

const QString &UserListElement::nickName() const
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

const QString &UserListElement::altNick() const
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

const QString &UserListElement::mobile() const
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

const QString &UserListElement::email() const
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

const QString &UserListElement::dnsName() const
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
		connect(new DnsHandler(Ip), SIGNAL(result(const QString &)),
				this, SLOT(setDnsName(const QString &)));
}

void UserListElement::operator = (const UserListElement &copyMe)
{
//	kdebugf();

	Parent = copyMe.Parent;
	Version = copyMe.Version;
	MaxImageSize = copyMe.MaxImageSize;
	Port = copyMe.Port;
	Ip = copyMe.Ip;
	DnsName = copyMe.DnsName;
	Blocking = copyMe.Blocking;
	OfflineTo = copyMe.OfflineTo;
	Notify = copyMe.Notify;
	Anonymous = copyMe.Anonymous;

	kdebugm(KDEBUG_DUMP, "setting status (%p <- %p)\n", Stat, copyMe.Stat);
	Stat->setStatus(*(copyMe.Stat));

	Group = copyMe.Group;
	FirstName = copyMe.FirstName;
	LastName = copyMe.LastName;
	NickName = copyMe.NickName;
	AltNick = copyMe.AltNick;
	Mobile = copyMe.Mobile;
	Email = copyMe.Email;
	Uin = copyMe.Uin;

	AliveSound = copyMe.AliveSound;
	OwnAliveSound = copyMe.OwnAliveSound;
	MessageSound = copyMe.MessageSound;
	OwnMessageSound = copyMe.OwnMessageSound;
	HomePhone = copyMe.HomePhone;
	
//	kdebugf2();
}

UserList::UserList(const UserList &source) : QObject(NULL, "userlist"), QMap<QString,UserListElement>()
{
	CONST_FOREACH(i, source)
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

UserListElement& UserList::byUin(UinType uin)
{
	FOREACH(i, *this)
		if((*i).uin() == uin)
			return (*i);
	kdebugmf(KDEBUG_PANIC, "Panic! uin:%d not found\n", uin);
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byNick(const QString& nickname)
{
	FOREACH(i, *this)
		if ((*i).nickName().lower() == nickname.lower())
			return (*i);
	kdebugmf(KDEBUG_PANIC, "Panic! %s not exists\n",
		nickname.lower().local8Bit().data());
	return *((UserListElement*)NULL);
}

UserListElement& UserList::byAltNick(const QString& altnick)
{
	QString altnick_norm = altnick.lower();
	if(contains(altnick_norm))
		return (*this)[altnick_norm];
	kdebugmf(KDEBUG_PANIC, "Panic! %s not exists\n",
		altnick_norm.local8Bit().data());
	return *((UserListElement*)NULL);
}

UserListElement UserList::byUinValue(UinType uin)
{
	CONST_FOREACH(i, *this)
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
	CONST_FOREACH(i, *this)
		if ((*i).uin() == uin)
			return true;
	kdebugmf(KDEBUG_INFO, "userlist doesn't contain %d\n", uin);
	return false;
}

bool UserList::containsAltNick(const QString &altnick) const
{
	QString altnick_norm = altnick.lower();
	if(contains(altnick_norm))
		return true;
	kdebugmf(KDEBUG_INFO, "userlist doesn't contain %s\n",
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
	e.setHomePhone("");
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

	kdebugmf(KDEBUG_INFO, "%s\n", filename.local8Bit().data());

	QFile f(filename);
	if (!f.open(IO_WriteOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening file :(\n");
		return false;
	}
	QCString str = codec_latin2->fromUnicode(gadu->userListToString(*this));
	f.writeBlock(str, str.length());
	f.close();


	QFile fa(faname);
	if (!fa.open(IO_WriteOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening file :(\n");
		return false;
	}

	QString s;
	CONST_FOREACH(i, *this)
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
			str = codec_latin2->fromUnicode(s);
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
	QMap<UinType, QStringList> attrs;
	QStringList userattribs,groupnames;
	QString line;
	UserListElement e;

	path = ggPath("userattribs");
	kdebugmf(KDEBUG_INFO, "Opening userattribs file: %s\n",
		path.local8Bit().data());
	QFile fa(path);
	if (!fa.open(IO_ReadOnly))
		kdebugmf(KDEBUG_ERROR, "Error opening userattribs file\n");
	else
	{
		QTextStream s(&fa);
		while ((line = s.readLine()).length())
		{
			QStringList slist;
			slist = QStringList::split(';', line);
			if (slist.count() == 4)
				attrs[slist[0].toULong()] = slist;
		}
		fa.close();
	}

	path = ggPath("userlist");
	kdebugmf(KDEBUG_INFO, "Opening userlist file: %s\n",
		path.local8Bit().data());
	QFile f(path);
	if (!f.open(IO_ReadOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening userlist file");
		return false;
	}

	kdebugmf(KDEBUG_INFO, "File opened successfuly\n");

	QTextStream t(&f);
	t.setCodec(codec_latin2);
	gadu->streamToUserList(t, *this);

	FOREACH(user, *this)
	{
		UinType uin = user.data().uin();
		if (attrs.contains(uin))
		{
			user.data().setBlocking(attrs[uin][1]=="true");
			user.data().setOfflineTo(attrs[uin][2]=="true");
			user.data().setNotify(attrs[uin][3]=="true");
		}
		else
		{
			user.data().setBlocking(false);
			user.data().setOfflineTo(false);
			user.data().setNotify(false);
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
	CONST_FOREACH(i, *this)
		emit userDataChanged(&(i.data()), NULL);

	QMap<QString,UserListElement>::operator=(userlist);
	FOREACH(i, *this)
		(*i).Parent = this;

	CONST_FOREACH(i, *this)
		emit userDataChanged(NULL, &(i.data()));

	emit modified();
	kdebugf2();
	return *this;
}

void UserList::merge(const UserList &ulist)
{
	kdebugf();
	UserListElement e(this);
	UserStatus status;

	CONST_FOREACH(i, *this)
		emit userDataChanged(&(i.data()), NULL);

	CONST_FOREACH(i, ulist)
	{
		Iterator j = begin();
		if ((*i).uin())
			while (j != end() && (*j).uin() != (*i).uin())
				++j;
		else
			while (j != end() && (*j).mobile() != (*i).mobile())
				++j;

		if (j != end())
		{
			status = (*j).status();
			remove((*j).altNick().lower());
		}
		else
			status = UserStatus();

		e = *i;
		e.status() = status;
		e.Parent = this;//!!! bez tej linii niez³a jazda jest...
		insert(e.altNick().lower(), e);
	}

	emit allNewContacts(*this);

	CONST_FOREACH(i, *this)
		emit userDataChanged(NULL, &(i.data()), true);

	emit modified();
	kdebugf2();
}

UserList userlist;

