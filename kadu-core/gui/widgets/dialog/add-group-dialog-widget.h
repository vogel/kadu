/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include <QtGui/QDialog>

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
