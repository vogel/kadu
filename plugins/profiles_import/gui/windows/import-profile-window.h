/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IMPORT_PROFILE_WINDOW_H
#define IMPORT_PROFILE_WINDOW_H

#include <QtWidgets/QDialog>

class QCheckBox;
class QLabel;

class IdentitiesComboBox;
class PathEdit;

class ImportProfileWindow : public QDialog
{
	Q_OBJECT

	PathEdit *ProfilePathEdit;
	IdentitiesComboBox *SelectIdentity;
	QCheckBox *ImportHistory;
	QLabel *ErrorLabel;
	QPushButton *ImportButton;

	void createGui();

private slots:
	void validate();

public:
	explicit ImportProfileWindow(QWidget *parent = 0);
	virtual ~ImportProfileWindow();

	virtual void accept();

};

#endif // IMPORT_PROFILE_WINDOW_H
