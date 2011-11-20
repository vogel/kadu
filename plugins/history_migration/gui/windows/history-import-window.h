/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_IMPORT_WINDOW_H
#define HISTORY_IMPORT_WINDOW_H

#include <QtGui/QDialog>

class QProgressBar;

class HistoryImportWindow : public QDialog
{
	Q_OBJECT

	QProgressBar *ChatsProgressBar;
	QProgressBar *MessagesProgressBar;

	void createGui();

public:
	explicit HistoryImportWindow(QWidget *parent = 0);
	virtual ~HistoryImportWindow();

	void setChatsCount(int count);
	void setChatsProgress(int progress);

	void setMessagesCount(int count);
	void setMessagesProgress(int progress);

};

#endif // HISTORY_IMPORT_WINDOW_H
