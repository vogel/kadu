/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef FILE_TRANSFER_ACTIONS_H
#define FILE_TRANSFER_ACTIONS_H

#include <QtGui/QAction>

class Action;
class ActionDescription;
class ContactSet;

class FileTransferActions : public QObject
{
	Q_OBJECT

	ActionDescription *SendFileActionDescription;
	ActionDescription *FileTransferWindowActionDescription;

	QStringList selectFilesToSend();
	void selectFilesAndSend(const ContactSet &contacts);

private slots:
	void sendFileActionCreated(Action *action);
	void sendFileActionActivated(QAction *sender, bool toggled);
	void toggleFileTransferWindow(QAction *sender, bool toggled);

public:
	explicit FileTransferActions(QObject *parent = 0);
	virtual ~FileTransferActions();

};

#endif // FILE_TRANSFER_ACTIONS_H
