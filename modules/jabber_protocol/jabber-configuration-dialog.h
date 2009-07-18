/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONFIGURATION_DIALOG
#define JABBER_CONFIGURATION_DIALOG

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/windows/configuration-window.h"

class JabberAccount;

class JabberConfigurationDialog : public ConfigurationWindow
{
	Q_OBJECT

	JabberAccount *AccountData;

	void createGui();

public:
	JabberConfigurationDialog(JabberAccount *accountData, QWidget *parent = 0);
	~JabberConfigurationDialog();

};

#endif // JABBER_CONFIGURATION_DIALOG
