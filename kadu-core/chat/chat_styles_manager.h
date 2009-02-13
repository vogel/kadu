#ifndef CHAT_STYLES_MANAGER_H
#define CHAT_STYLES_MANAGER_H

#include <QtCore/QMap>

#include "configuration_aware_object.h"

class ChatMessagesView;
class ChatStyleEngine;
class KaduChatStyleEngine;

class ChatStylesManager : public ConfigurationAwareObject
{
	static ChatStylesManager *Instance;
	ChatStylesManager();

	QMap<QString, ChatStyleEngine *> registeredEngines;
	QList<ChatMessagesView *> chatViews;
	QMap<QString, ChatStyleEngine *> availableStyles;

	ChatStyleEngine *CurrentEngine;

	bool CfgNoHeaderRepeat; /*!< Remove repeated message headers. */
	unsigned int CfgHeaderSeparatorHeight; /*!< Header separator height. */
	unsigned int CfgNoHeaderInterval; /*!< Time Interval, in which headers will not be repeated*/
	unsigned int ParagraphSeparator; /*!< Message separator height. */

	unsigned int Prune; /*!< Maximal number of visible messages. */

	bool NoServerTime; /*!< Remove server time */
	int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be removed */

	QString MainStyle;

	KaduChatStyleEngine *kaduEngine;

protected:
	virtual void configurationUpdated();

public:

	static ChatStylesManager * instance();
	~ChatStylesManager();

	void chatViewCreated(ChatMessagesView * view);
	void chatViewDestroyed(ChatMessagesView * view);
	void registerChatStyleEngine(const QString &name, ChatStyleEngine *engine);
	void unregisterChatStyleEngine(const QString &name);
	bool hasChatStyleEngine(const QString &name);

	const QList<ChatStyleEngine *> protocolFactories() { return registeredEngines.values(); }
	ChatStyleEngine * getChatStylesEngine(const QString &name);
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
};
#endif
