/*
 * %kadu copyright begin%
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef KADU_SOUND_SLOTS
#define KADU_SOUND_SLOTS

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "gui/windows/message-dialog.h"

#include "gui/widgets/configuration/notifier-configuration-widget.h"

#include "sound.h"

class SelectFile;

class ActionDescription;

/** @ingroup sound
 * @{
 */

class SoundSlots : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT

	ActionDescription* mute_action;
	QMap<QString, QString> soundfiles;
	QStringList soundNames;
	QStringList soundTexts;

	MessageDialog* SamplePlayingTestMsgBox;
	SoundDevice SamplePlayingTestDevice;
	qint16*    SamplePlayingTestSample;

	MessageDialog* FullDuplexTestMsgBox;
	SoundDevice FullDuplexTestDevice;
	qint16*    FullDuplexTestSample;

private slots:
	void muteActionActivated(QAction *action, bool is_on);
	void setMuteActionState();
	void muteUnmuteSounds();
	void testSamplePlaying();

protected:
	void configurationUpdated();
public:
	SoundSlots(bool firstLoad, QObject *parent = 0);
	~SoundSlots();
public slots:
	void themeChanged(const QString &theme);
};

/** @} */
#endif // KADU_SOUND_SLOTS
