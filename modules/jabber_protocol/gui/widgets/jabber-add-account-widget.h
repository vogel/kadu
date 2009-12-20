 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_ADD_ACCOUNT_WIDGET_H
#define JABBER_ADD_ACCOUNT_WIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include "gui/widgets/account-add-widget.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class JabberAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *Password;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	ChooseIdentityWidget *Identity;
	
	void createGui();

private slots:
	void dataChanged();
	void addThisAccount();
public:
	explicit JabberAddAccountWidget(QWidget *parent = 0);
	virtual ~JabberAddAccountWidget();

};

#endif // JABBER_ADD_ACCOUNT_WIDGET_H
