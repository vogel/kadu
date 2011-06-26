/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <QtGui/QMessageBox>

#include "icons/kadu-icon.h"

#include "exports.h"

class QBoxLayout;
class QLabel;

namespace MessageDialog
{
	KADUAPI void show(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
			  QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	KADUAPI int exec(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
			  QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	KADUAPI bool ask(const KaduIcon &icon, const QString &title, const QString &text,
			 QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}

#endif // MESSAGE_DIALOG_H
