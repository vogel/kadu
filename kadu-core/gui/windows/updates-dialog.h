/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef UPDATES_DIALOG_H
#define UPDATES_DIALOG_H

#include <QtWidgets/QDialog>

#include "os/generic/desktop-aware-object.h"

class QCheckBox;

class UpdatesDialog : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	QCheckBox *CheckForUpdates;

private slots:
	void accepted();
	void downloadClicked();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit UpdatesDialog(const QString &newestVersion, QWidget *parent = 0);
	virtual ~UpdatesDialog();
};

#endif // UPDATES_DIALOG_H
