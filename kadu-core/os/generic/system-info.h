/*****************************************************************************
	System Info

	Copyright (c) 2007-2008 by Remko Tronçon
		      2008 by Nigmatullin Ruslan <euroelessar@gmail.com>
		      2009 by Kadu Team

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#pragma once

#include "exports.h"

#include <QtCore/QObject>

class KADUAPI SystemInfo : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SystemInfo(QObject *parent = nullptr);
	virtual ~SystemInfo();

	const QString & osFullName() const { return OsFullName; }
	const QString & osName() const { return OsName; }
	const QString & osVersion() const { return OsVersion; }

	int timezoneOffset() const { return TimezoneOffset; }
	const QString & timezone() const { return Timezone; }

	QString localHostName();

private:
	int TimezoneOffset;
	QString Timezone;
	QString OsFullName;
	QString OsName;
	QString OsVersion;

    #if defined(Q_OS_LINUX)
	QString lsbRelease(const QStringList  &args);
	QString linuxHeuristicDetect();
    #endif

};
