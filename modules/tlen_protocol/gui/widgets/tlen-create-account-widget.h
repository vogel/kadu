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

class TlenCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QList<QWidget *> HaveNumberWidgets;
	QList<QWidget *> DontHaveNumberWidgets;

	void createGui();

private slots:
	void haveNumberChanged(bool haveNumber);

public:
	explicit TlenCreateAccountWidget(QWidget *parent = 0);
	virtual ~TlenCreateAccountWidget();

};

#endif // TLEN_CREATE_ACCOUNT_WIDGET_H
