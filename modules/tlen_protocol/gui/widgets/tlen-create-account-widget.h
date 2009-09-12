 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CREATE_ACCOUNT_WIDGET_H
#define TLEN_CREATE_ACCOUNT_WIDGET_H

#include "gui/widgets/account-create-widget.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class TlenCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QList<QWidget *> Widgets;

	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	ChooseIdentityWidget *Identity;
	QCheckBox *RememberPassword;

	QPushButton *AddThisAccount;
	//QPushButton *RemindPassword;

	void createGui();
	void createAccountGui(QGridLayout *gridLayout, int &row);

private slots:
	void iHaveAccountDataChanged();
	void addThisAccount();

public:
	explicit TlenCreateAccountWidget(QWidget *parent = 0);
	virtual ~TlenCreateAccountWidget();

};

#endif // TLEN_CREATE_ACCOUNT_WIDGET_H
