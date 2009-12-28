/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_STYLES_MANAGER_H
#define CHAT_STYLES_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"

class QPushButton;
class QComboBox;

class AdiumChatStyleEngine;
class ChatMessagesView;
class ChatStyleEngine;
class KaduChatStyleEngine;
class Preview;

//TODO: review
struct StyleInfo
{
	bool global;
	ChatStyleEngine *engine;
};

class ChatStylesManager : public QObject, ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

	static ChatStylesManager *Instance;
	ChatStylesManager();

	QMap<QString, ChatStyleEngine *> registeredEngines;
	QList<ChatMessagesView *> chatViews;
	QMap<QString, StyleInfo> availableStyles;

	ChatStyleEngine *CurrentEngine;

	bool CompositingEnabled;

	bool CfgNoHeaderRepeat; /*!< Remove repeated message headers. */
	unsigned int CfgHeaderSeparatorHeight; /*!< Header separator height. */
	unsigned int CfgNoHeaderInterval; /*!< Time Interval, in which headers will not be repeated*/
	unsigned int ParagraphSeparator; /*!< Message separator height. */

	unsigned int Prune; /*!< Maximal number of visible messages. */

	bool NoServerTime; /*!< Remove server time */
	int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be removed */

	QString MainStyle;

	KaduChatStyleEngine *kaduEngine;
	AdiumChatStyleEngine *adiumEngine;

	//configuration
	QComboBox *syntaxListCombo;
	QPushButton *editButton;
	QPushButton *deleteButton;

	QComboBox *variantListCombo;

	Preview *preview;

	void compositingEnabled();
	void compositingDisabled();

private slots:
	void styleChangedSlot(const QString &styleName);
	void variantChangedSlot(const QString &variantName);
	void editStyleClicked();
	void deleteStyleClicked();
	void configurationWindowDestroyed();
	void configurationApplied();

protected:
	virtual void configurationUpdated();

public:

	static ChatStylesManager * instance();
	~ChatStylesManager();

	void chatViewCreated(ChatMessagesView * view);
	void chatViewDestroyed(ChatMessagesView * view);
	void registerChatStyleEngine(const QString &name, ChatStyleEngine *engine);
	void unregisterChatStyleEngine(const QString &name);

	bool hasChatStyle(const QString &name) { return  availableStyles.contains(name); }

	ChatStyleEngine * currentEngine() { return CurrentEngine; }

	void loadThemes();

	bool cfgNoHeaderRepeat() { return CfgNoHeaderRepeat; }
	unsigned int cfgHeaderSeparatorHeight() { return CfgHeaderSeparatorHeight; }
	unsigned int cfgNoHeaderInterval() { return CfgNoHeaderInterval; }
	unsigned int paragraphSeparator() { return ParagraphSeparator; }

	bool noServerTime() { return NoServerTime; }
	int noServerTimeDiff() { return NoServerTimeDiff; }

	unsigned int prune() { return Prune; }

	QString mainStyle() { return MainStyle; }

	void mainConfigurationWindowCreated(MainConfigurationWindow *window);

	void preparePreview(Preview *preview);
	void addStyle(const QString &syntaxName, ChatStyleEngine *engine);

public slots:
	void syntaxUpdated(const QString &syntaxName);
};

#endif
