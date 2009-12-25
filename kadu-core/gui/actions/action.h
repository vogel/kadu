/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTION_H
#define ACTION_H

#include <QtCore/QMultiMap>
#include <QtGui/QAction>
#include <QtGui/QIcon>

#include "configuration/configuration-aware-object.h"

#include "exports.h"

class ActionDescription;
class Buddy;
class BuddySet;
class Contact;
class ContactSet;
class MainWindow;

class KADUAPI Action : public QAction
{
	Q_OBJECT

	ActionDescription *Description;

	QString OnText;
	QString OffText;

	QIcon OnIcon;
	QIcon OffIcon;

private slots:
	void changedSlot();
	void hoveredSlot();
	void toggledSlot(bool checked);
	void triggeredSlot(bool checked);

public:
	Action(ActionDescription *description, MainWindow *parent);
	virtual ~Action();

	Contact contact();
	ContactSet contacts();
	Buddy buddy();
	BuddySet buddies();

public slots:
	void checkState();
	void updateIcon();

signals:
	void changed(QAction *action);
	void hovered(QAction *action);
	void toggled(QAction *action, bool checked);
	void triggered(QAction *action, bool checked = false);

};

void disableEmptyContacts(Action *action);

#endif // ACTION_H
