/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "icons/kadu-icon.h"

#include "pcspeaker.h"
#include "pcspeaker_configuration_widget.h"

PCSpeakerConfigurationWidget::PCSpeakerConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	soundEdit = new QLineEdit(this);
	soundEdit->setToolTip(tr("Put the played sounds separate by space, _ for pause, eg. D2 C1# G0"));
	testButton = new QPushButton(KaduIcon("external_modules/mediaplayer-media-playback-play").icon(), QString(), this);
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

	for (QMap<QString, QString>::const_iterator it = Sounds.constBegin(), end = Sounds.constEnd(); it != end; ++it)
		config_file.writeEntry("PC Speaker", it.key() + "_Sound", it.value());
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
	PCSpeaker::instance()->parseAndPlay(soundEdit->text());
}

#include "moc_pcspeaker_configuration_widget.cpp"
