/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHOOSE_DESCRIPTION_H
#define CHOOSE_DESCRIPTION_H

#include <QtGui/QDialog>

#include "status/status.h"

class QComboBox;
class QLabel;

class StatusContainer;

class ChooseDescription : public QDialog
{
	Q_OBJECT

	static QMap<StatusContainer *, ChooseDescription *> Dialogs;

	QComboBox *Description;
	QLabel *AvailableChars;
	QPushButton *OkButton;

	StatusContainer *MyStatusContainer;

	void setPosition(const QPoint &position);

	explicit ChooseDescription(StatusContainer *statusContainer, QWidget *parent = 0);
	virtual ~ChooseDescription();

private slots:
	void statusChanged();
	void setDescription();
	void activated(int);
	void currentDescriptionChanged(const QString &);

public:
	static ChooseDescription * showDialog(StatusContainer *statusContainer, const QPoint &position = QPoint());

	virtual QSize sizeHint() const;

};

#endif // CHOOSE_DESCRIPTION_H
