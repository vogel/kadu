/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "actions/actions.h"
#include "gui/menu/menu-inventory.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-injected-factory.h"
#include "debug.h"
#include "exports.h"

#include "infos.h"
#include "infos_dialog.h"
#include "show-infos-window-action.h"

Infos::Infos(QObject *parent) :
		QObject{parent},
		menuID{}
{
}

Infos::~Infos()
{
}

void Infos::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void Infos::setActions(Actions *actions)
{
	m_actions = actions;
}

void Infos::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void Infos::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void Infos::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void Infos::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void Infos::setShowInfosWindowAction(ShowInfosWindowAction *showInfosWindowAction)
{
	m_showInfosWindowAction = showInfosWindowAction;
}

void Infos::init()
{
	kdebugf();

	m_actions->insert(m_showInfosWindowAction);
	triggerAllAccountsRegistered(m_accountManager);

	fileName = m_pathsProvider->profilePath() + QStringLiteral("last_seen.data");

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
				foreach(Account account, m_accountManager->byProtocolName(protocol))
				{
					contact = m_contactManager->byId(account, uin, ActionReturnNull);
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
	m_menuInventory
		->menu("tools")
		->addAction(m_showInfosWindowAction, KaduMenu::SectionTools, 3)
		->update();

	kdebugf2();
}

void Infos::done()
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

	m_menuInventory
		->menu("tools")
		->removeAction(m_showInfosWindowAction)
		->update();
	m_actions->remove(m_showInfosWindowAction);

	kdebugf2();
}

void Infos::onShowInfos()
{
	kdebugf();
	updateTimes();
	InfosDialog *infosDialog = m_pluginInjectedFactory->makeInjected<InfosDialog>(lastSeen);
	infosDialog->show();
	kdebugf2();
}

void Infos::accountRegistered(Account account)
{
	kdebugf();

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	kdebugf2();
}

void Infos::accountUnregistered(Account account)
{
	kdebugf();

	disconnect(account, 0, this, 0);
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
		         = QDateTime::currentDateTime().toString(QStringLiteral("dd-MM-yyyy hh:mm"));
		//kdebugm(KDEBUG_INFO, "Last seen %s %s %s\n", qPrintable(contact.contactAccount().protocolName()), qPrintable(contact.id()), qPrintable(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")));
	}
	kdebugf2();
}

void Infos::updateTimes()
{
	kdebugf();
	foreach (const Contact &contact, m_contactManager->items())
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

#include "moc_infos.cpp"
