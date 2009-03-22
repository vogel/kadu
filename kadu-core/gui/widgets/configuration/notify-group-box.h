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

#include <QtGui/QGroupBox>

class NotifyGroupBox : public QGroupBox
{
	Q_OBJECT

	QString Notificator;

private slots:
	void toggledSlot(bool toggled);

public:
	NotifyGroupBox(const QString &notificator, const QString &caption, QWidget *parent = 0);
	virtual ~NotifyGroupBox() {}

	QString notificator() { return Notificator; }

signals:
	void toggled(const QString &notificator, bool toggled);

};

#endif // NOTIFY_GROUP_BOX
