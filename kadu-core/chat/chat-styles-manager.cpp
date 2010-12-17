/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPalette>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-details-simple.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engines/chat-engine-adium/chat-engine-adium.h"
#include "chat/style-engines/chat-engine-kadu/chat-engine-kadu.h"
#include "chat/message/message-render-info.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "emoticons/emoticons-manager.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/preview.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "protocols/protocols-manager.h"

#include "chat-styles-manager.h"

ChatStylesManager * ChatStylesManager::Instance = 0;

ChatStylesManager * ChatStylesManager::instance()
{
	if (0 == Instance)
		Instance = new ChatStylesManager();

	return Instance;
}

ChatStylesManager::ChatStylesManager() :
		CurrentEngine(0), SyntaxListCombo(0), EditButton(0), DeleteButton(0),
		VariantListCombo(0), TurnOnTransparency(0), EnginePreview(0)
{
	//FIXME:
	KaduEngine = new KaduChatStyleEngine(this);
	registerChatStyleEngine("Kadu", KaduEngine);

	AdiumEngine = new AdiumChatStyleEngine(this);
	registerChatStyleEngine("Adium", AdiumEngine);

	loadStyles();
	configurationUpdated();
}

ChatStylesManager::~ChatStylesManager()
{
	unregisterChatStyleEngine("Kadu");
	unregisterChatStyleEngine("Adium");
}

void ChatStylesManager::registerChatStyleEngine(const QString &name, ChatStyleEngine *engine)
{
	if (0 != engine && !RegisteredEngines.contains(name))
		RegisteredEngines[name] = engine;
}

void ChatStylesManager::unregisterChatStyleEngine(const QString &name)
{
	if (RegisteredEngines.contains(name))
	{
		delete RegisteredEngines[name];
		RegisteredEngines.remove(name);
	}
}

void ChatStylesManager::chatViewCreated(ChatMessagesView *view)
{
	if (0 != view)
	{
		ChatViews.append(view);

		bool useTransparency = view->supportTransparency() && CompositingEnabled && config_file.readBoolEntry("Chat", "UseTransparency", false);
		if (useTransparency)
		{
			QPalette palette = view->renderer()->webPage()->palette();
			palette.setBrush(QPalette::Base, Qt::transparent);
			view->renderer()->webPage()->setPalette(palette);
		}
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
	EmoticonsManager::instance()->configurationUpdated();
	if (config_file.readBoolEntry("Chat", "ChatPrune"))
		Prune = config_file.readUnsignedNumEntry("Chat", "ChatPruneLen");
	else
		Prune = 0;

	ParagraphSeparator = config_file.readUnsignedNumEntry("Look", "ParagraphSeparator");

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
	QString backgroundColor = config_file.readColorEntry("Look", "ChatBgColor").name();

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

	CfgNoHeaderRepeat = config_file.readBoolEntry("Look", "NoHeaderRepeat");

	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
		CfgHeaderSeparatorHeight = config_file.readUnsignedNumEntry("Look", "HeaderSeparatorHeight");
		CfgNoHeaderInterval = config_file.readUnsignedNumEntry("Look", "NoHeaderInterval");
	}
	else
	{
		CfgHeaderSeparatorHeight = 0;
		CfgNoHeaderInterval = 0;
	}

	NoServerTime = config_file.readBoolEntry("Look", "NoServerTime");
	NoServerTimeDiff = config_file.readUnsignedNumEntry("Look", "NoServerTimeDiff");

	QString newStyleName = config_file.readEntry("Look", "Style");
	QString newVariantName = config_file.readEntry("Look", "ChatStyleVariant");
	// if Style was changed, load new Style
	if (!CurrentEngine || CurrentEngine->currentStyleName() != newStyleName || CurrentEngine->currentStyleVariant() != newVariantName)
	{
		newStyleName = fixedStyleName(newStyleName);
		CurrentEngine = AvailableStyles[newStyleName].engine;
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

		if (!view->supportTransparency())
		{
			CurrentEngine->refreshView(view->renderer());
			continue;
		}

		QPalette palette = view->renderer()->webPage()->palette();

		bool useTransparency = config_file.readBoolEntry("Chat", "UseTransparency", false);
		if (useTransparency)
			palette.setBrush(QPalette::Base, Qt::transparent);
		else
			palette.setBrush(QPalette::Base, config_file.readColorEntry("Look", "ChatBgColor"));

		view->renderer()->webPage()->setPalette(palette);

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

		if (!view->supportTransparency())
			continue;

		QPalette palette = view->renderer()->webPage()->palette();
		palette.setBrush(QPalette::Base, config_file.readColorEntry("Look", "ChatBgColor"));

		view->renderer()->webPage()->setPalette(palette);
		CurrentEngine->refreshView(view->renderer());
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

	path = profilePath() + "syntax/chat/";
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

	path = dataPath("kadu") + "/syntax/chat/";
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

	ConfigGroupBox *groupBox = window->widget()->configGroupBox("Look", "Chat Window", "Style");
//editor
	QLabel *editorLabel = new QLabel(qApp->translate("@default", "Style") + ':');
	editorLabel->setToolTip(qApp->translate("@default", "Choose style of chat window"));

	QWidget  *editor = new QWidget(groupBox->widget());
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	editor->setToolTip(qApp->translate("@default", "Choose style of chat window"));
	QHBoxLayout *editorLayout = new QHBoxLayout(editor);

	SyntaxListCombo = new QComboBox(editor);
	SyntaxListCombo->addItems(AvailableStyles.keys());
	SyntaxListCombo->setCurrentIndex(SyntaxListCombo->findText(CurrentEngine->currentStyleName()));
	connect(SyntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(styleChangedSlot(const QString &)));

	EditButton = new QPushButton(tr("Edit"), editor);
	EditButton->setEnabled(CurrentEngine->supportEditing());

	DeleteButton = new QPushButton(tr("Delete"), editor);
	DeleteButton->setEnabled(!AvailableStyles[CurrentEngine->currentStyleName()].global);
	connect(EditButton, SIGNAL(clicked()), this, SLOT(editStyleClicked()));
	connect(DeleteButton, SIGNAL(clicked()), this, SLOT(deleteStyleClicked()));

	editorLayout->addWidget(SyntaxListCombo, 100);
	editorLayout->addWidget(EditButton);
	editorLayout->addWidget(DeleteButton);
//preview
	EnginePreview = new Preview();

	preparePreview(EnginePreview);
//variants
	VariantListCombo = new QComboBox();
	VariantListCombo->addItems(CurrentEngine->styleVariants(CurrentEngine->currentStyleName()));
	QString defaultVariant = CurrentEngine->defaultVariant(CurrentEngine->currentStyleName());
	if (!defaultVariant.isEmpty() && VariantListCombo->findText(defaultVariant) == -1)
		VariantListCombo->insertItem(0, defaultVariant);

	QString newVariant = CurrentEngine->currentStyleVariant().isNull()
			? defaultVariant
			: CurrentEngine->currentStyleVariant();
	variantChangedSlot(newVariant);
	VariantListCombo->setCurrentIndex(VariantListCombo->findText(newVariant));
	VariantListCombo->setEnabled(CurrentEngine->supportVariants());
	connect(VariantListCombo, SIGNAL(activated(const QString &)), this, SLOT(variantChangedSlot(const QString &)));
//
	groupBox->addWidgets(editorLabel, editor);
	groupBox->addWidgets(new QLabel(qApp->translate("@default", "Style variant") + ':'), VariantListCombo);
	groupBox->addWidgets(new QLabel(qApp->translate("@default", "Preview") + ':'), EnginePreview);

	TurnOnTransparency = dynamic_cast<QCheckBox *>(window->widget()->widgetById("useTransparency"));
	TurnOnTransparency->setEnabled(CompositingEnabled);
}

void ChatStylesManager::configurationApplied()
{
	config_file.writeEntry("Look", "Style", SyntaxListCombo->currentText());
	config_file.writeEntry("Look", "ChatStyleVariant", VariantListCombo->currentText());
}

void ChatStylesManager::preparePreview(Preview *preview)
{
	Buddy example = Buddy::dummy();
	if (example.isNull())
		return;

	Chat chat = Chat::create();
	chat.setChatAccount(BuddyPreferredManager::instance()->preferredAccount(example));
	ChatDetailsSimple *details = new ChatDetailsSimple(chat);
	details->setState(StorableObject::StateNew);
	details->setContact(BuddyPreferredManager::instance()->preferredContact(example));
	chat.setDetails(details);

	connect(preview, SIGNAL(destroyed(QObject *)), chat, SLOT(deleteLater()));

	Message messageSent = Message::create();
	messageSent.setMessageChat(chat);
	messageSent.setType(Message::TypeSent);
	messageSent.setMessageSender(chat.chatAccount().accountContact());
	messageSent.setContent(tr("Your message"));
	messageSent.setReceiveDate(QDateTime::currentDateTime());
	messageSent.setSendDate(QDateTime::currentDateTime());

	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(messageSent);
	messageRenderInfo->setSeparatorSize(CfgHeaderSeparatorHeight);
	preview->addObjectToParse(BuddyPreferredManager::instance()->preferredContact(Core::instance()->myself()), messageRenderInfo);

	Message messageReceived = Message::create();
	messageReceived.setMessageChat(chat);
	messageReceived.setType(Message::TypeReceived);
	messageReceived.setMessageSender(BuddyPreferredManager::instance()->preferredContact(example));
	messageReceived.setContent(tr("Message from Your friend"));
	messageReceived.setReceiveDate(QDateTime::currentDateTime());
	messageReceived.setSendDate(QDateTime::currentDateTime());

	messageRenderInfo = new MessageRenderInfo(messageReceived);
	messageRenderInfo->setSeparatorSize(CfgHeaderSeparatorHeight);
	preview->addObjectToParse(BuddyPreferredManager::instance()->preferredContact(example), messageRenderInfo);
}

void ChatStylesManager::styleChangedSlot(const QString &styleName)
{
	ChatStyleEngine *engine = AvailableStyles[styleName].engine;
	EditButton->setEnabled(engine->supportEditing());
	DeleteButton->setEnabled(!AvailableStyles[styleName].global);
	VariantListCombo->clear();
	VariantListCombo->addItems(engine->styleVariants(styleName));

	QString currentVariant = AvailableStyles[SyntaxListCombo->currentText()].engine->defaultVariant(styleName);
	if (!currentVariant.isEmpty() && VariantListCombo->findText(currentVariant) == -1)
		VariantListCombo->insertItem(0, currentVariant);

	VariantListCombo->setCurrentIndex(VariantListCombo->findText(currentVariant));

	VariantListCombo->setEnabled(engine->supportVariants());
	engine->prepareStylePreview(EnginePreview, styleName, VariantListCombo->currentText());
	TurnOnTransparency->setChecked(engine->styleUsesTransparencyByDefault(styleName));
}

void ChatStylesManager::variantChangedSlot(const QString &variantName)
{
	AvailableStyles[SyntaxListCombo->currentText()].engine->prepareStylePreview(EnginePreview, SyntaxListCombo->currentText(), variantName);
}

void ChatStylesManager::editStyleClicked()
{
	AvailableStyles[SyntaxListCombo->currentText()].engine->styleEditionRequested(SyntaxListCombo->currentText());
}

void ChatStylesManager::deleteStyleClicked()
{
	QString styleName = SyntaxListCombo->currentText();
	if (AvailableStyles[styleName].engine->removeStyle(styleName))
	{
		AvailableStyles.remove(styleName);
		SyntaxListCombo->removeItem(SyntaxListCombo->currentIndex());
		styleChangedSlot(*AvailableStyles.keys().constBegin());
	}
	else
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Unable to remove style: %1").arg(styleName));
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

void ChatStylesManager::configurationWindowDestroyed()
{
	SyntaxListCombo = 0;
	EditButton = 0;
	DeleteButton = 0;
	VariantListCombo = 0;
	TurnOnTransparency = 0;
}
