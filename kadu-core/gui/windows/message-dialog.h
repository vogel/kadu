/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_DIALOG_H
#define MESSAGE_DIALOG_H

#include <QtWidgets/QMessageBox>

#include "icons/kadu-icon.h"

#include "exports.h"

class KADUAPI MessageDialog : public QObject
{
	Q_OBJECT

	QMessageBox *Box;

	MessageDialog(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
				      QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

protected slots:
	void messageBoxFinished(int result);

public:
	static MessageDialog * create(const KaduIcon &icon, const QString &title, const QString &text,
				      QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	static void show(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
			  QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	MessageDialog * addButton(QMessageBox::StandardButton button, const QString &text = "");
	void setDefaultButton(QMessageBox::StandardButton button);

	int exec();
	bool ask();
};

#endif // MESSAGE_DIALOG_H
