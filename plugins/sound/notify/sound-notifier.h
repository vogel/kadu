/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SOUND_NOTIFIER_H
#define SOUND_NOTIFIER_H

#include "notify/notifier.h"

class SoundNotifier : public Notifier
{
	Q_OBJECT
	Q_DISABLE_COPY(SoundNotifier)

	static SoundNotifier *Instance;

	SoundNotifier();
	virtual ~SoundNotifier();

public:
	static void createInstance();
	static void destroyInstance();
	static SoundNotifier * instance();

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);
	virtual void notify(Notification *notification);

};

#endif // SOUND_NOTIFIER_H
