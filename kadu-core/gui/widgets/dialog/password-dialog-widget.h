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

#ifndef PASSWORD_DIALOG_WIDGET_H
#define PASSWORD_DIALOG_WIDGET_H

#include <QtCore/QVariant>

#include "gui/widgets/dialog/dialog-widget.h"

class QCheckBox;
class QLineEdit;
class QFormLayout;

class PasswordDialogWidget : public DialogWidget
{
	Q_OBJECT

	QVariant Data;

	QFormLayout *formLayout;
	QLineEdit *Password;
	QCheckBox *Store;

	virtual void createGui();

private slots:
	virtual void dialogAccepted();
	virtual void dialogRejected();
	void passwordTextChanged(const QString &text);

public:
	explicit PasswordDialogWidget(const QString &title, QVariant data, QWidget* parent);
	virtual ~PasswordDialogWidget();

signals:
	void passwordEntered(const QVariant &data, const QString &password, bool permanent);

};

#endif // PASSWORD_DIALOG_WIDGET_H
