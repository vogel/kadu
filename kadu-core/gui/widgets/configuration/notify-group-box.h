/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFY_GROUP_BOX
#define NOTIFY_GROUP_BOX

#include <QtGui/QWidget>

#include "notify/notifier.h"

class QCheckBox;

class NotifyGroupBox : public QWidget
{
	Q_OBJECT

	QCheckBox *NotifierCheckBox;

	Notifier *Notificator;

private slots:
	void toggledSlot(bool toggled);

public:
	NotifyGroupBox(Notifier *Notificator, const QString &caption, QWidget *parent = 0);
	virtual ~NotifyGroupBox() {}

	Notifier * notificator() { return Notificator; }

	void setChecked(bool checked);
	void addWidget(QWidget *widget);

signals:
	void toggled(Notifier *notificator, bool toggled);

};

#endif // NOTIFY_GROUP_BOX
