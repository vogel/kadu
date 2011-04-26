/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef GADU_REMIND_PASSWORD_WINDOW_H
#define GADU_REMIND_PASSWORD_WINDOW_H

#include <QtGui/QWidget>

#include "gadu-protocol.h" //for UinType

class QLineEdit;
class QPushButton;

class GaduServerRemindPassword;
class TokenWidget;

class GaduRemindPasswordWindow : public QWidget
{
	Q_OBJECT

	UinType Uin;

	QLineEdit *EMail;
	TokenWidget *MyTokenWidget;
	QPushButton *SendPasswordButton;

	void createGui();

private slots:
	void dataChanged();
	void sendPassword();
	void remindPasswordFinished(GaduServerRemindPassword *gsrp);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit GaduRemindPasswordWindow(UinType uin, QWidget *parent = 0);
	virtual ~GaduRemindPasswordWindow();
};

#endif // GADU_REMIND_PASSWORD_WINDOW_H
