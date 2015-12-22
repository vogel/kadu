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

#pragma once

#include "configuration/gui/configuration-ui-handler.h"
#include "os/generic/compositing-aware-object.h"

#include <injeqt/injeqt.h>

class ChatStyleManager;
class ChatStylePreview;

class QCheckBox;
class QComboBox;

class ChatStyleConfigurationUiHandler : public QObject, CompositingAwareObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatStyleConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~ChatStyleConfigurationUiHandler();

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private:
	bool m_compositingEnabled;

	ChatStyleManager *m_chatStyleManager;

	QComboBox *m_syntaxListCombo;
	QComboBox *m_variantListCombo;
	QCheckBox *m_turnOnTransparency;
	ChatStylePreview *m_enginePreview;

private slots:
	INJEQT_SETTER void setChatStyleManager(ChatStyleManager *chatStyleManager);

	void styleChangedSlot(const QString &styleName);
	void variantChangedSlot(const QString &variantName);

};
