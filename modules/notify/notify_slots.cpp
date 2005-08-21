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
#include "misc.h"

void NotifySlots::onCreateConfigDialog()
{
	kdebugf();

	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	CONST_FOREACH(user, *userlist)
	{
		if ((*user).usesProtocol("Gadu") && !(*user).isAnonymous())
			if (!(*user).notify())
				e_availusers->insertItem((*user).altNick());
			else
				e_notifies->insertItem((*user).altNick());
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

	CONST_FOREACH(name, disabledControls)
	{
		ConfigDialog::getCheckBox("Notify", " ", *name)->setDisabled(true);
		ConfigDialog::getCheckBox("Notify", " ", *name)->setDown(true);
	}

	kdebugf2();
}

void NotifySlots::onApplyConfigDialog()
{
	kdebugf();
	unsigned int i;
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	unsigned int count = e_notifies->count();
	for (i = 0; i < count; ++i)
		userlist->byAltNick(e_notifies->text(i)).setNotify(true);
	count = e_availusers->count();
	for (i = 0; i < count; ++i)
		userlist->byAltNick(e_availusers->text(i)).setNotify(false);
	userlist->writeToConfig();
	kdebugf2();
}

NotifySlots::NotifySlots(QObject *parent, const char *name) : QObject(parent, name)
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

void NotifySlots::_Left2(QListBoxItem *)
{
	_Left();
}

void NotifySlots::_Right2(QListBoxItem *)
{
	_Right();
}

void NotifySlots::_Left(void)
{
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i, count = e_notifies->count();

	for (i = 0; i < count; ++i)
		if (e_notifies->isSelected(i))
			tomove += e_notifies->text(i);

	CONST_FOREACH(elem, tomove)
	{
		e_availusers->insertItem(*elem);
		e_notifies->removeItem(e_notifies->index(e_notifies->findItem(*elem)));
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
	unsigned int i, count = e_availusers->count();

	for(i = 0; i < count; ++i)
		if (e_availusers->isSelected(i))
			tomove += e_availusers->text(i);

	CONST_FOREACH(elem, tomove)
	{
		e_notifies->insertItem(*elem);
		e_availusers->removeItem(e_availusers->index(e_availusers->findItem(*elem)));
	}

	e_notifies->sort();
	kdebugf2();
}
