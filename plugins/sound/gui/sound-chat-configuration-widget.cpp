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

#include "sound-chat-configuration-widget.h"

#include "gui/sound-select-file.h"
#include "sound-manager.h"

#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

SoundChatConfigurationWidget::SoundChatConfigurationWidget(const Chat &chat, SoundManager *soundManager, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		ChatConfigurationWidget(chat, parent),
		m_soundManager(soundManager),
		m_stateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("Sound"));

	createGui();
	loadValues();
	updateState();
}

SoundChatConfigurationWidget::~SoundChatConfigurationWidget()
{
}

void SoundChatConfigurationWidget::createGui()
{
	auto layout = new QVBoxLayout(this);

	m_useCustomSoundCheckBox = new QCheckBox(tr("Use custom sound"));
	m_customSoundSelectFile = new SoundSelectFile{m_soundManager, this};

	connect(m_useCustomSoundCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(m_customSoundSelectFile, SIGNAL(fileChanged()), this, SLOT(updateState()));

	layout->addWidget(m_useCustomSoundCheckBox);
	layout->addWidget(m_customSoundSelectFile);
	layout->addStretch(100);
}

void SoundChatConfigurationWidget::loadValues()
{
	m_useCustomSoundCheckBox->setChecked(chat().property("sound:use_custom_sound", false).toBool());
	m_customSoundSelectFile->setFile(chat().property("sound:custom_sound", QString{}).toString());
}

void SoundChatConfigurationWidget::updateState()
{
	m_customSoundSelectFile->setEnabled(m_useCustomSoundCheckBox->isChecked());

	if (chat().property("sound:use_custom_sound", false).toBool() != m_useCustomSoundCheckBox->isChecked())
	{
		m_stateNotifier->setState(StateChangedDataValid);
		return;
	}

	if (!m_useCustomSoundCheckBox->isChecked())
	{
		m_stateNotifier->setState(StateNotChanged);
		return;
	}

	if (chat().property("sound:custom_sound", QString{}).toString() != m_customSoundSelectFile->file())
		m_stateNotifier->setState(StateChangedDataValid);
	else
		m_stateNotifier->setState(StateNotChanged);
}

const ConfigurationValueStateNotifier * SoundChatConfigurationWidget::stateNotifier() const
{
	return m_stateNotifier;
}

void SoundChatConfigurationWidget::apply()
{
	chat().addProperty("sound:use_custom_sound", m_useCustomSoundCheckBox->isChecked(), CustomProperties::Storable);
	chat().addProperty("sound:custom_sound", m_customSoundSelectFile->file(), CustomProperties::Storable);
	updateState();
}

void SoundChatConfigurationWidget::cancel()
{
	loadValues();
}
