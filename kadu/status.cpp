/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status.h"
#include "gadu.h"
#include "debug.h"
#include <qobject.h>

Status::Status()
{
	Stat = Offline;
	Description = "";
	FriendsOnly = false;
}

Status::Status(const Status &copyMe)
{
	Stat = copyMe.Stat;
	Description = copyMe.Description;
	FriendsOnly = copyMe.FriendsOnly;
}

Status::~Status()
{
}

void Status::operator = (const Status &copyMe)
{
	Stat = copyMe.Stat;
	Description = copyMe.Description;
	FriendsOnly = copyMe.FriendsOnly;
}

bool Status::operator == (const Status &compare) const
{
	return (Stat == compare.Stat) && (Description == compare.Description);
}

bool Status::operator != (const Status &compare) const
{
	return (Stat != compare.Stat) || (Description != compare.Description);
}

QPixmap Status::pixmap(bool mobile) const
{
	return pixmap(Stat, hasDescription(), mobile);
}

QPixmap Status::pixmap(const Status &stat, bool mobile) const
{
	return pixmap(stat.status(), stat.hasDescription(), mobile);
}

QPixmap Status::pixmap(eStatus stat, bool desc, bool mobile) const
{
	static QPixmap result;
	return result;
}

eStatus Status::status() const
{
	return Stat;
}

bool Status::isOnline() const
{
	return Stat == Online;
}

bool Status::isBusy() const
{
	return Stat == Busy;
}

bool Status::isInvisible() const
{
	return Stat == Invisible;
}

bool Status::isOffline() const
{
	return Stat == Offline;
}

bool Status::isBlocking() const
{
	return Stat == Blocking;
}

bool Status::isOffline(int index)
{
	return (index == 6) || (index == 7);
}

bool Status::hasDescription() const
{
	return !Description.isEmpty();
}

bool Status::isFriendsOnly() const
{
	return FriendsOnly;
}

QString Status::description() const
{
	return Description;
}

int Status::index(eStatus stat, bool desc)
{
	return (static_cast<int>(stat) << 1) + (desc ? 1 : 0);
}

int Status::index() const
{
	return (static_cast<int>(Stat) << 1) + (Description.isEmpty() ? 0 : 1);
}

void Status::setOnline(const QString& desc)
{
	if (Stat == Online && Description == desc && !Changed)
		return;

	Stat = Online;
	Description = desc;
	Changed = false;

	emit goOnline(Description);
	emit changed(*this);
}

void Status::setBusy(const QString& desc)
{
	if (Stat == Busy && Description == desc && !Changed)
		return;

	Stat = Busy;
	Description = desc;
	Changed = false;

	emit goBusy(Description);
	emit changed(*this);
}

void Status::setInvisible(const QString& desc)
{
	if (Stat == Invisible && Description == desc && !Changed)
		return;

	Stat = Invisible;
	Description = desc;
	Changed = false;

	emit goInvisible(Description);
	emit changed(*this);
}

void Status::setOffline(const QString& desc)
{
	if (Stat == Offline && Description == desc)
		return;

	Stat = Offline;
	Description = desc;
	Changed = false;

	emit goOffline(Description);
	emit changed(*this);
}

void Status::setBlocking()
{
	if (Stat == Blocking)
		return;

	Stat = Blocking;
	Description = "";
	Changed = false;

	emit goBlocking();
	emit changed(*this);
}

void Status::setDescription(const QString& desc)
{
	if (Description == desc)
		return;

	Description = desc;

	switch (Stat)
	{
		case Online:
			emit goOnline(Description);
			emit changed(*this);
			break;

		case Busy:
			emit goBusy(Description);
			emit changed(*this);
			break;

		case Invisible:
			emit goInvisible(Description);
			emit changed(*this);
			break;

		case Blocking:
			emit goBlocking();
			emit changed(*this);
			break;

		case Offline:
		default:
			emit goOffline(Description);
			emit changed(*this);
			break;
	}
}

void Status::setFriendsOnly(bool f)
{
	if (FriendsOnly == f)
		return;

	FriendsOnly = f;
	Changed = true;

	switch (Stat)
	{
		case Online: setOnline(Description); break;
		case Busy: setBusy(Description); break;
		case Invisible: setInvisible(Description); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			break;
	}
}

void Status::setStatus(const Status& stat)
{
	setFriendsOnly(stat.FriendsOnly);

	switch (stat.Stat)
	{
		case Online: setOnline(stat.Description); break;
		case Busy: setBusy(stat.Description); break;
		case Invisible: setInvisible(stat.Description); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			setOffline(stat.Description);
			break;
	}
}

void Status::setIndex(int index, const QString& desc)
{
	if (index % 2 == 0)
		setStatus(static_cast<eStatus>(index >> 1), "");
	else
		setStatus(static_cast<eStatus>(index >> 1), desc);
}

void Status::setStatus(eStatus stat, const QString& desc)
{
	switch (stat)
	{
		case Online: setOnline(desc); break;
		case Busy: setBusy(desc); break;
		case Invisible: setInvisible(desc); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			setOffline(desc); break;
	}
}

void Status::refresh()
{
	Changed = true;
	setStatus(*this);
}

eStatus Status::fromString(const QString& stat)
{
	if (stat.contains("Online"))
		return Online;
	if (stat.contains("Busy"))
		return Busy;
	if (stat.contains("Invisible"))
		return Invisible;
	if (stat.contains("Blocking"))
		return Blocking;
	return Offline;
}

QString Status::toString(eStatus stat, bool desc)
{
	QString res;
	switch (stat)
	{
		case Online: res.append("Online"); break;
		case Busy: res.append("Busy"); break;
		case Invisible: res.append("Invisible"); break;
		case Blocking: res.append("Blocking"); break;
		case Offline:
		default:
			res.append("Offline");
			break;
	}
	if (desc && stat!=Blocking)
		res.append("WithDescription");
	return res;
}

int Status::count()
{
	return 9;
}

int Status::initCount()
{
	return 7;
}

QString Status::name(int nr)
{
	static const char * names[] = {
		QT_TR_NOOP("Online"),
		QT_TR_NOOP("Online (d.)"),
		QT_TR_NOOP("Busy"),
		QT_TR_NOOP("Busy (d.)"),
		QT_TR_NOOP("Invisible"),
		QT_TR_NOOP("Invisible (d.)"),
		QT_TR_NOOP("Offline"),
		QT_TR_NOOP("Offline (d.)"),
		QT_TR_NOOP("Blocking")
	};

	return names[nr];
}

QString Status::name() const
{
	return name(index());
}

/* our own description container */
QStringList defaultdescriptions;
