/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>

#include "buddies/buddy-preferred-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/select-file.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"
#include "speech.h"
#include "debug.h"

#include "speech-configuration-ui-handler.h"

SpeechConfigurationUiHandler * SpeechConfigurationUiHandler::Instance = 0;

void SpeechConfigurationUiHandler::registerUiHandler()
{
	if (!Instance)
	{
		Instance = new SpeechConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/speech.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void SpeechConfigurationUiHandler::unregisterUiHandler()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/speech.ui"));
		delete Instance;
		Instance = 0;
	}
}

SpeechConfigurationUiHandler::SpeechConfigurationUiHandler()
{
}

void SpeechConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	frequencySlider = static_cast<QSlider *>(mainConfigurationWindow->widget()->widgetById("speech/frequency"));
	tempoSlider = static_cast<QSlider *>(mainConfigurationWindow->widget()->widgetById("speech/tempo"));
	baseFrequencySlider = static_cast<QSlider *>(mainConfigurationWindow->widget()->widgetById("speech/baseFrequency"));
	melodyCheckBox = static_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("spech/melody"));

	programSelectFile = static_cast<SelectFile *>(mainConfigurationWindow->widget()->widgetById("speech/program"));;

	soundSystemComboBox = static_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("speech/soundSystem"));
	dspDeviceLineEdit = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("speech/dspDevice"));
	klattSyntCheckBox = static_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("speech/klattSynt"));

	connect(soundSystemComboBox, SIGNAL(activated(int)), this, SLOT(soundSystemChanged(int)));
	connect(mainConfigurationWindow->widget()->widgetById("speech/test"), SIGNAL(clicked()), this, SLOT(testSpeech()));
}

void SpeechConfigurationUiHandler::soundSystemChanged(int index)
{
	Q_UNUSED(index)
	bool dsp = soundSystemComboBox->currentItemValue() == "Dsp";

	dspDeviceLineEdit->setEnabled(dsp);
	klattSyntCheckBox->setEnabled(dsp);
}

void SpeechConfigurationUiHandler::testSpeech()
{
	kdebugf();

	QString program = programSelectFile->file();
	// TODO: mo�e u�ywa� jakiego� normalnego tekstu ?
	QString formatM = config_file.readEntry("Speech", "NewChat_Syntax/Male");
	QString formatF = config_file.readEntry("Speech", "NewChat_Syntax/Female");
	QString device = dspDeviceLineEdit->text();
	bool klatt = klattSyntCheckBox->isChecked();
	bool mel = melodyCheckBox->isChecked();

	QString sound_system = soundSystemComboBox->currentItemValue();
	kdebugm(KDEBUG_INFO, "flags: %d %d %s\n", mel, klatt, qPrintable(sound_system));

	int frequency = frequencySlider->value();
	int tempo = tempoSlider->value();
	int baseFrequency = baseFrequencySlider->value();

	kdebugm(KDEBUG_INFO, "%d %d %d\n", frequency, tempo, baseFrequency);

	QString text;
	text = Parser::parse(formatF, BuddyOrContact(Buddy::dummy()));

	Speech::instance()->say(text.contains("%1") ? text.arg("Test") : QString("Test"), program, klatt, mel, sound_system, device, frequency, tempo, baseFrequency);

	kdebugf2();
}
