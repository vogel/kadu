 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CREATE_ACCOUNT_WIDGET_H
#define JABBER_CREATE_ACCOUNT_WIDGET_H

#include <QtGui/QWidget>

class JabberCreateAccountWidget : public QWidget
{
	Q_OBJECT

	QList<QWidget *> HaveJidWidgets;
	QList<QWidget *> DontHaveJidWidgets;

	void createGui();

private slots:
	void haveJidChanged(bool haveNumber);

public:
	explicit JabberCreateAccountWidget(QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

};

#endif // JABBER_CREATE_ACCOUNT_WIDGET_H
