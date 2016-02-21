/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "configuration/gui/configuration-ui-handler.h"
#include "gui/windows/configuration-window.h"

#include "hint-manager.h"

class BuddyDummyFactory;
class Configuration;
class HintsManager;
class InjectedFactory;
class PathsProvider;

class QCheckBox;
class QComboBox;
class QSpinBox;

class HintsConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

private:
	QPointer<BuddyDummyFactory> m_buddyDummyFactory;
	QPointer<Configuration> m_configuration;
	QPointer<HintManager> m_hintManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MainConfigurationWindow> m_mainConfigurationWindow;
	QPointer<PathsProvider> m_pathsProvider;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	
	QFrame *overUserConfigurationPreview;
	QLabel *overUserConfigurationTipLabel;

private slots:
	INJEQT_SET void setBuddyDummyFactory(BuddyDummyFactory *buddyDummyFactory);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setHintManager(HintManager *hintManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);
	
	void toolTipClassesHighlighted(const QString &value);

	void updateOverUserPreview();

public:
	Q_INVOKABLE explicit HintsConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~HintsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

};
