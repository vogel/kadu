/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "misc/path-conversion.h"
#include "debug.h"
#include "exports.h"

#include "infos.h"
#include "infos_dialog.h"

Infos::Infos(QObject *parent) :
	QObject(parent)
{
	kdebugf();

	triggerAllAccountsRegistered();

	fileName = profilePath("last_seen.data");

	if (QFile::exists(fileName))
	{
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly))
		{
			kdebugm(KDEBUG_INFO, "file opened '%s'\n", qPrintable(file.fileName()));
			QTextStream stream(&file);
			while (!stream.atEnd())
			{
				QStringList fullId = stream.readLine().split(':', QString::SkipEmptyParts);
				if (fullId.count() != 2)
					continue;
				QString protocol = fullId[0];
				QString uin = fullId[1];
				QString dateTime = stream.readLine();
				//kdebugm(KDEBUG_INFO, "Last seen %s %s %s\n", qPrintable(protocol), qPrintable(uin), qPrintable(dateTime));

				Contact contact;
				// wstawiamy tylko konta, które są na liście kontaktów
				foreach(Account account, AccountManager::instance()->byProtocolName(protocol))
				{
					contact = ContactManager::instance()->byId(account, uin, ActionReturnNull);
					if (contact.isNull())
						continue;
					if (!contact.isAnonymous())
					{
						lastSeen[qMakePair(protocol, uin)] = dateTime;
						// wystarczy, że kontakt jest na jednym koncie, omijamy resztę
						continue;
					}
				}
				QString tmp = stream.readLine(); // skip empty line
			}
			file.close();
		}
		else
		{
			fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
			fflush(stderr);
		}
	}

	// Main menu entry
	lastSeenActionDescription = new ActionDescription(
		this, ActionDescription::TypeMainMenu, "lastSeenAction",
		this, SLOT(onShowInfos()),
		KaduIcon(), tr("&Show infos about buddies...")
	);
	Core::instance()->kaduWindow()->insertMenuActionDescription(lastSeenActionDescription, KaduWindow::MenuTools, 3);

	kdebugf2();
}

Infos::~Infos()
{
	kdebugf();

	updateTimes();
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", qPrintable(file.fileName()));
		QTextStream stream(&file);
		for (LastSeen::Iterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
		{
			QPair<QString, QString> lastSeenKey = it.key();
			//kdebugm(KDEBUG_INFO, "Last seen %s %s %s\n", qPrintable(lastSeenKey.first), qPrintable(lastSeenKey.second), qPrintable(it.value()));
			stream << lastSeenKey.first << ":" << lastSeenKey.second << "\n" << it.value() << "\n\n";
		}
		file.close();
	}
	else
	{
		fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
		fflush(stderr);
	}

	Core::instance()->kaduWindow()->removeMenuActionDescription(lastSeenActionDescription);

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

void Infos::accountRegistered(Account account)
{
	kdebugf();
	if (!account.protocolHandler())
	{
		kdebugf2();
		return;
	}

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	kdebugf2();
}

void Infos::accountUnregistered(Account account)
{
	kdebugf();
	if (!account.protocolHandler())
	{
		kdebugf2();
		return;
	}

	disconnect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	kdebugf2();
}

void Infos::contactStatusChanged(Contact contact, Status status)
{
	Q_UNUSED(status)
	kdebugf();
	// interesuje nas tylko zmiana na offline, lastSeen dla ludzi online
	// zostanie zapisany przy wyjściu z programu
	if (contact.currentStatus().isDisconnected())
	{
		lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())]
		         = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
		//kdebugm(KDEBUG_INFO, "Last seen %s %s %s\n", qPrintable(contact.contactAccount().protocolName()), qPrintable(contact.id()), qPrintable(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")));
	}
	kdebugf2();
}

void Infos::updateTimes()
{
	kdebugf();
	foreach (const Contact &contact, ContactManager::instance()->items())
	{
		if (!contact.currentStatus().isDisconnected())
		{
			kdebugm(KDEBUG_INFO, "Updating %s:%s time\n", qPrintable(contact.contactAccount().protocolName()), qPrintable(contact.id()));
			kdebugm(KDEBUG_INFO, "Previous one: %s\n", qPrintable(lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())]));
			kdebugm(KDEBUG_INFO, "New one: %s\n\n", qPrintable(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")));
			lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())]
			         = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
		}
	}
	kdebugf2();
}
