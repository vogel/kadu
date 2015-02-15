/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/actions/action-description.h"

#include <QtCore/QPointer>

class SoundManager;

class SoundMuteAction : public ActionDescription
{
	Q_OBJECT

public:
	explicit SoundMuteAction(QObject *parent = nullptr);
	virtual ~SoundMuteAction();

	void setSoundManager(SoundManager *soundManager);

protected:
	virtual void actionTriggered(QAction *sender, bool toggled) override;
	virtual void updateActionState(Action *action) override;

private:
	QPointer<SoundManager> m_soundManager;

};
