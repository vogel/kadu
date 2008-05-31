/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_UPDATES_H
#define KADU_UPDATES_H

#include <QDateTime>
#include <QHttpResponseHeader>

#include "protocol.h"

class QHttp;

class Updates : public QObject
{
	Q_OBJECT

	static bool UpdateChecked;
	static Updates *instance;
	static QDateTime LastUpdateCheck;
		
	QString query;
	QHttp *httpClient;

	Updates(UinType uin);
	virtual ~Updates();
	static bool ifNewerVersion(const QString &newestversion);

private slots:
	void gotUpdatesInfo(const QHttpResponseHeader &responseHeader);
	void run();

public:
	static void initModule();
	static void closeModule();

};

#endif
