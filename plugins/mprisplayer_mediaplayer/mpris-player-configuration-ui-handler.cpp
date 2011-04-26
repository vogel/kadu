/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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
#include "misc/path-conversion.h"

#include "gui/windows/mpris-player-dialog.h"
#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

MPRISPlayerConfigurationUiHandler *MPRISPlayerConfigurationUiHandler::Instance = 0;

void MPRISPlayerConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new MPRISPlayerConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/mprisplayer_mediaplayer.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void MPRISPlayerConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/mprisplayer_mediaplayer.ui"));
		MainConfigurationWindow::unregisterUiHandler(Instance);
		delete Instance;
		Instance = 0;
	}
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
	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox(qApp->translate("@default", "MediaPlayer"), qApp->translate("@default", "General"), qApp->translate("@default", "MPRIS Player"));

	QWidget *options = new QWidget(optionsGroupBox->widget());

	QGridLayout *selectionLayout = new QGridLayout(options);
	QGridLayout *buttonsLayout = new QGridLayout(options);

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

	loadPlayersListFromFile(dataPath(MPRISPlayer::playersListFileName()));
	fillPlayersBox();
	PlayersBox->setCurrentIndex(PlayersBox->findText(config_file.readEntry("MPRISPlayer", "Player")));

	connect(add, SIGNAL(clicked()), this, SLOT(addPlayer()));
	connect(edit, SIGNAL(clicked()), this, SLOT(editPlayer()));
	connect(del, SIGNAL(clicked()), this, SLOT(delPlayer()));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

void MPRISPlayerConfigurationUiHandler::loadPlayersListFromFile(const QString &fileName)
{
	PlainConfigFile PlayersFile(fileName);
	QStringList sections = PlayersFile.getGroupList();
	PlayersMap.clear();

	foreach (const QString &section, sections)
	{
		QString player = PlayersFile.readEntry(section, "player");
		QString service = PlayersFile.readEntry(section, "service");

		if ((!player.isEmpty()) && (!service.isEmpty()))
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

	if ((!newPlayer.isEmpty()) && (!newService.isEmpty()))
	{
		QString oldPlayerName = config_file.readEntry("MPRISPlayer", "Player");
		PlainConfigFile PlayersFile(dataPath(MPRISPlayer::playersListFileName()));

		PlayersFile.writeEntry(newPlayer, "player", newPlayer);
		PlayersFile.writeEntry(newPlayer, "service", newService);
		PlayersFile.sync();

		loadPlayersListFromFile(dataPath(MPRISPlayer::playersListFileName()));
		fillPlayersBox();

		PlayersBox->setCurrentIndex(PlayersBox->findText(oldPlayerName));
	}
}

void MPRISPlayerConfigurationUiHandler::editPlayer()
{
	MPRISPlayerDialog Dialog(true);

	QString oldPlayer = PlayersBox->currentText();
	QString oldService = PlayersMap.value(oldPlayer);

	Dialog.setPlayer(oldPlayer);
	Dialog.setService(oldService);

	if (Dialog.exec() != QDialog::Accepted)
		return;

	QString newPlayer = Dialog.getPlayer();
	QString newService = Dialog.getService();

	if ((!newPlayer.isEmpty()) && (!newService.isEmpty()) && ((newPlayer != oldPlayer) || (oldService != newService)))
	{
		PlainConfigFile PlayersFile(dataPath(MPRISPlayer::playersListFileName()));
		QStringList sections = PlayersFile.getGroupList();

		foreach (const QString &section, sections)
		{
			if (section == oldPlayer)
			{
				PlayersFile.writeEntry(newPlayer, "player", newPlayer);
				PlayersFile.writeEntry(newPlayer, "service", newService);
				break;
			}
		}

		PlayersFile.sync();

		loadPlayersListFromFile(dataPath(MPRISPlayer::playersListFileName()));
		fillPlayersBox();

		PlayersBox->setCurrentIndex(PlayersBox->findText(newPlayer));
	}
}

void MPRISPlayerConfigurationUiHandler::delPlayer()
{
	PlainConfigFile PlayersFile(dataPath(MPRISPlayer::playersListFileName()));
	QStringList sections = PlayersFile.getGroupList();
	QString playerToRemove = PlayersBox->currentText();

	foreach (const QString &section, sections)
	{
		if (section == playerToRemove)
		{
			PlayersFile.writeEntry(section, "player", QString());
			PlayersFile.writeEntry(section, "service", QString());
			break;
		}
	}

	PlayersFile.sync();

	loadPlayersListFromFile(dataPath(MPRISPlayer::playersListFileName()));
	fillPlayersBox();

	PlayersBox->setCurrentIndex(-1);
}

void MPRISPlayerConfigurationUiHandler::configurationApplied()
{
	config_file.writeEntry("MPRISPlayer", "Player", PlayersBox->currentText());
	config_file.writeEntry("MPRISPlayer", "Service", PlayersMap.value(PlayersBox->currentText()));

	MPRISPlayer::instance()->configurationApplied();
}
