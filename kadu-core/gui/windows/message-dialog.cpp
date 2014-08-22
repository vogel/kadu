/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QPixmap>
#include <QtGui/QStyle>

#include "message-dialog.h"


MessageDialog * MessageDialog::create(const KaduIcon &icon, const QString &title, const QString &text, QWidget *parent, Qt::WindowFlags f)
{
	return new MessageDialog(icon, title, text, QMessageBox::NoButton, parent, f);
}

void MessageDialog::show(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags f)
{
	auto dialog = new MessageDialog(icon, title, text, buttons, parent, f);
	dialog->exec();
}

MessageDialog::MessageDialog(const KaduIcon &icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags f)
{
	Box = new QMessageBox(QMessageBox::NoIcon, title, text, buttons, parent, f);
	connect(Box, SIGNAL(finished(int)), this, SLOT(messageBoxFinished(int)));
	Box->setAttribute(Qt::WA_DeleteOnClose, true);

	int iconSize = Box->style()->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, Box);
	QPixmap pixmap(icon.icon().pixmap(iconSize, iconSize));
	if (!pixmap.isNull())
		Box->setIconPixmap(pixmap);
}

void MessageDialog::messageBoxFinished(int result)
{
	Q_UNUSED(result)

	deleteLater();
}

MessageDialog * MessageDialog::addButton(QMessageBox::StandardButton button, const QString &text)
{
	Box->addButton(button);
	if (!text.isEmpty())
		Box->setButtonText(button, text);

	return this;
}

void MessageDialog::setDefaultButton(QMessageBox::StandardButton button)
{
	Box->setDefaultButton(button);
}

int MessageDialog::exec()
{
	return Box->exec();
}

bool MessageDialog::ask()
{
	return QMessageBox::Yes == Box->exec();
}

#include "moc_message-dialog.cpp"
