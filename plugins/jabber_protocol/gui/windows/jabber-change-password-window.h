/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

class QLineEdit;
class QPushButton;

class JabberChangePasswordService;

class JabberChangePasswordWindow : public QWidget
{
	Q_OBJECT

public:
	explicit JabberChangePasswordWindow(JabberChangePasswordService *changePasswordService, Account account, QWidget *parent = nullptr);
	virtual ~JabberChangePasswordWindow();

signals:
	void passwordChanged(const QString &);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	QPointer<JabberChangePasswordService> m_changePasswordService;
	Account m_account;

	QLineEdit *m_newPassword;
	QLineEdit *m_reNewPassword;
	QPushButton *m_changePasswordButton;

	void createGui();

private slots:
	void dataChanged();
	void changePassword();
	void passwordChanged();
	void error(const QString &errorMessage);

};
