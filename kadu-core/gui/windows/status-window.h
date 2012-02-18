/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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
#include <status/status.h>

class QComboBox;
class QLabel;
class QTextEdit;

class StatusContainer;

class StatusWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	static QMap<StatusContainer *, StatusWindow *> Dialogs;

	StatusContainer *Container;

	QComboBox *StatusSelect;
	QComboBox *DescriptionSelect;
	QPushButton *ClearDescriptionsHistoryButton;
	QLabel *DescriptionCounter;
	QTextEdit *DescriptionEdit;
	QPushButton *EraseButton;
	QPushButton *SetStatusButton;
	QPushButton *CancelButton;

	/**
	 * Don't allow descriptionEditTextChanged() to modify DescriptionSelect after changing text (programically) in DescriptionEdit.
	 */
	bool IgnoreNextTextChange;

	explicit StatusWindow(StatusContainer *Container, QWidget *parent = 0);
	virtual ~StatusWindow();

	void createLayout();
	void setupStatusSelect();
	void setupDescriptionSelect(const QString &description);

	void checkDescriptionLengthLimit();

	StatusType findCommonStatusType(const QList<StatusContainer *> &containers);

private slots:
	void applyStatus();
	void descriptionSelected(int index);
	void clearDescriptionsHistory();
	void eraseDescription();
	void descriptionEditTextChanged();

protected:
	virtual bool eventFilter(QObject *source, QEvent *event);

public:
	/**
	 * @short Shows new status window.
	 * @param statusContainer status container
	 * @param parent dialog's parent widget, null by default
	 * @return Pointer to the created StatusWindow dialog.
	 *
	 * Creates and shows new status window.
	 */
	static StatusWindow * showDialog(StatusContainer *statusContainer, QWidget *parent = 0);

	virtual QSize sizeHint() const;
};

#endif // STATUS_WINDOW_H
