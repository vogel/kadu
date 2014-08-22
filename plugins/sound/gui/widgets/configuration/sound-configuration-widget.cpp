/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtWidgets/QPushButton>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/select-file.h"
#include "icons/kadu-icon.h"

#include "sound-manager.h"

#include "sound-configuration-widget.h"

SoundConfigurationWidget::SoundConfigurationWidget(QWidget *parent) :
		NotifierConfigurationWidget(parent), CurrentNotifyEvent(QString())
{
	QPushButton *testButton = new QPushButton(KaduIcon("external_modules/mediaplayer-media-playback-play").icon(), QString(), this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	SoundFileSelectFile = new SelectFile("audio", this);
	connect(SoundFileSelectFile, SIGNAL(fileChanged()), this, SIGNAL(soundFileEdited()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->insertSpacing(0, 20);
	layout->addWidget(testButton);
	layout->addWidget(SoundFileSelectFile);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

SoundConfigurationWidget::~SoundConfigurationWidget()
{
}

void SoundConfigurationWidget::test()
{
	SoundManager::instance()->playFile(SoundFileSelectFile->file(), true);
}

void SoundConfigurationWidget::saveNotifyConfigurations()
{
	if (!CurrentNotifyEvent.isEmpty())
		SoundFiles[CurrentNotifyEvent] = SoundFileSelectFile->file();

	for (QMap<QString, QString>::const_iterator it = SoundFiles.constBegin(), end = SoundFiles.constEnd(); it != end; ++it)
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Sounds", it.key() + "_sound", it.value());
}

void SoundConfigurationWidget::switchToEvent(const QString &event)
{
	if (!CurrentNotifyEvent.isEmpty())
		SoundFiles[CurrentNotifyEvent] = SoundFileSelectFile->file();
	CurrentNotifyEvent = event;

	if (SoundFiles.contains(event))
		SoundFileSelectFile->setFile(SoundFiles[event]);
	else
		SoundFileSelectFile->setFile(Application::instance()->configuration()->deprecatedApi()->readEntry("Sounds", event + "_sound"));
}

void SoundConfigurationWidget::themeChanged(int index)
{
	if (index == 0)
		return;

	//refresh soundFiles
	for (QMap<QString, QString>::iterator it = SoundFiles.begin(), end = SoundFiles.end(); it != end; ++it)
	{
		it.value() = Application::instance()->configuration()->deprecatedApi()->readEntry("Sounds", it.key() + "_sound");
		if (it.key() == CurrentNotifyEvent)
			SoundFileSelectFile->setFile(it.value());
	}
}

#include "moc_sound-configuration-widget.cpp"
