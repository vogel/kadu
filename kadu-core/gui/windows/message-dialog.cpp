/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtGui/QStyle>

#include "icons-manager.h"

#include "message-dialog.h"

void KADUAPI MessageDialog::show(const QString &iconName, const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
			QWidget *parent, Qt::WindowFlags f)
{
	QMessageBox *mb = new QMessageBox(QMessageBox::NoIcon, title, text, buttons, parent, f);
	mb->setAttribute(Qt::WA_DeleteOnClose, true);

	QIcon icon = IconsManager::instance()->iconByPath(iconName);
	if (!icon.isNull())
	{
		QStyle *style = mb->style();
		int iconSize = style->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, mb);

		mb->setIconPixmap(icon.pixmap(iconSize, iconSize));
	}

	mb->show();
}

int KADUAPI MessageDialog::exec(const QString &iconName, const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
			QWidget *parent, Qt::WindowFlags f)
{
	QMessageBox mb(QMessageBox::NoIcon, title, text, buttons, parent, f);

	QIcon icon = IconsManager::instance()->iconByPath(iconName);
	if (!icon.isNull())
	{
		QStyle *style = mb.style();
		int iconSize = style->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, &mb);

		mb.setIconPixmap(icon.pixmap(iconSize, iconSize));
	}

	return mb.exec();
}

bool KADUAPI MessageDialog::ask(const QString &iconName, const QString &title, const QString &text, QWidget *parent, Qt::WindowFlags f)
{
	return QMessageBox::Yes == exec(iconName, title, text, QMessageBox::Yes | QMessageBox::No, parent, f);
}
