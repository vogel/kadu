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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QPalette>
#include <QtWidgets/QPushButton>

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-details-contact.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engines/chat-engine-adium/chat-engine-adium.h"
#include "chat/style-engines/chat-engine-kadu/chat-engine-kadu.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/preview.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "message/message-render-info.h"
#include "misc/kadu-paths.h"
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
		CurrentEngine(0), SyntaxListCombo(0),
		VariantListCombo(0), TurnOnTransparency(0), EnginePreview(0)
{
}

ChatStylesManager::~ChatStylesManager()
{
	unregisterChatStyleEngine("Kadu");
	unregisterChatStyleEngine("Adium");
}

void ChatStylesManager::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void ChatStylesManager::init()
{
	//FIXME:
	KaduEngine = new KaduChatStyleEngine(this);
	registerChatStyleEngine("Kadu", KaduEngine);

	AdiumEngine = new AdiumChatStyleEngine(this);
	AdiumEngine->setMessageHtmlRendererService(Core::instance()->messageHtmlRendererService());
	registerChatStyleEngine("Adium", AdiumEngine);

	loadStyles();
	configurationUpdated();
}

void ChatStylesManager::registerChatStyleEngine(const QString &name, ChatStyleEngine *engine)
{
	if (0 != engine && !RegisteredEngines.contains(name))
		RegisteredEngines.insert(name, engine);
}

void ChatStylesManager::unregisterChatStyleEngine(const QString &name)
{
	if (RegisteredEngines.contains(name))
	{
		delete RegisteredEngines.value(name);
		RegisteredEngines.remove(name);
	}
}

void ChatStylesManager::chatViewCreated(ChatMessagesView *view)
{
	if (0 != view)
	{
		ChatViews.append(view);

		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		CurrentEngine->refreshView(view->renderer(), useTransparency);
	}
}

void ChatStylesManager::chatViewDestroyed(ChatMessagesView *view)
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

	QFont font = Core::instance() && Core::instance()->kaduWindow()
		? QFont(config_file.readFontEntry("Look", "ChatFont"), Core::instance()->kaduWindow())
		: config_file.readFontEntry("Look", "ChatFont");

	QString fontFamily = font.family();
	QString fontSize = QString::number(QFontMetrics(font).ascent()) + "px";
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

	QString newStyleName = config_file.readEntry("Look", "Style");
	QString newVariantName = config_file.readEntry("Look", "ChatStyleVariant");
	// if Style was changed, load new Style
	if (!CurrentEngine || CurrentEngine->currentStyleName() != newStyleName || CurrentEngine->currentStyleVariant() != newVariantName)
	{
		newStyleName = fixedStyleName(newStyleName);
		CurrentEngine = AvailableStyles.value(newStyleName).engine;
		newVariantName = fixedVariantName(newStyleName, newVariantName);

		CurrentEngine->loadStyle(newStyleName, newVariantName);
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
	foreach (ChatMessagesView *view, ChatViews)
	{
		view->updateBackgroundsAndColors();

		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		CurrentEngine->refreshView(view->renderer(), useTransparency);
	}

	if (TurnOnTransparency)
		TurnOnTransparency->setEnabled(true);
}

void ChatStylesManager::compositingDisabled()
{
	CompositingEnabled = false;
	foreach (ChatMessagesView *view, ChatViews)
	{
		view->updateBackgroundsAndColors();

		bool useTransparency = view->supportTransparency() & ChatConfigurationHolder::instance()->useTransparency();
		CurrentEngine->refreshView(view->renderer(), useTransparency);
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
			foreach (ChatStyleEngine *engine, RegisteredEngines)
			{
				StyleName = engine->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine;
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
			foreach (ChatStyleEngine *engine, RegisteredEngines)
			{
				StyleName = engine->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine;
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
	SyntaxListCombo->setCurrentIndex(SyntaxListCombo->findText(CurrentEngine->currentStyleName()));
	connect(SyntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(styleChangedSlot(const QString &)));

	editorLayout->addWidget(SyntaxListCombo, 100);
//preview
	EnginePreview = new Preview();

	preparePreview(EnginePreview);
//variants
	VariantListCombo = new QComboBox();
	VariantListCombo->addItems(CurrentEngine->styleVariants(CurrentEngine->currentStyleName()));
	QString defaultVariant = CurrentEngine->defaultVariant(CurrentEngine->currentStyleName());
	if (!defaultVariant.isEmpty() && VariantListCombo->findText(defaultVariant) == -1)
		VariantListCombo->insertItem(0, defaultVariant);

	QString newVariant = CurrentEngine->currentStyleVariant().isEmpty()
			? defaultVariant
			: CurrentEngine->currentStyleVariant();
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

void ChatStylesManager::preparePreview(Preview *preview)
{
	if (!CurrentFormattedStringFactory)
		return;

	Buddy example = Buddy::dummy();
	if (example.isNull())
		return;

	Chat chat = Chat::create();
	chat.setChatAccount(BuddyPreferredManager::instance()->preferredAccount(example));
	chat.setType("Contact");

	ChatDetailsContact *details = dynamic_cast<ChatDetailsContact *>(chat.details());
	details->setState(StorableObject::StateNew);
	details->setContact(BuddyPreferredManager::instance()->preferredContact(example));

	Message sentMessage = Message::create();
	sentMessage.setMessageChat(chat);
	sentMessage.setType(MessageTypeSent);
	sentMessage.setMessageSender(chat.chatAccount().accountContact());
	sentMessage.setContent(CurrentFormattedStringFactory.data()->fromPlainText(tr("Your message")));
	sentMessage.setReceiveDate(QDateTime::currentDateTime());
	sentMessage.setSendDate(QDateTime::currentDateTime());

	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(sentMessage);
	messageRenderInfo->setSeparatorSize(CfgHeaderSeparatorHeight);
	preview->addMessage(messageRenderInfo);

	Message receivedMessage = Message::create();
	receivedMessage.setMessageChat(chat);
	receivedMessage.setType(MessageTypeReceived);
	receivedMessage.setMessageSender(BuddyPreferredManager::instance()->preferredContact(example));
	receivedMessage.setContent(CurrentFormattedStringFactory.data()->fromPlainText(tr("Message from Your friend")));
	receivedMessage.setReceiveDate(QDateTime::currentDateTime());
	receivedMessage.setSendDate(QDateTime::currentDateTime());

	messageRenderInfo = new MessageRenderInfo(receivedMessage);
	messageRenderInfo->setSeparatorSize(CfgHeaderSeparatorHeight);
	preview->addMessage(messageRenderInfo);
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
	engine->prepareStylePreview(EnginePreview, styleName, VariantListCombo->currentText());
	TurnOnTransparency->setChecked(engine->styleUsesTransparencyByDefault(styleName));

	emit previewSyntaxChanged(styleName);
}

void ChatStylesManager::variantChangedSlot(const QString &variantName)
{
	QString styleName = SyntaxListCombo->currentText();
	if (!AvailableStyles.contains(styleName) || !AvailableStyles.value(styleName).engine)
		return;

	AvailableStyles.value(styleName).engine->prepareStylePreview(EnginePreview, styleName, variantName);
}

void ChatStylesManager::syntaxUpdated(const QString &syntaxName)
{
	if (!AvailableStyles.contains(syntaxName))
		return;

	if (SyntaxListCombo && SyntaxListCombo->currentText() == syntaxName)
		styleChangedSlot(syntaxName);

	if (CurrentEngine->currentStyleName() == syntaxName)
		CurrentEngine->loadStyle(syntaxName, VariantListCombo->currentText());
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
