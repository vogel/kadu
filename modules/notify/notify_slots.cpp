/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qgrid.h>

#include "notify_slots.h"
#include "debug.h"
#include "notify.h"
#include "config_dialog.h"
#include "userlist.h"

void NotifySlots::onCreateConfigDialog()
{
	kdebugf();

	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
	{
		if ((*i).uin())
			if (!(*i).notify())
				e_availusers->insertItem((*i).altNick());
			else
				e_notifies->insertItem((*i).altNick());
	}

	e_availusers->sort();
	e_notifies->sort();
	e_availusers->setSelectionMode(QListBox::Extended);
	e_notifies->setSelectionMode(QListBox::Extended);

	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
		panebox->setEnabled(false);

	connect(b_notifyall, SIGNAL(toggled(bool)), this, SLOT(ifNotifyAll(bool)));

	for (QStringList::iterator it=disabledControls.begin(); it!=disabledControls.end(); ++it)
	{
		ConfigDialog::getCheckBox("Notify", " ", *it)->setDisabled(true);
		ConfigDialog::getCheckBox("Notify", " ", *it)->setDown(true);
	}

	kdebugf2();
}

void NotifySlots::onApplyConfigDialog()
{
	kdebugf();
	unsigned int i;
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	for (i = 0; i < e_notifies->count(); ++i)
		userlist.byAltNick(e_notifies->text(i)).setNotify(true);
	for (i = 0; i < e_availusers->count(); ++i)
		userlist.byAltNick(e_availusers->text(i)).setNotify(false);
	kdebugf2();
}

NotifySlots::NotifySlots()
{
	kdebugf();
	kdebugf2();
}

NotifySlots::~NotifySlots()
{
	kdebugf();
	kdebugf2();
}

void NotifySlots::registerDisabledControl(const QString &name)
{
	kdebugf();
	disabledControls.append(name);
	kdebugf2();
}

void NotifySlots::unregisterDisabledControl(const QString &name)
{
	kdebugf();
	disabledControls.remove(name);
	kdebugf2();
}

void NotifySlots::ifNotifyAll(bool toggled)
{
	ConfigDialog::getGrid("Notify","listboxy")->setEnabled(!toggled);
}

void NotifySlots::_Left2( QListBoxItem *item)
{
	_Left();
}

void NotifySlots::_Right2( QListBoxItem *item)
{
	_Right();
}

void NotifySlots::_Left(void)
{
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0; i<e_notifies->count(); ++i)
		if (e_notifies->isSelected(i))
			tomove+=e_notifies->text(i);

	for(i=0; i<tomove.size(); ++i)
	{
		e_availusers->insertItem(tomove[i]);
		e_notifies->removeItem(e_notifies->index(e_notifies->findItem(tomove[i])));
	}

	e_availusers->sort();
	kdebugf2();
}

void NotifySlots::_Right(void)
{
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0; i<e_availusers->count(); ++i)
		if (e_availusers->isSelected(i))
			tomove+=e_availusers->text(i);

	for(i=0; i<tomove.size(); ++i)
	{
		e_notifies->insertItem(tomove[i]);
		e_availusers->removeItem(e_availusers->index(e_availusers->findItem(tomove[i])));
	}

	e_notifies->sort();
	kdebugf2();
}
