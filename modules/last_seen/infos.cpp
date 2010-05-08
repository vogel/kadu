/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <QMenu>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "misc/misc.h"
#include "debug.h"

#include "infos.h"
#include "infos_dialog.h"

Infos *lastSeen;

extern "C" KADU_EXPORT int last_seen_init()
{
	kdebugf();

	lastSeen = new Infos();

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void last_seen_close()
{
	kdebugf();

	delete lastSeen;
	lastSeen = NULL;

	kdebugf2();
}


Infos::Infos(QObject *parent)
: QObject(parent)
{
	kdebugf();
	fileName = profilePath("last_seen.data");

	QList<Account> allGaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	QList<Account>::iterator accountIt;

	if(QFile::exists(fileName))
	{
		QFile dataFile(fileName);
		if(dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			while(!dataFile.atEnd())
			{
				QTextStream dataStream(&dataFile);
				QString uin = dataStream.readLine();
				QString dateTime = dataStream.readLine();
				for (accountIt = allGaduAccounts.begin(); accountIt != allGaduAccounts.end(); ++accountIt) {
					Contact contact = ContactManager::instance()->byId((*accountIt), uin);
					if (contact.isNull())
						continue;
					if (!contact.ownerBuddy().isAnonymous())
						lastSeen[uin] = dateTime;
				}
				uin = dataStream.readLine();
			}
			dataFile.close();
		}
	}

	// Main menu entry
	lastSeenActionDescription = new ActionDescription(
		this, ActionDescription::TypeMainMenu, "lastSeenAction",
		this, SLOT(onShowInfos()),
		"", "", tr("&Show infos about contacts...")
	);
	Core::instance()->kaduWindow()->insertMenuActionDescription(lastSeenActionDescription, KaduWindow::MenuKadu, 0);
	
	for (accountIt = allGaduAccounts.begin(); accountIt	!= allGaduAccounts.end(); ++accountIt) {
		connect((*accountIt), SIGNAL(buddyStatusChanged(Contact, Status)),
				this, SLOT(contactStatusChanged(Contact, Status)));
	}
}

Infos::~Infos()
{
	kdebugf();

	updateTimes();
	QFile dataFile(fileName);
	if(dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream dataStream(&dataFile);
		for(LastSeen::ConstIterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
		{
			dataStream << it.key() << "\n" << it.value() << "\n\n";
		}
	}
	dataFile.close();

	Core::instance()->kaduWindow()->removeMenuActionDescription(lastSeenActionDescription);
	delete lastSeenActionDescription;

	kdebugf2();
}

void Infos::onShowInfos()
{
	kdebugf();
	updateTimes();
	InfosDialog *infosDialog = new InfosDialog(lastSeen);
	infosDialog->show();
	kdebugf2();
}

void Infos::contactStatusChanged(Contact contact, Status status)
{
	kdebugf();
	if (contact.contactAccount().protocolName().compare("Gadu") == 0)
		if(!status.isDisconnected())
			lastSeen[contact.id()] = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
	kdebugf2();
}

void Infos::updateTimes() {
	kdebugf();
	QList<Account> allGaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	QList<Account>::iterator accountIt;
	for (LastSeen::Iterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
		for (accountIt = allGaduAccounts.begin(); accountIt != allGaduAccounts.end(); ++accountIt) {
			Contact contact = ContactManager::instance()->byId((*accountIt), it.key());
			if (contact.isNull())
				continue;
			if (!contact.currentStatus().isDisconnected()) {
				kdebugm(KDEBUG_INFO, "Updating %s's time\n", qPrintable(it.key()));
				kdebugm(KDEBUG_INFO, "Previous one: %s\n", qPrintable(it.value()));
				kdebugm(KDEBUG_INFO, "New one: %s\n\n", qPrintable(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")));
				it.value() = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
			}
		}
	kdebugf2();
}

