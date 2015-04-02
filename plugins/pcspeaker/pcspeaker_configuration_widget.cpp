/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
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
	testButton->setIconSize(QSize{14, 14});
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(testButton);
	layout->addWidget(soundEdit);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

PCSpeakerConfigurationWidget::~PCSpeakerConfigurationWidget()
{
}

void PCSpeakerConfigurationWidget::saveNotifyConfigurations()
{
	if (!CurrentNotificationEvent.isEmpty())
		Sounds[CurrentNotificationEvent] = soundEdit->text();

	for (QMap<QString, QString>::const_iterator it = Sounds.constBegin(), end = Sounds.constEnd(); it != end; ++it)
		Application::instance()->configuration()->deprecatedApi()->writeEntry("PC Speaker", it.key() + "_Sound", it.value());
}

void PCSpeakerConfigurationWidget::switchToEvent(const QString &event)
{
	if (!CurrentNotificationEvent.isEmpty())
		Sounds[CurrentNotificationEvent] = soundEdit->text();

	CurrentNotificationEvent = event;

	if (Sounds.contains(event))
		soundEdit->setText(Sounds[event]);
	else
		soundEdit->setText(Application::instance()->configuration()->deprecatedApi()->readEntry("PC Speaker", event + "_Sound"));
}

void PCSpeakerConfigurationWidget::test()
{
	PCSpeaker::instance()->parseAndPlay(soundEdit->text());
}

#include "moc_pcspeaker_configuration_widget.cpp"
