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

#include "sound-buddy-configuration-widget.h"

#include "gui/sound-select-file.h"
#include "sound-manager.h"

#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

SoundBuddyConfigurationWidget::SoundBuddyConfigurationWidget(const Buddy &buddy, SoundManager *soundManager, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		BuddyConfigurationWidget(buddy, parent),
		m_soundManager(soundManager),
		m_stateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("Sound"));

	createGui();
	loadValues();
	updateState();
}

SoundBuddyConfigurationWidget::~SoundBuddyConfigurationWidget()
{
}

void SoundBuddyConfigurationWidget::createGui()
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

void SoundBuddyConfigurationWidget::loadValues()
{
	m_useCustomSoundCheckBox->setChecked(buddy().property("sound:use_custom_sound", false).toBool());
	m_customSoundSelectFile->setFile(buddy().property("sound:custom_sound", QString{}).toString());
}

void SoundBuddyConfigurationWidget::updateState()
{
	m_customSoundSelectFile->setEnabled(m_useCustomSoundCheckBox->isChecked());

	if (buddy().property("sound:use_custom_sound", false).toBool() != m_useCustomSoundCheckBox->isChecked())
	{
		m_stateNotifier->setState(StateChangedDataValid);
		return;
	}

	if (!m_useCustomSoundCheckBox->isChecked())
	{
		m_stateNotifier->setState(StateNotChanged);
		return;
	}

	if (buddy().property("sound:custom_sound", QString{}).toString() != m_customSoundSelectFile->file())
		m_stateNotifier->setState(StateChangedDataValid);
	else
		m_stateNotifier->setState(StateNotChanged);
}

const ConfigurationValueStateNotifier * SoundBuddyConfigurationWidget::stateNotifier() const
{
	return m_stateNotifier;
}

void SoundBuddyConfigurationWidget::apply()
{
	buddy().addProperty("sound:use_custom_sound", m_useCustomSoundCheckBox->isChecked(), CustomProperties::Storable);
	buddy().addProperty("sound:custom_sound", m_customSoundSelectFile->file(), CustomProperties::Storable);
	updateState();
}

void SoundBuddyConfigurationWidget::cancel()
{
	loadValues();
}
