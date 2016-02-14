/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "misc/memory.h"
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class ScreenModeChecker;

class FullScreenService : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

public:
	Q_INVOKABLE explicit FullScreenService(QObject *parent = nullptr);
	virtual ~FullScreenService();

	bool hasFullscreenApplication() const;

	void start();
	void stop();

signals:
	void fullscreenToggled(bool fullscreen);

private:
	not_owned_qptr<ScreenModeChecker> m_fullscreenChecker;
	bool m_hasFullscreenApplication;
	int m_started;

private slots:
	void fullscreenToggledSlot(bool fullscreen);

};
