/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONFIGURATION_DIALOG
#define GADU_CONFIGURATION_DIALOG

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

class GaduAccountData;

class GaduConfigurationDialog : public QDialog
{
	Q_OBJECT

	GaduAccountData *AccountData;
	QLineEdit *UinLineEdit;
	QLineEdit *PasswordLineEdit;

	void createGui();
	void loadAccountData();

private slots:
	void updateAccountData();

public:
	GaduConfigurationDialog(GaduAccountData *accountData, QWidget *parent = 0);
	~GaduConfigurationDialog();

};

#endif // GADU_CONFIGURATION_DIALOG
