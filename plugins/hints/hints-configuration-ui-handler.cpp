/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include "buddies/buddy-dummy-factory.h"
#include "buddies/buddy-preferred-manager.h"
#include "configuration/config-file-data-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "notification/notification.h"
#include "talkable/talkable.h"

#include "hint-manager.h"

#include "misc/paths-provider.h"
#include "activate.h"

#include "hints-configuration-ui-handler.h"
#include <core/injected-factory.h>

#define FRAME_WIDTH 1
#define BORDER_RADIUS 0

HintsConfigurationUiHandler::HintsConfigurationUiHandler(QObject *parent):
		QObject(parent),
		minimumWidth{},
		maximumWidth{},
		overUserConfigurationPreview{},
		overUserConfigurationTipLabel{}
{
}

HintsConfigurationUiHandler::~HintsConfigurationUiHandler()
{
}

void HintsConfigurationUiHandler::setBuddyDummyFactory(BuddyDummyFactory *buddyDummyFactory)
{
	m_buddyDummyFactory = buddyDummyFactory;
}

void HintsConfigurationUiHandler::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void HintsConfigurationUiHandler::setHintManager(HintManager *hintManager)
{
	m_hintManager = hintManager;
}

void HintsConfigurationUiHandler::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void HintsConfigurationUiHandler::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void HintsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	m_mainConfigurationWindow = mainConfigurationWindow;

	connect(mainConfigurationWindow->widget()->widgetById("hints/showContent"), SIGNAL(toggled(bool)),
	mainConfigurationWindow->widget()->widgetById("hints/showContentCount"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("toolTipClasses"), SIGNAL(currentIndexChanged(const QString &)),
		this, SLOT(toolTipClassesHighlighted(const QString &)));

	ConfigGroupBox *toolTipBox = mainConfigurationWindow->widget()->configGroupBox("Buddies list", "Information", "Tooltip over buddy");

	QWidget *configureHint = new QWidget(toolTipBox->widget());
	overUserConfigurationPreview = new QFrame(configureHint);
	QHBoxLayout *lay = new QHBoxLayout(overUserConfigurationPreview);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	overUserConfigurationTipLabel = new QLabel(overUserConfigurationPreview);
	overUserConfigurationTipLabel->setTextFormat(Qt::RichText);
	overUserConfigurationTipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	overUserConfigurationTipLabel->setContentsMargins(10, 10, 10, 10);

	lay->addWidget(overUserConfigurationTipLabel);

	Buddy example = m_buddyDummyFactory->dummy();

	if (!example.isNull())
		m_hintManager->prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationTipLabel, example);

	lay = new QHBoxLayout(configureHint);
	lay->addWidget(overUserConfigurationPreview);

	toolTipBox->addWidgets(new QLabel(tr("Hint over buddy list: ")), configureHint);

	minimumWidth = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/minimumWidth"));
	maximumWidth = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/maximumWidth"));
	connect(minimumWidth, SIGNAL(valueChanged(int)), this, SLOT(minimumWidthChanged(int)));
	connect(maximumWidth, SIGNAL(valueChanged(int)), this, SLOT(maximumWidthChanged(int)));
}

void HintsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	overUserConfigurationPreview = 0;
}

void HintsConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

void HintsConfigurationUiHandler::toolTipClassesHighlighted(const QString &value)
{
	overUserConfigurationPreview->setEnabled(value == QCoreApplication::translate("@default", "Hints"));
}

void HintsConfigurationUiHandler::updateOverUserPreview()
{
	if (!overUserConfigurationPreview)
		return;

	auto example = m_buddyDummyFactory->dummy();

	if (!example.isNull())
		m_hintManager->prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationTipLabel, example);
}

void HintsConfigurationUiHandler::minimumWidthChanged(int value)
{
	if (value > maximumWidth->value())
		maximumWidth->setValue(value);
}

void HintsConfigurationUiHandler::maximumWidthChanged(int value)
{
	if (value < minimumWidth->value())
		minimumWidth->setValue(value);
}

#include "moc_hints-configuration-ui-handler.cpp"
