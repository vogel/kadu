/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/paths-provider.h"

#include "gui/windows/mpris-player-dialog.h"
#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

MPRISPlayerConfigurationUiHandler *MPRISPlayerConfigurationUiHandler::Instance = 0;

void MPRISPlayerConfigurationUiHandler::registerConfigurationUi()
{
	if (Instance)
		return;

	Instance = new MPRISPlayerConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
	MainConfigurationWindow::registerUiHandler(Instance);
}

void MPRISPlayerConfigurationUiHandler::unregisterConfigurationUi()
{
	if (!Instance)
		return;

	MainConfigurationWindow::unregisterUiHandler(Instance);
	delete Instance;
	Instance = 0;

	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
}

MPRISPlayerConfigurationUiHandler::MPRISPlayerConfigurationUiHandler() :
	PlayersBox(0)
{

}

MPRISPlayerConfigurationUiHandler::~MPRISPlayerConfigurationUiHandler()
{

}

void MPRISPlayerConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("MediaPlayer", "General", "MPRIS Player");

	QWidget *options = new QWidget(optionsGroupBox->widget());

	QGridLayout *selectionLayout = new QGridLayout(options);
	QGridLayout *buttonsLayout = new QGridLayout();

	QLabel *label = new QLabel(tr("Select Player:"), options);
	PlayersBox = new QComboBox(options);

	QPushButton *add = new QPushButton(tr("Add Player"), options);
	QPushButton *edit = new QPushButton(tr("Edit Player"), options);
	QPushButton *del = new QPushButton(tr("Delete Player"), options);

	selectionLayout->addWidget(label, 0, 0);
	selectionLayout->addWidget(PlayersBox, 0, 1, 1, 5);

	buttonsLayout->addWidget(add, 0, 0);
	buttonsLayout->addWidget(edit, 0, 1);
	buttonsLayout->addWidget(del, 0, 2);

	selectionLayout->addLayout(buttonsLayout, 1, 0, 1, 6);

	optionsGroupBox->addWidgets(0, options);

	loadPlayersListFromFile();
	fillPlayersBox();
	PlayersBox->setCurrentIndex(PlayersBox->findText(Application::instance()->configuration()->deprecatedApi()->readEntry("MPRISPlayer", "Player")));

	connect(add, SIGNAL(clicked()), this, SLOT(addPlayer()));
	connect(edit, SIGNAL(clicked()), this, SLOT(editPlayer()));
	connect(del, SIGNAL(clicked()), this, SLOT(delPlayer()));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

void MPRISPlayerConfigurationUiHandler::loadPlayersListFromFile()
{
	QSettings userPlayersSettings(MPRISPlayer::userPlayersListFileName(), QSettings::IniFormat);
	userPlayersSettings.setIniCodec("ISO8859-2");

	QSettings globalPlayersSettings(MPRISPlayer::globalPlayersListFileName(), QSettings::IniFormat);
	globalPlayersSettings.setIniCodec("ISO8859-2");

	QStringList globalSections = globalPlayersSettings.childGroups();
	QStringList userSections = userPlayersSettings.childGroups();

	PlayersMap.clear();

	foreach (const QString &section, userSections)
	{
		QString player = userPlayersSettings.value(section + "/player").toString();
		QString service = userPlayersSettings.value(section + "/service").toString();

		if (!player.isEmpty() && !service.isEmpty())
			PlayersMap.insert(player, service);
	}

	foreach (const QString &globalSection, globalSections)
	{
		if (userSections.contains(globalSection))
			continue;

		QString player = globalPlayersSettings.value(globalSection + "/player").toString();
		QString service = globalPlayersSettings.value(globalSection + "/service").toString();

		if (!player.isEmpty() && !service.isEmpty())
			PlayersMap.insert(player, service);
	}
}

void MPRISPlayerConfigurationUiHandler::fillPlayersBox()
{
	QMap<QString, QString>::const_iterator it = PlayersMap.constBegin();
	PlayersBox->clear();

	while (it != PlayersMap.constEnd())
	{
		PlayersBox->addItem(it.key());
		++it;
	}
}

void MPRISPlayerConfigurationUiHandler::addPlayer()
{
	MPRISPlayerDialog Dialog;

	if (Dialog.exec() != QDialog::Accepted)
		return;

	QString newPlayer = Dialog.getPlayer();
	QString newService = Dialog.getService();

	if (newPlayer.isEmpty() || newService.isEmpty())
		return;

	QString oldPlayerName = Application::instance()->configuration()->deprecatedApi()->readEntry("MPRISPlayer", "Player");
	QSettings userPlayersSettings(MPRISPlayer::userPlayersListFileName(), QSettings::IniFormat);
	userPlayersSettings.setIniCodec("ISO8859-2");

	userPlayersSettings.setValue(newPlayer + "/player", newPlayer);
	userPlayersSettings.setValue(newPlayer + "/service", newService);
	userPlayersSettings.sync();

	loadPlayersListFromFile();
	fillPlayersBox();

	PlayersBox->setCurrentIndex(PlayersBox->findText(oldPlayerName));
}

void MPRISPlayerConfigurationUiHandler::editPlayer()
{
	MPRISPlayerDialog Dialog(true);

	QString oldPlayer = PlayersBox->currentText();
	QString oldService = PlayersMap.value(oldPlayer);

	if ((oldPlayer.isEmpty() || oldService.isEmpty()))
		return;

	Dialog.setPlayer(oldPlayer);
	Dialog.setService(oldService);

	if (Dialog.exec() != QDialog::Accepted)
		return;

	QString newPlayer = Dialog.getPlayer();
	QString newService = Dialog.getService();

	if ((newPlayer.isEmpty() || newService.isEmpty()) || (newPlayer == oldPlayer && oldService == newService))
		return;

	QSettings globalPlayersSettings(MPRISPlayer::globalPlayersListFileName(), QSettings::IniFormat);
	globalPlayersSettings.setIniCodec("ISO8859-2");
	QSettings userPlayersSettings(MPRISPlayer::userPlayersListFileName(), QSettings::IniFormat);
	userPlayersSettings.setIniCodec("ISO8859-2");
	QStringList sections = globalPlayersSettings.childGroups();

	if (!sections.contains(oldPlayer))
		sections = userPlayersSettings.childGroups();

	foreach (const QString &section, sections)
	{
		if (section != oldPlayer)
			continue;

		userPlayersSettings.remove(section + "/player");
		userPlayersSettings.remove(section + "/service");

		userPlayersSettings.setValue(newPlayer + "/player", newPlayer);
		userPlayersSettings.setValue(newPlayer + "/service", newService);
		break;
	}

	userPlayersSettings.sync();

	loadPlayersListFromFile();
	fillPlayersBox();

	PlayersBox->setCurrentIndex(PlayersBox->findText(newPlayer));
}

void MPRISPlayerConfigurationUiHandler::delPlayer()
{
	QString playerToRemove = PlayersBox->currentText();

	QSettings globalPlayersSettings(MPRISPlayer::globalPlayersListFileName(), QSettings::IniFormat);
	globalPlayersSettings.setIniCodec("ISO8859-2");
	QSettings userPlayersSettings(MPRISPlayer::userPlayersListFileName(), QSettings::IniFormat);
	userPlayersSettings.setIniCodec("ISO8859-2");

	QStringList sections = globalPlayersSettings.childGroups();

	if (!sections.contains(playerToRemove))
		sections = userPlayersSettings.childGroups();

	foreach (const QString &section, sections)
	{
		if (section != playerToRemove)
			continue;

		userPlayersSettings.remove(section + "/player");
		userPlayersSettings.remove(section + "/service");
		break;
	}

	userPlayersSettings.sync();

	loadPlayersListFromFile();
	fillPlayersBox();

	PlayersBox->setCurrentIndex(-1);
}

void MPRISPlayerConfigurationUiHandler::configurationApplied()
{
	Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Player", PlayersBox->currentText());
	Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Service", PlayersMap.value(PlayersBox->currentText()));

	MPRISPlayer::instance()->configurationApplied();
}

#include "moc_mpris-player-configuration-ui-handler.cpp"
