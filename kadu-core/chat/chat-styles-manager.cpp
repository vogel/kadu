/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPalette>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-details-contact.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engine/adium-style-engine/adium-style-engine.h"
#include "chat/style-engine/configured-chat-messages-renderer-provider.h"
#include "chat/style-engine/kadu-style-engine/kadu-style-engine.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-style-preview.h"
#include "gui/widgets/webkit-messages-view.h"
#include "gui/windows/message-dialog.h"
#include "misc/algorithm.h"
#include "misc/kadu-paths.h"
#include "misc/memory.h"
#include "protocols/protocols-manager.h"

#include "chat-styles-manager.h"

static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
	return s1.toLower() < s2.toLower();
}

ChatStylesManager * ChatStylesManager::Instance = 0;

ChatStylesManager * ChatStylesManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatStylesManager();
		Instance->init();
	}

	return Instance;
}

ChatStylesManager::ChatStylesManager() :
		CurrentEngine{},
		CompositingEnabled{}, CfgNoHeaderRepeat{}, CfgHeaderSeparatorHeight{},
		CfgNoHeaderInterval{}, ParagraphSeparator{}, Prune{}, NoServerTime{},
		NoServerTimeDiff{}, SyntaxListCombo{},
		VariantListCombo{}, TurnOnTransparency{}, EnginePreview{}
{
}

ChatStylesManager::~ChatStylesManager()
{
	unregisterChatStyleEngine("Kadu");
	unregisterChatStyleEngine("Adium");
}

void ChatStylesManager::setConfiguredChatMessagesRendererProvider(ConfiguredChatMessagesRendererProvider *configuredChatMessagesRendererProvider)
{
	CurrentConfiguredChatMessagesRendererProvider = configuredChatMessagesRendererProvider;
	configurationUpdated();
}

void ChatStylesManager::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void ChatStylesManager::init()
{
	registerChatStyleEngine("Kadu", make_unique<KaduStyleEngine>());

	auto adiumStyleEngine = make_unique<AdiumStyleEngine>();
	adiumStyleEngine.get()->setMessageHtmlRendererService(Core::instance()->messageHtmlRendererService());
	registerChatStyleEngine("Adium", std::move(adiumStyleEngine));

	loadStyles();
}

void ChatStylesManager::registerChatStyleEngine(const QString &name, std::unique_ptr<ChatStyleEngine> engine)
{
	if (engine && !contains(RegisteredEngines, name))
		RegisteredEngines.insert(std::make_pair(name, std::move(engine)));
}

void ChatStylesManager::unregisterChatStyleEngine(const QString &name)
{
	RegisteredEngines.erase(name);
}

void ChatStylesManager::chatViewCreated(WebkitMessagesView *view)
{
	if (0 != view)
	{
		ChatViews.append(view);

		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->refreshView(view->renderer(), useTransparency);
	}
}

void ChatStylesManager::chatViewDestroyed(WebkitMessagesView *view)
{
	if (ChatViews.contains(view))
		ChatViews.removeAll(view);
}

void ChatStylesManager::configurationUpdated()
{
	if (config_file.readBoolEntry("Chat", "ChatPrune"))
		Prune = config_file.readNumEntry("Chat", "ChatPruneLen");
	else
		Prune = 0;

	ParagraphSeparator = config_file.readNumEntry("Look", "ParagraphSeparator");

	QFont font = config_file.readFontEntry("Look","ChatFont");

	QString fontFamily = font.family();
	QString fontSize;
	if (font.pointSize() > 0)
#ifdef Q_OS_MAC
		/*  Dorr: On MacOSX this font is being displayed 3pts larger than
		 *  it really is, so reduce it's size to be coherent in entire
		 *  application.
		 */
		fontSize = QString::number(font.pointSize()-3) + "pt";
#else
		fontSize = QString::number(font.pointSize()) + "pt";
#endif
	else
		fontSize = QString::number(font.pixelSize()) + "px";
	QString fontStyle = font.italic() ? "italic" : "normal";
	QString fontWeight = font.bold() ? "bold" : "normal";
	QString textDecoration = font.underline() ? "underline" : "none";
	QString backgroundColor = "transparent";
	if (ChatConfigurationHolder::instance()->chatBgFilled())
		backgroundColor = ChatConfigurationHolder::instance()->chatBgColor().name();

	MainStyle = QString(
		"html {"
		"	font: %1 %2 %3 %4;"
		"	text-decoration: %5;"
		"	word-wrap: break-word;"
		"}"
		"a {"
		"	text-decoration: underline;"
		"}"
		"body {"
		"	margin: %6;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"p {"
		"	margin: 0;"
		"	padding: 3px;"
		"}").arg(fontStyle, fontWeight, fontSize, fontFamily, textDecoration, QString::number(ParagraphSeparator), backgroundColor);

	CfgNoHeaderRepeat = config_file.readBoolEntry("Look", "NoHeaderRepeat", true);

	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
		CfgHeaderSeparatorHeight = config_file.readNumEntry("Look", "HeaderSeparatorHeight");
		CfgNoHeaderInterval = config_file.readNumEntry("Look", "NoHeaderInterval");
	}
	else
	{
		CfgHeaderSeparatorHeight = 0;
		CfgNoHeaderInterval = 0;
	}

	NoServerTime = config_file.readBoolEntry("Look", "NoServerTime");
	NoServerTimeDiff = config_file.readNumEntry("Look", "NoServerTimeDiff");

	auto newChatStyle = ChatStyle{config_file.readEntry("Look", "Style"), config_file.readEntry("Look", "ChatStyleVariant")};

	// if Style was changed, load new Style
	if (!CurrentEngine || newChatStyle != m_currentChatStyle)
	{
		auto newStyleName = fixedStyleName(newChatStyle.name());
		CurrentEngine = AvailableStyles.value(newStyleName).engine;
		auto newVariantName = fixedVariantName(newStyleName, newChatStyle.variant());
		m_currentChatStyle = {newStyleName, newVariantName};

		Core::instance()->configuredChatMessagesRendererProvider()->setChatMessagesRenderer(CurrentEngine->createRenderer(newStyleName, newVariantName));
	}
	else
		CurrentEngine->configurationUpdated();

	triggerCompositingStateChanged();
}

QString ChatStylesManager::fixedStyleName(QString styleName)
{
	if (!AvailableStyles.contains(styleName))
	{
		styleName = "Satin";
		if (!AvailableStyles.contains(styleName))
		{
			styleName = "kadu";
			if (!AvailableStyles.contains(styleName))
				styleName = *AvailableStyles.keys().constBegin();
		}
	}

	return styleName;
}

QString ChatStylesManager::fixedVariantName(const QString &styleName, QString variantName)
{
	if (!CurrentEngine->styleVariants(styleName).contains(variantName))
		return CurrentEngine->defaultVariant(styleName);

	return variantName;
}

void ChatStylesManager::compositingEnabled()
{
	CompositingEnabled = true;
	foreach (WebkitMessagesView *view, ChatViews)
	{
		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->refreshView(view->renderer(), useTransparency);
	}

	if (TurnOnTransparency)
		TurnOnTransparency->setEnabled(true);
}

void ChatStylesManager::compositingDisabled()
{
	CompositingEnabled = false;
	foreach (WebkitMessagesView *view, ChatViews)
	{
		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->refreshView(view->renderer(), useTransparency);
	}

	if (TurnOnTransparency)
		TurnOnTransparency->setEnabled(false);
}

//any better ideas?
void ChatStylesManager::loadStyles()
{
	QDir dir;
	QString path, StyleName;
	QFileInfo fi;
	QStringList files;

	path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/");
	dir.setPath(path);

	files = dir.entryList();

	AvailableStyles.clear(); // allow reloading of styles

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !AvailableStyles.contains(file))
		{
			for (auto &&engine : RegisteredEngines)
			{
				StyleName = engine.second->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine.second.get();
					AvailableStyles[StyleName].global = false;
					break;
				}
			}
		}
	}

	path = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/");
	dir.setPath(path);

	files = dir.entryList();

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !AvailableStyles.contains(file))
		{
			for (auto &&engine : RegisteredEngines)
			{
				StyleName = engine.second->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine.second.get();
					AvailableStyles[StyleName].global = true;
					break;
				}
			}
		}
	}
}

void ChatStylesManager::mainConfigurationWindowCreated(MainConfigurationWindow *window)
{
	connect(window, SIGNAL(destroyed()), this, SLOT(configurationWindowDestroyed()));
	connect(window, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));

	loadStyles(); // reload styles to allow style testing without application restart

	ConfigGroupBox *groupBox = window->widget()->configGroupBox("Look", "Chat", "Style");
//editor
	QLabel *editorLabel = new QLabel(QCoreApplication::translate("@default", "Style") + ':');
	editorLabel->setToolTip(QCoreApplication::translate("@default", "Choose style of chat window"));

	QWidget  *editor = new QWidget(groupBox->widget());
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	editor->setToolTip(QCoreApplication::translate("@default", "Choose style of chat window"));
	QHBoxLayout *editorLayout = new QHBoxLayout(editor);

	SyntaxListCombo = new QComboBox(editor);
	QStringList styleNames = AvailableStyles.keys();
	qSort(styleNames.begin(), styleNames.end(), caseInsensitiveLessThan);
	SyntaxListCombo->addItems(styleNames);
	SyntaxListCombo->setCurrentIndex(SyntaxListCombo->findText(m_currentChatStyle.name()));
	connect(SyntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(styleChangedSlot(const QString &)));

	editorLayout->addWidget(SyntaxListCombo, 100);
//preview
	EnginePreview = new ChatStylePreview();

//variants
	VariantListCombo = new QComboBox();
	VariantListCombo->addItems(CurrentEngine->styleVariants(m_currentChatStyle.name()));
	QString defaultVariant = CurrentEngine->defaultVariant(m_currentChatStyle.name());
	if (!defaultVariant.isEmpty() && VariantListCombo->findText(defaultVariant) == -1)
		VariantListCombo->insertItem(0, defaultVariant);

	QString newVariant = m_currentChatStyle.variant().isEmpty()
			? defaultVariant
			: m_currentChatStyle.variant();
	variantChangedSlot(newVariant);
	VariantListCombo->setCurrentIndex(VariantListCombo->findText(newVariant));
	VariantListCombo->setEnabled(CurrentEngine->supportVariants());
	connect(VariantListCombo, SIGNAL(activated(const QString &)), this, SLOT(variantChangedSlot(const QString &)));
//
	groupBox->addWidgets(editorLabel, editor);
	groupBox->addWidgets(new QLabel(QCoreApplication::translate("@default", "Style variant") + ':'), VariantListCombo);
	groupBox->addWidgets(new QLabel(QCoreApplication::translate("@default", "Preview") + ':'), EnginePreview, Qt::AlignRight | Qt::AlignTop);

	TurnOnTransparency = static_cast<QCheckBox *>(window->widget()->widgetById("useTransparency"));
	TurnOnTransparency->setEnabled(CompositingEnabled);

	QLabel *help = static_cast<QLabel *>(window->widget()->widgetById("useTransparencyHelp"));
	help->setVisible(!CompositingEnabled);
}

void ChatStylesManager::configurationApplied()
{
	config_file.writeEntry("Look", "Style", SyntaxListCombo->currentText());
	config_file.writeEntry("Look", "ChatStyleVariant", VariantListCombo->currentText());
}

void ChatStylesManager::styleChangedSlot(const QString &styleName)
{
	if (!AvailableStyles.contains(styleName))
		return;

	ChatStyleEngine *engine = AvailableStyles.value(styleName).engine;
	VariantListCombo->clear();
	VariantListCombo->addItems(engine->styleVariants(styleName));

	QString currentVariant;
	if (AvailableStyles.contains(SyntaxListCombo->currentText()))
		if (AvailableStyles.value(SyntaxListCombo->currentText()).engine)
			currentVariant = AvailableStyles.value(SyntaxListCombo->currentText()).engine->defaultVariant(styleName);
	if (!currentVariant.isEmpty() && VariantListCombo->findText(currentVariant) == -1)
		VariantListCombo->insertItem(0, currentVariant);

	VariantListCombo->setCurrentIndex(VariantListCombo->findText(currentVariant));
	VariantListCombo->setEnabled(engine->supportVariants());

	EnginePreview->setRenderer(engine->createRenderer(styleName, VariantListCombo->currentText()));
	TurnOnTransparency->setChecked(engine->styleUsesTransparencyByDefault(styleName));
}

void ChatStylesManager::variantChangedSlot(const QString &variantName)
{
	QString styleName = SyntaxListCombo->currentText();
	if (!AvailableStyles.contains(styleName) || !AvailableStyles.value(styleName).engine)
		return;

	EnginePreview->setRenderer(AvailableStyles.value(styleName).engine->createRenderer(styleName, variantName));
}

void ChatStylesManager::addStyle(const QString &syntaxName, ChatStyleEngine *engine)
{
	if (AvailableStyles.contains(syntaxName))
		return;

	AvailableStyles[syntaxName].engine = engine;
	AvailableStyles[syntaxName].global = false;

	if (SyntaxListCombo)
		SyntaxListCombo->addItem(syntaxName);
}

StyleInfo ChatStylesManager::chatStyleInfo(const QString &name)
{
	if (AvailableStyles.contains(name))
		return AvailableStyles.value(name);
	else
		return StyleInfo();
}

void ChatStylesManager::configurationWindowDestroyed()
{
	SyntaxListCombo = 0;
	VariantListCombo = 0;
	TurnOnTransparency = 0;
}

#include "moc_chat-styles-manager.cpp"
