/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2008, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef INFOS_DIALOG_H
#define INFOS_DIALOG_H

#include <QtWidgets/QDialog>

typedef QMap<QPair<QString, QString>, QString> LastSeen;

class QTreeWidget;

/*!
 * Creates a QListView within itself. It also updates the "last seen" times.
 * \brief Dialog window for "last seen", IP, DNS.
 */
class InfosDialog : public QDialog
{
	Q_OBJECT

	QTreeWidget *ListView;

private slots:
	void customContextMenuRequested(const QPoint &point);

public:
	/*! Default constructor. */
	explicit InfosDialog(const LastSeen &lastSeen, QWidget *parent = 0);

	/*! Default destructor. */
	~InfosDialog();
};

#endif // INFOS_DIALOG_H
