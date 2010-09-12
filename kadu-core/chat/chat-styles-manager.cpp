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
#include "chat/chat-details-simple.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engines/chat-engine-adium/chat-engine-adium.h"
#include "chat/style-engines/chat-engine-kadu/chat-engine-kadu.h"
#include "chat/message/message-render-info.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
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

ChatStylesManager::ChatStylesManager() : CurrentEngine(0), kaduEngine(0), turnOnTransparency(0)
{
	//FIXME:
	kaduEngine = new KaduChatStyleEngine();
	registerChatStyleEngine("Kadu", kaduEngine);

	adiumEngine = new AdiumChatStyleEngine();
	registerChatStyleEngine("Adium", adiumEngine);

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
	if (0 != engine && !registeredEngines.contains(name))
		registeredEngines[name] = engine;
}

void ChatStylesManager::unregisterChatStyleEngine(const QString &name)
{
	if (registeredEngines.contains(name))
	{
		delete registeredEngines[name];
		registeredEngines.remove(name);
	}
}

void ChatStylesManager::chatViewCreated(ChatMessagesView *view)
{
	if (0 != view)
	{
		chatViews.append(view);

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
	if (chatViews.contains(view))
		chatViews.removeAll(view);
}

void ChatStylesManager::configurationUpdated()
{
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
		if (!availableStyles.contains(newStyleName))// if Style not exists load kadu Style
			newStyleName = "kadu";
		if (availableStyles[newStyleName].engine != CurrentEngine)
			CurrentEngine = availableStyles[newStyleName].engine;
		CurrentEngine->loadStyle(newStyleName, newVariantName);
	}
	else
		CurrentEngine->configurationUpdated();

	triggerCompositingStateChanged();
}

void ChatStylesManager::compositingEnabled()
{
	CompositingEnabled = true;
	foreach (ChatMessagesView *view, chatViews)
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

	if (turnOnTransparency)
		turnOnTransparency->setEnabled(true);
}

void ChatStylesManager::compositingDisabled()
{
	CompositingEnabled = false;
	foreach (ChatMessagesView *view, chatViews)
	{
		view->updateBackgroundsAndColors();

		if (!view->supportTransparency())
			continue;

		QPalette palette = view->renderer()->webPage()->palette();
		palette.setBrush(QPalette::Base, config_file.readColorEntry("Look", "ChatBgColor"));

		view->renderer()->webPage()->setPalette(palette);
		CurrentEngine->refreshView(view->renderer());
	}

	if (turnOnTransparency)
		turnOnTransparency->setEnabled(false);
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

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !availableStyles.contains(file))
		{
			foreach (ChatStyleEngine *engine, registeredEngines.values())
			{
				if ((StyleName = engine->isStyleValid(path + file)) != QString::null)
				{
					availableStyles[StyleName].engine = engine;
					availableStyles[StyleName].global = false;
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
		if (fi.isReadable() && !availableStyles.contains(file))
		{
			foreach (ChatStyleEngine *engine, registeredEngines.values())
			{
				if ((StyleName = engine->isStyleValid(path + file)) != QString::null)
				{
					availableStyles[StyleName].engine = engine;
					availableStyles[StyleName].global = true;
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

	ConfigGroupBox *groupBox = window->widget()->configGroupBox("Look", "Chat Window", "Style");
//editor
	QLabel *editorLabel = new QLabel(qApp->translate("@default", "Style") + ":");
	editorLabel->setToolTip(qApp->translate("@default", "Choose style of chat window"));

	QWidget  *editor = new QWidget(groupBox->widget());
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	editor->setToolTip(qApp->translate("@default", "Choose style of chat window"));
	QHBoxLayout *editorLayout = new QHBoxLayout(editor);

	syntaxListCombo = new QComboBox(editor);
	syntaxListCombo->addItems(availableStyles.keys());
	syntaxListCombo->setCurrentIndex(syntaxListCombo->findText(CurrentEngine->currentStyleName()));
	connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(styleChangedSlot(const QString &)));

	editButton = new QPushButton(tr("Edit"), editor);
	editButton->setEnabled(CurrentEngine->supportEditing());

	deleteButton = new QPushButton(tr("Delete"), editor);
	deleteButton->setEnabled(!availableStyles[CurrentEngine->currentStyleName()].global);
	connect(editButton, SIGNAL(clicked()), this, SLOT(editStyleClicked()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteStyleClicked()));

	editorLayout->addWidget(syntaxListCombo, 100);
	editorLayout->addWidget(editButton);
	editorLayout->addWidget(deleteButton);
//variants
	variantListCombo = new QComboBox();
	variantListCombo->addItems(CurrentEngine->styleVariants(CurrentEngine->currentStyleName()));
	QString defaultVariant = CurrentEngine->defaultVariant(CurrentEngine->currentStyleName());
	if (!defaultVariant.isEmpty() && variantListCombo->findText(defaultVariant) == -1)
		variantListCombo->insertItem(0, defaultVariant);

	variantListCombo->setCurrentIndex(variantListCombo->findText(CurrentEngine->currentStyleVariant().isNull() ? defaultVariant : CurrentEngine->currentStyleVariant()));
	variantListCombo->setEnabled(CurrentEngine->supportVariants());
	connect(variantListCombo, SIGNAL(activated(const QString &)), this, SLOT(variantChangedSlot(const QString &)));
//preview
	preview = new Preview();

	preparePreview(preview);

	CurrentEngine->prepareStylePreview(preview, CurrentEngine->currentStyleName(), CurrentEngine->currentStyleVariant());
//
	groupBox->addWidgets(editorLabel, editor);
	groupBox->addWidgets(new QLabel(qApp->translate("@default", "Style variant") + ":"), variantListCombo);
	groupBox->addWidgets(new QLabel(qApp->translate("@default", "Preview") + ":"), preview);

	turnOnTransparency = dynamic_cast<QCheckBox *>(window->widget()->widgetById("useTransparency"));
	turnOnTransparency->setEnabled(CompositingEnabled);
}

void ChatStylesManager::configurationApplied()
{
	config_file.writeEntry("Look", "Style", syntaxListCombo->currentText());
	config_file.writeEntry("Look", "ChatStyleVariant", variantListCombo->currentText());
}

void ChatStylesManager::preparePreview(Preview *preview)
{
	Buddy example = Buddy::dummy();
	if (example.isNull())
		return;

	Chat chat = Chat::create();
	chat.setChatAccount(example.preferredAccount());
	ChatDetailsSimple *details = new ChatDetailsSimple(chat);
	details->setState(StorableObject::StateNew);
	details->setContact(example.preferredContact());
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
	preview->addObjectToParse(Core::instance()->myself().preferredContact(), messageRenderInfo);

	Message messageReceived = Message::create();
	messageReceived.setMessageChat(chat);
	messageReceived.setType(Message::TypeReceived);
	messageReceived.setMessageSender(example.preferredContact());
	messageReceived.setContent(tr("Message from Your friend"));
	messageReceived.setReceiveDate(QDateTime::currentDateTime());
	messageReceived.setSendDate(QDateTime::currentDateTime());

	messageRenderInfo = new MessageRenderInfo(messageReceived);
	messageRenderInfo->setSeparatorSize(CfgHeaderSeparatorHeight);
	preview->addObjectToParse(example.preferredContact(), messageRenderInfo);
}

void ChatStylesManager::styleChangedSlot(const QString &styleName)
{
	ChatStyleEngine *engine = availableStyles[styleName].engine;
	editButton->setEnabled(engine->supportEditing());
	deleteButton->setEnabled(!availableStyles[styleName].global);
	variantListCombo->clear();
	variantListCombo->addItems(engine->styleVariants(styleName));

	QString currentVariant = CurrentEngine->defaultVariant(styleName);
	if (!currentVariant.isEmpty() && variantListCombo->findText(currentVariant) == -1)
		variantListCombo->insertItem(0, currentVariant);

	variantListCombo->setCurrentIndex(variantListCombo->findText(currentVariant.isNull() ? "Default.css" : currentVariant));

	variantListCombo->setEnabled(engine->supportVariants());
	engine->prepareStylePreview(preview, styleName, variantListCombo->currentText());
	turnOnTransparency->setChecked(engine->styleUsesTransparencyByDefault(styleName));
}

void ChatStylesManager::variantChangedSlot(const QString &variantName)
{
	availableStyles[syntaxListCombo->currentText()].engine->prepareStylePreview(preview, syntaxListCombo->currentText(), variantName);
}

void ChatStylesManager::editStyleClicked()
{
	availableStyles[syntaxListCombo->currentText()].engine->styleEditionRequested(syntaxListCombo->currentText());
}

void ChatStylesManager::deleteStyleClicked()
{
	QString styleName = syntaxListCombo->currentText();
	if (availableStyles[styleName].engine->removeStyle(styleName))
	{
		availableStyles.remove(styleName);
		syntaxListCombo->removeItem(syntaxListCombo->currentIndex());
		styleChangedSlot(*(availableStyles.keys().begin()));
	}
	else
		MessageDialog::msg(tr("Unable to remove style: %1").arg(styleName), true, "dialog-warning.png");
}

void ChatStylesManager::syntaxUpdated(const QString &syntaxName)
{
	if (!availableStyles.contains(syntaxName))
		return;

	if (syntaxListCombo && syntaxListCombo->currentText() == syntaxName)
		styleChangedSlot(syntaxName);

	if (CurrentEngine->currentStyleName() == syntaxName)
		CurrentEngine->loadStyle(syntaxName, variantListCombo->currentText());
}

void ChatStylesManager::addStyle(const QString &syntaxName, ChatStyleEngine *engine)
{
	if (availableStyles.contains(syntaxName))
		return;

	availableStyles[syntaxName].engine = engine;
	availableStyles[syntaxName].global = false;

	if (syntaxListCombo)
		syntaxListCombo->addItem(syntaxName);
}

void ChatStylesManager::configurationWindowDestroyed()
{
	syntaxListCombo = 0;
	turnOnTransparency = 0;
}
