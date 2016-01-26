/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QPushButton;
class QTableView;

class AccountsComboBox;
class Configuration;
class MultilogonService;
class MultilogonSession;

class KADUAPI MultilogonWindow : public QWidget, DesktopAwareObject
{
	Q_OBJECT

	static MultilogonWindow *Instance;

	QPointer<Configuration> m_configuration;

	AccountsComboBox *Accounts;
	QTableView *SessionsTable;
	QPushButton *KillSessionButton;

	void createGui();

	MultilogonService * multilogonService();
	MultilogonSession * multilogonSession();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

	void accountChanged();
	void selectionChanged();
	void killSession();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static MultilogonWindow * instance();

	explicit MultilogonWindow(QWidget *parent = nullptr);
	virtual ~MultilogonWindow();

	void show();

};
