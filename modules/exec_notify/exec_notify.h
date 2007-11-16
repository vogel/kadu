/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXEC_NOTIFY_H
#define EXEC_NOTIFY_H

#include <qobject.h>
#include <qstringlist.h>

#include "../notify/notify.h"
#include "../notify/notification.h"

class QLineEdit;

class ExecConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotifyEvent;
	QMap<QString, QString> Commands;

	QLineEdit *commandLineEdit;	

public:
	ExecConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual ~ExecConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

class ExecNotify : public Notifier
{
	Q_OBJECT

	void import_0_5_0_configuration();
	void import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to);

public:
	ExecNotify(QObject *parent = 0, const char *name = 0);
	~ExecNotify();

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual void notify(Notification *notification);

	void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

public slots:
		void run(const QStringList &args, const QString &stdin);

};

extern ExecNotify *exec_notify;

#endif // EXEC_NOTIFY_H
