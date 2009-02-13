/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>

#include "chat/style-engines/chat_engine_kadu.h"

#include "gui/widgets/chat_messages_view.h"

#include "config_file.h"
#include "misc.h"

#include "chat_styles_manager.h"

ChatStylesManager * ChatStylesManager::Instance = 0;

ChatStylesManager * ChatStylesManager::instance()
{
	if (0 == Instance)
		Instance = new ChatStylesManager();

	return Instance;
}

ChatStylesManager::ChatStylesManager() : CurrentEngine(0), kaduEngine(0)
{
	//FIXME:
	kaduEngine = new KaduChatStyleEngine();
	registerChatStyleEngine("Kadu", kaduEngine);

	loadThemes();
	configurationUpdated();
}

ChatStylesManager::~ChatStylesManager()
{
	unregisterChatStyleEngine("Kadu");
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
		chatViews.append(view);
}

void ChatStylesManager::chatViewDestroyed(ChatMessagesView *view)
{
	if (chatViews.contains(view))
		chatViews.remove(view);
}

bool ChatStylesManager::hasChatStyleEngine(const QString& name)
{
	return registeredEngines.contains(name);
}

ChatStyleEngine * ChatStylesManager::getChatStylesEngine(const QString &name)
{
	return registeredEngines.contains(name)
		? registeredEngines[name]
		: 0;
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
	
	// if theme was changed, load new theme
	if (!CurrentEngine || CurrentEngine->currentStyleName() != newStyleName)
	{
		if (availableStyles[newStyleName] != CurrentEngine)
			CurrentEngine = availableStyles[newStyleName];
		CurrentEngine->loadTheme(newStyleName);
	}
	else
	{//FIXME
		CurrentEngine->configurationUpdated();
	}

	foreach (ChatMessagesView *view, chatViews)
	{
		view->updateBackgroundsAndColors();
		CurrentEngine->refreshView(view);
	}
}

//any better ideas?
void ChatStylesManager::loadThemes()
{
	QDir dir;
	QString path;
	QFileInfo fi;
	QStringList files;

	path = ggPath() + "/syntax/chat/";
	dir.setPath(path);

	files = dir.entryList();

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !availableStyles.contains(file))
		{
			foreach (ChatStyleEngine *engine, registeredEngines.values())
			{
				if (engine->isThemeValid(path + file))
				{
					availableStyles[fi.baseName()] = engine;
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
				if (engine->isThemeValid(path + file))
				{
					availableStyles[fi.baseName()] = engine;
					break;
				}
			}
		}
	}
}
