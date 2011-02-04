/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>

#include "gui/widgets/configuration/notify-group-box.h"
#include "configuration/configuration-file.h"

#include "pcspeaker_configuration_widget.h"
#include "pcspeaker.h"

PCSpeakerConfigurationWidget::PCSpeakerConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	soundEdit = new QLineEdit(this);
	soundEdit->setToolTip(tr("Put the played sounds separate by space, _ for pause, eg. D2 C1# G0"));
	testButton = new QPushButton(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-play"),"", this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(testButton);
	layout->addWidget(soundEdit);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

PCSpeakerConfigurationWidget::~PCSpeakerConfigurationWidget()
{
}

void PCSpeakerConfigurationWidget::saveNotifyConfigurations()
{
	if (!CurrentNotifyEvent.isEmpty())
		Sounds[CurrentNotifyEvent] = soundEdit->text();

	foreach (const QString &key, Sounds.keys())
		config_file.writeEntry("PC Speaker", key + "_Sound", Sounds[key]);
}

void PCSpeakerConfigurationWidget::switchToEvent(const QString &event)
{
	if (!CurrentNotifyEvent.isEmpty())
		Sounds[CurrentNotifyEvent] = soundEdit->text();

	CurrentNotifyEvent = event;

	if (Sounds.contains(event))
		soundEdit->setText(Sounds[event]);
	else
		soundEdit->setText(config_file.readEntry("PC Speaker", event + "_Sound"));
}

void PCSpeakerConfigurationWidget::test()
{
	pcspeaker->parseAndPlay(soundEdit->text());
}
