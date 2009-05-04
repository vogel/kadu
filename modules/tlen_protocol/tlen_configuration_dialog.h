/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONFIGURATION_DIALOG
#define TLEN_CONFIGURATION_DIALOG

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/windows/configuration-window.h"

class TlenAccount;

class TlenConfigurationDialog : public ConfigurationWindow
{
	Q_OBJECT

	TlenAccount *AccountData;

	void createGui();

public:
	TlenConfigurationDialog(TlenAccount *accountData, QWidget *parent = 0);
	~TlenConfigurationDialog();

};

#endif // TLEN_CONFIGURATION_DIALOG
