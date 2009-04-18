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

#include <QtGui/QWidget>

class GaduCreateAccountWidget : public QWidget
{
	Q_OBJECT

	QList<QWidget *> HaveNumberWidgets;
	QList<QWidget *> DontHaveNumberWidgets;

	void createGui();

private slots:
	void haveNumberChanged(bool haveNumber);

public:
	explicit GaduCreateAccountWidget(QWidget *parent = 0);
	virtual ~GaduCreateAccountWidget();

};

#endif // GADU_CREATE_ACCOUNT_WIDGET_H
