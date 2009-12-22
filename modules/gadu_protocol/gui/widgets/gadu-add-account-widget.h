 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_ADD_ACCOUNT_WIDGET_H
#define GADU_ADD_ACCOUNT_WIDGET_H

#include "gui/widgets/account-add-widget.h"

class QCheckBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class GaduAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	ChooseIdentityWidget *Identity;
	
	QPushButton *AddThisAccount;

	void createGui();

private slots:
	void addThisAccount();
	void dataChanged();
public:
	explicit GaduAddAccountWidget(QWidget *parent = 0);
	virtual ~GaduAddAccountWidget();

};

#endif // GADU_ADD_ACCOUNT_WIDGET_H
