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

#include "gui/widgets/configuration/configuration-window.h"

class GaduAccountData;
class GaduServerConnector;

class GaduConfigurationDialog : public ConfigurationWindow
{
	Q_OBJECT

	GaduAccountData *AccountData;
	void createGui();

private slots:
	void registerNewAccountClicked();
	void registerNewAccountFinished(GaduServerConnector *);

public:
	GaduConfigurationDialog(GaduAccountData *accountData, QWidget *parent = 0);
	~GaduConfigurationDialog();

};

#endif // GADU_CONFIGURATION_DIALOG
