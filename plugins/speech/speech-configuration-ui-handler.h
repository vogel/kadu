/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#pragma once

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QCheckBox;
class QLineEdit;
class QSlider;

class BuddyDummyFactory;
class ConfigComboBox;
class Configuration;
class MainConfigurationWindow;
class Parser;
class SelectFile;
class Speech;

class SpeechConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
    Q_OBJECT

    QPointer<BuddyDummyFactory> m_buddyDummyFactory;
    QPointer<Configuration> m_configuration;
    QPointer<Parser> m_parser;
    QPointer<Speech> m_speech;

    QSlider *frequencySlider;
    QSlider *tempoSlider;
    QSlider *baseFrequencySlider;
    QLineEdit *dspDeviceLineEdit;
    QCheckBox *klattSyntCheckBox;
    QCheckBox *melodyCheckBox;

    SelectFile *programSelectFile;

    ConfigComboBox *soundSystemComboBox;

private slots:
    INJEQT_SET void setBuddyDummyFactory(BuddyDummyFactory *buddyDummyFactory);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setParser(Parser *parser);
    INJEQT_SET void setSpeech(Speech *speech);

    void testSpeech();
    void soundSystemChanged(int index);

public:
    Q_INVOKABLE explicit SpeechConfigurationUiHandler(QObject *parent = nullptr);
    virtual ~SpeechConfigurationUiHandler();

    virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
    virtual void mainConfigurationWindowDestroyed() override;
    virtual void mainConfigurationWindowApplied() override;
};
