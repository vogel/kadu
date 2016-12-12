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

#include "chat-style-configuration-ui-handler.h"

#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-engine.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "misc/memory.h"
#include "widgets/chat-style-preview.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/configuration/config-group-box.h"
#include "windows/main-configuration-window.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSizePolicy>

ChatStyleConfigurationUiHandler::ChatStyleConfigurationUiHandler(QObject *parent) :
		QObject{parent},
		m_compositingEnabled{false},
		m_syntaxListCombo{nullptr},
		m_variantListCombo{nullptr},
		m_turnOnTransparency{nullptr},
		m_enginePreview{nullptr}
{
	triggerCompositingStateChanged();
}

ChatStyleConfigurationUiHandler::~ChatStyleConfigurationUiHandler()
{
}

void ChatStyleConfigurationUiHandler::setChatStyleManager(ChatStyleManager *chatStyleManager)
{
	m_chatStyleManager = chatStyleManager;
}

void ChatStyleConfigurationUiHandler::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ChatStyleConfigurationUiHandler::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void ChatStyleConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	m_chatStyleManager->loadStyles(); // reload styles to allow style testing without application restart

	auto groupBox = mainConfigurationWindow->widget()->configGroupBox("Look", "Chat", "Style");

//editor
	auto editorLabel = new QLabel(QCoreApplication::translate("@default", "Style") + ':');
	editorLabel->setToolTip(QCoreApplication::translate("@default", "Choose style of chat window"));
	auto editor = new QWidget(groupBox->widget());
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	editor->setToolTip(QCoreApplication::translate("@default", "Choose style of chat window"));
	auto editorLayout = new QHBoxLayout(editor);

	m_syntaxListCombo = new QComboBox(editor);
	auto styleNames = m_chatStyleManager->availableStyles().keys();
	qSort(styleNames.begin(), styleNames.end(), [](const QString &s1, const QString &s2){ return s1.toLower() < s2.toLower(); });
	m_syntaxListCombo->addItems(styleNames);
	m_syntaxListCombo->setCurrentIndex(m_syntaxListCombo->findText(m_chatStyleManager->currentChatStyle().name()));
	connect(m_syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(styleChangedSlot(const QString &)));

	editorLayout->addWidget(m_syntaxListCombo, 100);

//preview
	m_enginePreview = m_injectedFactory->makeInjected<ChatStylePreview>();

//variants
	m_variantListCombo = new QComboBox();
	m_variantListCombo->addItems(m_chatStyleManager->currentEngine()->styleVariants(m_chatStyleManager->currentChatStyle().name()));
	auto defaultVariant = m_chatStyleManager->currentEngine()->defaultVariant(m_chatStyleManager->currentChatStyle().name());
	if (!defaultVariant.isEmpty() && m_variantListCombo->findText(defaultVariant) == -1)
		m_variantListCombo->insertItem(0, defaultVariant);

	auto newVariant = m_chatStyleManager->currentChatStyle().variant().isEmpty()
			? defaultVariant
			: m_chatStyleManager->currentChatStyle().variant();
	variantChangedSlot(newVariant);
	m_variantListCombo->setCurrentIndex(m_variantListCombo->findText(newVariant));
	m_variantListCombo->setEnabled(m_chatStyleManager->currentEngine()->supportVariants());
	connect(m_variantListCombo, SIGNAL(activated(const QString &)), this, SLOT(variantChangedSlot(const QString &)));
//
	groupBox->addWidgets(editorLabel, editor);
	groupBox->addWidgets(new QLabel(QCoreApplication::translate("@default", "Style variant") + ':'), m_variantListCombo);
	groupBox->addWidgets(new QLabel(QCoreApplication::translate("@default", "Preview") + ':'), m_enginePreview, Qt::AlignRight | Qt::AlignTop);

	m_turnOnTransparency = static_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("useTransparency"));
	m_turnOnTransparency->setVisible(m_compositingEnabled);
}

void ChatStyleConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	m_syntaxListCombo = 0;
	m_variantListCombo = 0;
	m_turnOnTransparency = 0;
}

void ChatStyleConfigurationUiHandler::mainConfigurationWindowApplied()
{
	m_configuration->deprecatedApi()->writeEntry("Look", "Style", m_syntaxListCombo->currentText());
	m_configuration->deprecatedApi()->writeEntry("Look", "ChatStyleVariant", m_variantListCombo->currentText());
}

void ChatStyleConfigurationUiHandler::compositingEnabled()
{
	m_compositingEnabled = true;
	if (m_turnOnTransparency)
		m_turnOnTransparency->setEnabled(true);
}

void ChatStyleConfigurationUiHandler::compositingDisabled()
{
	m_compositingEnabled = false;
	if (m_turnOnTransparency)
		m_turnOnTransparency->setEnabled(false);
}

void ChatStyleConfigurationUiHandler::variantChangedSlot(const QString &variantName)
{
	auto styleName = m_syntaxListCombo->currentText();
	if (!m_chatStyleManager->isChatStyleValid(styleName))
		return;

	m_enginePreview->setRendererFactory(m_chatStyleManager->availableStyles().value(styleName).engine->createRendererFactory({styleName, variantName}));
}

void ChatStyleConfigurationUiHandler::styleChangedSlot(const QString &styleName)
{
	if (!m_chatStyleManager->availableStyles().contains(styleName))
		return;

	auto engine = m_chatStyleManager->availableStyles().value(styleName).engine;
	m_variantListCombo->clear();
	m_variantListCombo->addItems(engine->styleVariants(styleName));

	QString currentVariant;
	if (m_chatStyleManager->availableStyles().contains(m_syntaxListCombo->currentText()))
		if (m_chatStyleManager->availableStyles().value(m_syntaxListCombo->currentText()).engine)
			currentVariant = m_chatStyleManager->availableStyles().value(m_syntaxListCombo->currentText()).engine->defaultVariant(styleName);
	if (!currentVariant.isEmpty() && m_variantListCombo->findText(currentVariant) == -1)
		m_variantListCombo->insertItem(0, currentVariant);

	m_variantListCombo->setCurrentIndex(m_variantListCombo->findText(currentVariant));
	m_variantListCombo->setEnabled(engine->supportVariants());

	m_enginePreview->setRendererFactory(engine->createRendererFactory({styleName, m_variantListCombo->currentText()}));
	m_turnOnTransparency->setChecked(engine->styleUsesTransparencyByDefault(styleName));
}

#include "moc_chat-style-configuration-ui-handler.cpp"
