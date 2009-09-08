/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <QMenu>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "kadu.h"
#include "misc.h"
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


Infos::Infos(QObject *parent, const char *name)
: QObject(parent, name)
{
	kdebugf();
	fileName = ggPath("last_seen.data");
	
	if(QFile::exists(fileName))
	{
		QFile dataFile(fileName);
		if(dataFile.open(IO_ReadOnly))
		{
			while(!dataFile.atEnd())
			{
				QTextStream dataStream(&dataFile);
				QString uin = dataStream.readLine();
				QString dateTime = dataStream.readLine();
				if(!userlist->byID("Gadu", uin).isAnonymous())
					lastSeen[uin] = dateTime;
				uin = dataStream.readLine();
			}
			dataFile.close();
		}
	}

	// Main menu entry
	lastSeenActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "lastSeenAction",
		this, SLOT(onShowInfos()),
		"LastSeen", tr("&Show infos about contacts...")
	);
	kadu->insertMenuActionDescription(0, lastSeenActionDescription);
	
	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool) ),
		this, SLOT( onUserStatusChangedSlot(QString, UserListElement, QString, QVariant, QVariant, bool, bool) ));
}

Infos::~Infos()
{
	kdebugf();

	updateTimes();
	QFile dataFile(fileName);
	if(dataFile.open(IO_WriteOnly))
	{
		QTextStream dataStream(&dataFile);
		for(LastSeen::ConstIterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
		{
			dataStream << it.key() << "\n" << it.data() << "\n\n";
		}
	}
	dataFile.close();

	kadu->removeMenuActionDescription(lastSeenActionDescription);
	delete lastSeenActionDescription;

	kdebugf2();
}

void Infos::onShowInfos()
{
	kdebugf();
	updateTimes();
	InfosDialog *infosDialog = new InfosDialog(lastSeen, NULL, "infos dialog", false, Qt::WDestructiveClose);
	infosDialog->show();
	kdebugf2();
}

void Infos::onUserStatusChangedSlot(QString protocolName, UserListElement elem, QString name, QVariant oldValue,
		QVariant currentValue, bool massively, bool last)
{
	kdebugf();
	if (protocolName.compare("Gadu") == 0)
		if(elem.status(protocolName).isAvailable())
			lastSeen[elem.ID(protocolName)] = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
	kdebugf2();
}

void Infos::updateTimes()
{
	kdebugf();
	for(LastSeen::Iterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
		if (userlist->byID("Gadu", it.key()).status("Gadu").isAvailable())
		{
			kdebugm(KDEBUG_INFO, "Updating %s's time\n", it.key().latin1());
			kdebugm(KDEBUG_INFO, "Previous one: %s\n", it.data().latin1());
			kdebugm(KDEBUG_INFO, "New one: %s\n\n", QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm").latin1());
			it.data() = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
		}
	kdebugf2();
}

