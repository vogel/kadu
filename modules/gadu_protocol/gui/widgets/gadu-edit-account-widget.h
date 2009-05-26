 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_EDIT_ACCOUNT_WIDGET_H
#define GADU_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QTabWidget;

class GaduEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	void createGui();
	void createGeneralTab(QTabWidget *);

public:
	explicit GaduEditAccountWidget(Account *account, QWidget *parent = 0);
	virtual ~GaduEditAccountWidget();

};

#endif // GADU_EDIT_ACCOUNT_WIDGET_H
