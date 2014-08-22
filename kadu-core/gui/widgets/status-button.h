/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_BUTTON_H
#define STATUS_BUTTON_H

#include <QtGui/QToolButton>

#include "configuration/configuration-aware-object.h"
#include "icons/kadu-icon.h"
#include "status/status.h"

class StatusContainer;
class StatusIcon;
class StatusMenu;

class StatusButton : public QToolButton, private ConfigurationAwareObject
{
	Q_OBJECT

	StatusContainer *MyStatusContainer;
	bool DisplayStatusName;
	StatusIcon *Icon;
	QAction *MenuTitleAction;

	void createGui();
	void updateStatus();

	QString prepareDescription(const QString &description) const;

private slots:
	void iconUpdated(const KaduIcon &icon);
	void statusUpdated(StatusContainer *container = 0);
	void addTitleToMenu(const QString &title, QMenu *menu);

protected:
	virtual void configurationUpdated();

public:
	explicit StatusButton(StatusContainer *statusContainer, QWidget *parent = 0);
	virtual ~StatusButton();

	void setDisplayStatusName(bool displayStatusName);
	bool eventFilter(QObject *object, QEvent *event);

};

#endif // STATUS_BUTTON_H
