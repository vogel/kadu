/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/kadu-paths.h"

#include "gui/windows/mpris-player-dialog.h"
#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

MPRISPlayerConfigurationUiHandler *MPRISPlayerConfigurationUiHandler::Instance = 0;

void MPRISPlayerConfigurationUiHandler::registerConfigurationUi()
{
	if (Instance)
		return;

	Instance = new MPRISPlayerConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
	MainConfigurationWindow::registerUiHandler(Instance);
}

void MPRISPlayerConfigurationUiHandler::unregisterConfigurationUi()
{
	if (!Instance)
		return;

	MainConfigurationWindow::unregisterUiHandler(Instance);
	delete Instance;
	Instance = 0;

	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
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

	loadPlayersListFromFile(MPRISPlayer::globalPlayersListFileName(), MPRISPlayer::userPlayersListFileName());
	fillPlayersBox();
	PlayersBox->setCurrentIndex(PlayersBox->findText(config_file.readEntry("MPRISPlayer", "Player")));

	connect(add, SIGNAL(clicked()), this, SLOT(addPlayer()));
	connect(edit, SIGNAL(clicked()), this, SLOT(editPlayer()));
	connect(del, SIGNAL(clicked()), this, SLOT(delPlayer()));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

void MPRISPlayerConfigurationUiHandler::loadPlayersListFromFile(const QString &globalFileName, const QString &userFileName)
{
	PlainConfigFile globalPlayersFile(globalFileName);
	PlainConfigFile userPlayersFile(userFileName);

	QStringList globalSections = globalPlayersFile.getGroupList();
	QStringList userSections = userPlayersFile.getGroupList();

	PlayersMap.clear();

	foreach (const QString &section, userSections)
	{
		QString player = userPlayersFile.readEntry(section, "player");
		QString service = userPlayersFile.readEntry(section, "service");

		if (!player.isEmpty() && !service.isEmpty())
			PlayersMap.insert(player, service);
	}

	foreach (const QString &globalSection, globalSections)
	{
		if (userSections.contains(globalSection))
			continue;

		QString player = globalPlayersFile.readEntry(globalSection, "player");
		QString service = globalPlayersFile.readEntry(globalSection, "service");

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

	QString oldPlayerName = config_file.readEntry("MPRISPlayer", "Player");
	PlainConfigFile userPlayersFile(MPRISPlayer::userPlayersListFileName());

	userPlayersFile.writeEntry(newPlayer, "player", newPlayer);
	userPlayersFile.writeEntry(newPlayer, "service", newService);
	userPlayersFile.sync();

	loadPlayersListFromFile(MPRISPlayer::globalPlayersListFileName(), MPRISPlayer::userPlayersListFileName());
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

	PlainConfigFile globalPlayersFile(MPRISPlayer::globalPlayersListFileName());
	PlainConfigFile userPlayersFile(MPRISPlayer::userPlayersListFileName());
	QStringList sections = globalPlayersFile.getGroupList();

	if (!sections.contains(oldPlayer))
		sections = userPlayersFile.getGroupList();

	foreach (const QString &section, sections)
	{
		if (section != oldPlayer)
			continue;

		userPlayersFile.writeEntry(section, "player", QString());
		userPlayersFile.writeEntry(section, "service", QString());

		userPlayersFile.writeEntry(newPlayer, "player", newPlayer);
		userPlayersFile.writeEntry(newPlayer, "service", newService);
		break;
	}

	userPlayersFile.sync();

	loadPlayersListFromFile(MPRISPlayer::globalPlayersListFileName(), MPRISPlayer::userPlayersListFileName());
	fillPlayersBox();

	PlayersBox->setCurrentIndex(PlayersBox->findText(newPlayer));
}

void MPRISPlayerConfigurationUiHandler::delPlayer()
{
	QString playerToRemove = PlayersBox->currentText();

	PlainConfigFile globalPlayersFile(MPRISPlayer::globalPlayersListFileName());
	PlainConfigFile userPlayersFile(MPRISPlayer::userPlayersListFileName());

	QStringList sections = globalPlayersFile.getGroupList();

	if (!sections.contains(playerToRemove))
		sections = userPlayersFile.getGroupList();

	foreach (const QString &section, sections)
	{
		if (section != playerToRemove)
			continue;

		userPlayersFile.writeEntry(section, "player", QString());
		userPlayersFile.writeEntry(section, "service", QString());
		break;
	}

	userPlayersFile.sync();

	loadPlayersListFromFile(MPRISPlayer::globalPlayersListFileName(), MPRISPlayer::userPlayersListFileName());
	fillPlayersBox();

	PlayersBox->setCurrentIndex(-1);
}

void MPRISPlayerConfigurationUiHandler::configurationApplied()
{
	config_file.writeEntry("MPRISPlayer", "Player", PlayersBox->currentText());
	config_file.writeEntry("MPRISPlayer", "Service", PlayersMap.value(PlayersBox->currentText()));

	MPRISPlayer::instance()->configurationApplied();
}
