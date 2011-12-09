/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHOOSE_DESCRIPTION_H
#define CHOOSE_DESCRIPTION_H

#include <QtGui/QDialog>

#include "os/generic/desktop-aware-object.h"

class QComboBox;
class QLabel;

class StatusContainer;

class ChooseDescription : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	static QMap<QWidget *, ChooseDescription *> Dialogs;

	QComboBox *Description;
	QLabel *AvailableChars;
	QPushButton *OkButton;

	QList<StatusContainer *> StatusContainers;
	StatusContainer *FirstStatusContainer;

	void setPosition(const QPoint &position);

	explicit ChooseDescription(const QList<StatusContainer *> &statusContainerList, QWidget *parent = 0);
	virtual ~ChooseDescription();

private slots:
	void statusUpdated();
	void setDescription();
	void activated(int);
	void currentDescriptionChanged(const QString &);

public:
	static QPoint ShowCentered;
	/**
	 * @short Shows new choose description dialog window.
	 * @return Pointer to the created ChooseDescription dialog.
	 * @param statusContainerList list of statusContainers
	 * @param position position in which new dialog should be shown on the desktop, ChooseDescription::ShowCentered by default
	 * @param parent dialog's parent widget, null by default
	 *
	 * Creates and shows new choose description dialog window.
	 *
	 * Use ChooseDescription::ShowCentered value for position parameter to show the widget in the center of the desktop.
	 */
	static ChooseDescription * showDialog(const QList<StatusContainer *> &statusContainerList, const QPoint &position = ChooseDescription::ShowCentered, QWidget *parent = 0);

	virtual QSize sizeHint() const;

};

#endif // CHOOSE_DESCRIPTION_H
