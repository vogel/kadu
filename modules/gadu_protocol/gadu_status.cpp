/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <libgadu.h>

#include <QtGui/QPixmap>

#include "gadu_status.h"
#include "icons_manager.h"

#define GG_STATUS_INVISIBLE2 0x0009

GaduStatus::GaduStatus()
{
}

GaduStatus::~GaduStatus()
{
}

GaduStatus &GaduStatus::operator = (const UserStatus &copyMe)
{
	setStatus(copyMe);
	return *this;
}

QPixmap GaduStatus::pixmap(eUserStatus stat, bool hasDescription, bool mobile) const
{
	QString pixname = pixmapName(stat, hasDescription, mobile);
	return icons_manager->loadPixmap(pixname);
}

QString GaduStatus::pixmapName(eUserStatus stat, bool hasDescription, bool mobile) const
{
	QString add(hasDescription ? "WithDescription" : "");
	add.append(mobile ? (!hasDescription) ? "WithMobile" : "Mobile" : "");

	switch (stat)
	{
		case Online:
			return QString("Online").append(add);
		case Busy:
			return QString("Busy").append(add);
		case Invisible:
			return QString("Invisible").append(add);
		case Blocking:
			return QString("Blocking");
		default:
			return QString("Offline").append(add);
	}
}

int GaduStatus::toStatusNumber() const
{
	return toStatusNumber(Stat, !Description.isEmpty());
}

int GaduStatus::toStatusNumber(eUserStatus status, bool has_desc)
{
	int sn = 0;

	switch (status)
	{
		case Online:
			sn = has_desc ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;
			break;

		case Busy:
			sn = has_desc ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;
			break;

		case Invisible:
			sn = has_desc ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;
			break;

		case Blocking:
			sn = GG_STATUS_BLOCKED;
			break;

		case Offline:
		default:
			sn = has_desc ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
			break;
	}

	return sn;
}

void GaduStatus::fromStatusNumber(int statusNumber, const QString &description)
{
	Description.truncate(0);

	switch (statusNumber)
	{
		case GG_STATUS_AVAIL_DESCR:
			Description = description;
		case GG_STATUS_AVAIL:
			Stat = Online;
			break;

		case GG_STATUS_BUSY_DESCR:
			Description = description;
		case GG_STATUS_BUSY:
			Stat = Busy;
			break;

		case GG_STATUS_INVISIBLE_DESCR:
			Description = description;
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			Stat = Invisible;
			break;

		case GG_STATUS_BLOCKED:
			Stat = Blocking;
			break;

		case GG_STATUS_NOT_AVAIL_DESCR:
			Description = description;
		case GG_STATUS_NOT_AVAIL:
		default:
			Stat = Offline;
			break;
	}
}

UserStatus *GaduStatus::copy() const
{
	return new GaduStatus(*this);
}

QString GaduStatus::protocolName() const
{
	static const QString protoName("Gadu");
	return protoName;
}
