 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CREATE_ACCOUNT_WIDGET_H
#define GADU_CREATE_ACCOUNT_WIDGET_H

#include "gui/widgets/account-create-widget.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class GaduServerRegisterAccount;
class TokenWidget;

class GaduCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QList<QWidget *> HaveNumberWidgets;
	QList<QWidget *> DontHaveNumberWidgets;

	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;

	QPushButton *AddThisAccount;
	QPushButton *RemindPassword;

	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QLineEdit *EMail;

	TokenWidget *tokenWidget;

	QPushButton *registerAccount;

	void createGui();
	void createIHaveAccountGui(QGridLayout *gridLayout, int &row);
	void createRegisterAccountGui(QGridLayout *gridLayout, int &row);

private slots:
	void haveNumberChanged(bool haveNumber);
	void iHaveAccountDataChanged();
	void addThisAccount();
	void registerAccountDataChanged();
	void registerNewAccount(bool);
	void registerNewAccountFinished(GaduServerRegisterAccount *gsra);

public:
	explicit GaduCreateAccountWidget(QWidget *parent = 0);
	virtual ~GaduCreateAccountWidget();

};

#endif // GADU_CREATE_ACCOUNT_WIDGET_H
