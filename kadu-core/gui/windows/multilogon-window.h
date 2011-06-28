/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MULTILOGON_WINDOW_H
#define MULTILOGON_WINDOW_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

class QPushButton;
class QTableView;

class AccountsComboBox;
class MultilogonService;
class MultilogonSession;

class KADUAPI MultilogonWindow : public QWidget, DesktopAwareObject
{
	Q_OBJECT

	static MultilogonWindow *Instance;

	AccountsComboBox *Accounts;
	QTableView *SessionsTable;
	QPushButton *KillSessionButton;

	explicit MultilogonWindow(QWidget *parent = 0);
    virtual ~MultilogonWindow();

	void createGui();

	MultilogonService * multilogonService();
	MultilogonSession * multilogonSession();

private slots:
	void accountChanged();
	void selectionChanged();
	void killSession();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static MultilogonWindow * instance();

	void show();

};

#endif // MULTILOGON_WINDOW_H
