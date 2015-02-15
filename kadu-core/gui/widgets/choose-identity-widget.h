/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHOOSE_IDENTITY_WIDGET_H
#define CHOOSE_IDENTITY_WIDGET_H

#include <QtWidgets/QWidget>

#include "exports.h"

class QComboBox;
class QLineEdit;

class KADUAPI ChooseIdentityWidget : public QWidget
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
