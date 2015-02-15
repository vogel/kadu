/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ADD_GROUP_DIALOG_WIDGET_H
#define ADD_GROUP_DIALOG_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QDialog>

#include "gui/widgets/dialog/dialog-widget.h"

class QCheckBox;
class QLineEdit;
class QFormLayout;

class AddGroupDialogWidget : public DialogWidget
{
	Q_OBJECT

protected:
	QFormLayout *formLayout;
	QLineEdit *GroupName;

	virtual void createGui();

protected slots:
	virtual void dialogAccepted();
	virtual void dialogRejected();
	void groupNameTextChanged(const QString &text);

public:
	explicit AddGroupDialogWidget(const QString &title, QWidget* parent);
	virtual ~AddGroupDialogWidget();
};

#endif // ADD_GROUP_DIALOG_WIDGET_H
