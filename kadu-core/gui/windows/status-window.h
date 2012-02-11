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

#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW_H

#include <QtGui/QDialog>

#include "os/generic/desktop-aware-object.h"

class QComboBox;
class QTextEdit;
class QLabel;

class StatusContainer;

class StatusWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	static QMap<QWidget *, StatusWindow *> Dialogs;

	QComboBox *statusCombo;
	QTextEdit *DescriptionEdit;
	QLabel *AvailableChars;
	QPushButton *OkButton;

	QList<StatusContainer *> StatusContainers;
	StatusContainer *FirstStatusContainer;

	void setPosition(const QPoint &position);

	explicit StatusWindow(const QList<StatusContainer *> &statusContainerList, QWidget *parent = 0);
	virtual ~StatusWindow();

private slots:
	void setDescription();
	void currentDescriptionChanged();
	void openDescriptionsList();
  	void descriptionSelected(const QString &description);

public:
	static QPoint ShowCentered;
	/**
	 * @short Shows new choose description dialog window.
	 * @return Pointer to the created StatusWindow dialog.
	 * @param statusContainerList list of statusContainers
	 * @param position position in which new dialog should be shown on the desktop, StatusWindow::ShowCentered by default
	 * @param parent dialog's parent widget, null by default
	 *
	 * Creates and shows new choose description dialog window.
	 *
	 * Use StatusWindow::ShowCentered value for position parameter to show the widget in the center of the desktop.
	 */
	static StatusWindow * showDialog(const QList<StatusContainer *> &statusContainerList, const QPoint &position = StatusWindow::ShowCentered, QWidget *parent = 0);

	virtual QSize sizeHint() const;
};

#endif // STATUS_WINDOW_H
