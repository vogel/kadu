/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_ICON_H
#define STATUS_ICON_H

#include <QtGui/QPushButton>

#include "configuration/configuration-aware-object.h"
#include "exports.h"
#include "icons/kadu-icon.h"
#include "status/status.h"

class StatusContainer;

class KADUAPI StatusIcon : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT

	StatusContainer *MyStatusContainer;
	QTimer *BlinkTimer;
	bool BlinkOffline;

	KaduIcon Icon;

	void updateStatus();

	void enableBlink();
	void disableBlink();

	void setIcon(const KaduIcon &icon);

private slots:
	void blink();
	void statusUpdated();

protected:
	virtual void configurationUpdated();

public:
	explicit StatusIcon(StatusContainer *statusContainer, QObject *parent = 0);
	virtual ~StatusIcon();

	const KaduIcon & icon() const { return Icon; }

signals:
	void iconUpdated(const KaduIcon &icon);

};

#endif // STATUS_ICON_H
