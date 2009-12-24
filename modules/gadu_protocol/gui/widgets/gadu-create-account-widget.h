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

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;
class GaduServerRegisterAccount;
class TokenWidget;

class GaduCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QLineEdit *AccountName;
	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QCheckBox *RememberPassword;	
	QLineEdit *EMail;
	ChooseIdentityWidget *Identity;
	TokenWidget *tokenWidget;


	void createGui();

private slots:
	void dataChanged();
	void registerNewAccountFinished(GaduServerRegisterAccount *gsra);

public:
	explicit GaduCreateAccountWidget(QWidget *parent = 0);
	virtual ~GaduCreateAccountWidget();
	
	virtual void apply();

};

#endif // GADU_CREATE_ACCOUNT_WIDGET_H
