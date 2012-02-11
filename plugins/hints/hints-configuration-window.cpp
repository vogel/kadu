/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QPalette>
#include <QtGui/QSpinBox>

#include "configuration/notifier-configuration-data-manager.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-label.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "hints-configuration-window.h"

QMap<QString, HintsConfigurationWindow *> HintsConfigurationWindow::ConfigurationWindows;

HintsConfigurationWindow::HintsConfigurationWindow(const QString &eventName, NotifierConfigurationDataManager *dataManager) :
	ConfigurationWindow("HintEventConfiguration", tr("Hints configuration"), "Hints", dataManager), EventName(eventName)
{
	widget()->appendUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/hints-notifier.ui"));

	widget()->widgetById("syntax")->setToolTip(tr(MainConfigurationWindow::SyntaxTextNotify));
	static_cast<QSpinBox *>(widget()->widgetById("timeout"))->setSpecialValueText(tr("Don't hide"));

	preview = static_cast<ConfigLabel *>(widget()->widgetById("preview"));
	preview->setMargin(3);
	preview->setText(tr("<b>Here</b> you can see the preview"), false);

	connect(static_cast<ConfigSelectFont *>(widget()->widgetById("font")), SIGNAL(fontChanged(QFont)),
			this, SLOT(fontChanged(QFont)));
	connect(static_cast<ConfigColorButton *>(widget()->widgetById("fgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(foregroundColorChanged(const QColor &)));
	connect(static_cast<ConfigColorButton *>(widget()->widgetById("bgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(backgroundColorChanged(const QColor &)));

	dataManager->configurationWindowCreated(this);

	QFont font;
	font.fromString(dataManager->readEntry("Hints", "_font").toString());
	preview->setFont(font);

	QColor bcolor = dataManager->readEntry("Hints", "_bgcolor").value<QColor>();
	QColor fcolor = dataManager->readEntry("Hints", "_fgcolor").value<QColor>();
	QString style = QString("QWidget {color:%1; background-color:%2}").arg(fcolor.name(), bcolor.name());
	preview->setStyleSheet(style);
}

HintsConfigurationWindow::~HintsConfigurationWindow()
{
	HintsConfigurationWindow::windowDestroyed(EventName);
}

void HintsConfigurationWindow::windowDestroyed(const QString &eventName)
{
	ConfigurationWindows.remove(eventName);
}

HintsConfigurationWindow * HintsConfigurationWindow::configWindowForEvent(const QString &eventName)
{
    	if (ConfigurationWindows[eventName])
		return ConfigurationWindows[eventName];
	else
	{
		NotifierConfigurationDataManager *dataManager = NotifierConfigurationDataManager::dataManagerForEvent(eventName);
		return ConfigurationWindows[eventName] = new HintsConfigurationWindow(eventName, dataManager);
	}
}

void HintsConfigurationWindow::fontChanged(QFont font)
{
	preview->setFont(font);
}

void HintsConfigurationWindow::foregroundColorChanged(const QColor &color)
{
	QColor bcolor = preview->palette().color(preview->backgroundRole());
	QString style = QString("QWidget {color:%1; background-color:%2}").arg(color.name(), bcolor.name());
	preview->setStyleSheet(style);
}

void HintsConfigurationWindow::backgroundColorChanged(const QColor &color)
{
	QColor fcolor = preview->palette().color(preview->foregroundRole());
	QString style = QString("QWidget {color:%1; background-color:%2}").arg(fcolor.name(), color.name());
	preview->setStyleSheet(style);
}
