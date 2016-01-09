/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtWidgets/QDialog>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "os/generic/desktop-aware-object.h"
#include "status/status.h"
#include "exports.h"

class QComboBox;
class QLabel;
class QTextEdit;

class StatusContainer;
class StatusTypeManager;

class KADUAPI StatusWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	QPointer<StatusTypeManager> m_statusTypeManager;

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

	void createLayout();
	void setupStatusSelect();
	void setupDescriptionSelect(const QString &description);

	void checkDescriptionLengthLimit();

	StatusType findCommonStatusType(const QList<StatusContainer *> &containers);

private slots:
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_INIT void init();

	void applyStatus();
	void descriptionSelected(int index);
	void clearDescriptionsHistory();
	void eraseDescription();
	void descriptionEditTextChanged();

protected:
	virtual bool eventFilter(QObject *source, QEvent *event);

public:
	explicit StatusWindow(StatusContainer *Container, QWidget *parent = nullptr);
	virtual ~StatusWindow();

	virtual QSize sizeHint() const;

signals:
	void statusWindowClosed(StatusContainer *statusContainer);

};
