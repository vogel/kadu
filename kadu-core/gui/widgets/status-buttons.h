/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_BUTTONS_H
#define STATUS_BUTTONS_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "status/status-container-aware-object.h"

class QHBoxLayout;
class QSpacerItem;

class StatusButton;
class StatusContainer;

class KADUAPI StatusButtons : public QWidget, private StatusContainerAwareObject
{
	Q_OBJECT

	QHBoxLayout *Layout;
	QSpacerItem *Spacer;
	bool SimpleMode;

	QMap<StatusContainer *, StatusButton *> Buttons;

	void createGui();
	void enableStatusName();
	void disableStatusName();
	void addButton(StatusButton *button);
	void updateLayout(bool addStretch);

private slots:
    void simpleModeChanged();
	void rebuildGui();

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	explicit StatusButtons(QWidget *parent = 0);
	virtual ~StatusButtons();

};

#endif // STATUS_BUTTONS_H
