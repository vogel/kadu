/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 * This module bases on the filtering module created by Pinkworm and Vogel.
 * It also uses searchbox class code by Matteo Bertozzi:
 * http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/
 */

#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "userbox.h"
#include "userlist.h"

#include "filtering.h"
#include "searchbox_mac.h"

extern "C" int macosx_filtering_init(bool firstLoad)
{	
	filtering = new Filtering();
	return 0;
}

extern "C" void macosx_filtering_close()
{
	delete filtering;
	filtering = NULL;
}

Filtering::Filtering(): QWidget(kadu)
{
	kdebugf();

	setContentsMargins(0, 2, 0, 0);
	kadu->mainLayout()->insertWidget(0, this);
	
	filter = new UserGroup(userlist->toUserListElements());
	
	search = new QMacSearchBox(this);
	search->setMinimumHeight(24);
	connect(search, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));

	QHBoxLayout *l = new QHBoxLayout(this, 0, 1);
	l->addWidget(search);

	kadu->userbox()->installEventFilter(this);

	createDefaultConfiguration();
	configurationUpdated();
	
	kdebugf2();
}
	
Filtering::~Filtering()
{
	kdebugf();
	
	kadu->userbox()->removeFilter(filter);
	disconnect(search, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
	kadu->userbox()->removeEventFilter(this);

	kdebugf2();
}

void Filtering::textChanged (const QString& s)
{
	kdebugf();

	clearFilter();

	if (!s.isEmpty())
		filterWith(s);

	kdebugf2();
}

void Filtering::clearFilter()
{
	kdebugf();

	kadu->userbox()->removeFilter(filter);
	filter->removeUsers(userlist);

	kdebugf2();
}

void Filtering::filterWith(const QString& f)
{
	kdebugf();

	foreach(const UserListElement &u, userlist->toUserListElements()) {
		if (checkString(u.firstName(), f)
				|| checkString(u.lastName(), f)
				|| checkString(u.altNick(), f)
				|| checkString(u.nickName(), f)) {
			filter->addUser (u);
		}
	}

	kadu->userbox()->applyFilter(filter);

	kdebugf2();
}

bool Filtering::checkString (const QString &hay, const QString& needle)
{
	int index = hay.find (needle, 0, false);
	return (index >= 0);
}

void Filtering::configurationUpdated()
{
	setShown(true);
}

void Filtering::createDefaultConfiguration()
{
}

Filtering *filtering = 0;

