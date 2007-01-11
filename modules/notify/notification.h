/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include "userlist.h"

#include "../notify/notification.h"

class QTimer;

class Notification : public QObject {
	Q_OBJECT

private:

	QString Type;
	UserListElements Ule;

	QString Title;
	QString Text;
	QString Icon;

	QValueList<QPair<QString, QString> > Callbacks;
	QTimer *DefaultCallbackTimer;

public:

	Notification(const QString &type, const QString &icon, const UserListElements &userListElements);
	virtual ~Notification();

	void close();

	void clearCallbacks();
	void addCallback(const QString &caption, const QString &slot);

	void setDefaultCallback(int timeout, const QString &slot);

	QString type();
	UserListElements userListElements();

	void setTitle(const QString &title);
	QString title();

	void setText(const QString &text);
	QString text();

	void setIcon(const QString &icon);
	QString icon();

	const QValueList<QPair<QString, QString> > & getCallbacks();

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();

	void clearDefaultCallback();

signals:
	void closed();

};

#endif // NOTIFICATION_H
