/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHOOSE_IDENTITY_WIDGET_H
#define CHOOSE_IDENTITY_WIDGET_H

#include <QtGui/QWidget>

class QComboBox;
class QLineEdit;

class ChooseIdentityWidget : public QWidget
{
	Q_OBJECT

	QComboBox *description;
	QLineEdit *newDescriptionEdit;

private slots:
	void identitySelected(int index);

public:
	ChooseIdentityWidget(QWidget *parent = 0);

	QString identityName();

signals:
	void identityChanged();
};

#endif // CHOOSE_IDENTITY_WIDGET_H
