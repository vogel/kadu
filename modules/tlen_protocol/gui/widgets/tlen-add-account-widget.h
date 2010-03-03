/*
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

#ifndef TLEN_ADD_ACCOUNT_WIDGET_H
#define TLEN_ADD_ACCOUNT_WIDGET_H

#include "gui/widgets/account-add-widget.h"

#include "accounts/account.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class IdentitiesComboBox;

class TlenAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	IdentitiesComboBox *Identity;
	QCheckBox *RememberPassword;

	QPushButton *AddAccountButton;
	//QPushButton *RemindPassword;

	void createGui();
	void createAccountGui(QGridLayout *gridLayout, int &row);

private slots:
	void addAccountButtonClicked();
	void cancelButtonClicked();

	void dataChanged();

public:
	explicit TlenAddAccountWidget(QWidget *parent = 0);
	virtual ~TlenAddAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

signals:
	void accountCreated(Account account);
	
	void remindPasssword();
};

#endif // TLEN_ADD_ACCOUNT_WIDGET_H
