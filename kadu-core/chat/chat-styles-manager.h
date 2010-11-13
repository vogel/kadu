/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_STYLES_MANAGER_H
#define CHAT_STYLES_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QCheckBox;
class QComboBox;
class QPushButton;

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

class KADUAPI ChatStylesManager : public QObject, ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

	static ChatStylesManager *Instance;
	ChatStylesManager();

	QMap<QString, ChatStyleEngine *> RegisteredEngines;
	QList<ChatMessagesView *> ChatViews;
	QMap<QString, StyleInfo> AvailableStyles;

	ChatStyleEngine *CurrentEngine;

	bool CompositingEnabled;

	bool CfgNoHeaderRepeat; /*!< Remove repeated message headers. */
	unsigned int CfgHeaderSeparatorHeight; /*!< Header separator height. */
	unsigned int CfgNoHeaderInterval; /*!< Time Interval, in which headers will not be repeated*/
	unsigned int ParagraphSeparator; /*!< Message separator height. */

	int Prune; /*!< Maximal number of visible messages. */

	bool NoServerTime; /*!< Remove server time */
	int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be removed */

	QString MainStyle;

	KaduChatStyleEngine *KaduEngine;
	AdiumChatStyleEngine *AdiumEngine;

	//configuration
	QComboBox *SyntaxListCombo;
	QPushButton *EditButton;
	QPushButton *DeleteButton;

	QComboBox *VariantListCombo;

	QCheckBox *TurnOnTransparency;

	Preview *EnginePreview;

	void compositingEnabled();
	void compositingDisabled();

	QString fixedStyleName(QString styleName);
	QString fixedVariantName(const QString &styleName, QString variantName);

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

	bool hasChatStyle(const QString &name) { return  AvailableStyles.contains(name); }

	ChatStyleEngine * currentEngine() { return CurrentEngine; }

	void loadStyles();

	bool cfgNoHeaderRepeat() { return CfgNoHeaderRepeat; }
	unsigned int cfgHeaderSeparatorHeight() { return CfgHeaderSeparatorHeight; }
	unsigned int cfgNoHeaderInterval() { return CfgNoHeaderInterval; }
	unsigned int paragraphSeparator() { return ParagraphSeparator; }

	bool noServerTime() { return NoServerTime; }
	int noServerTimeDiff() { return NoServerTimeDiff; }

	int prune() { return Prune; }

	const QString & mainStyle() { return MainStyle; }

	void mainConfigurationWindowCreated(MainConfigurationWindow *window);

	void preparePreview(Preview *preview);
	void addStyle(const QString &syntaxName, ChatStyleEngine *engine);

public slots:
	void syntaxUpdated(const QString &syntaxName);
};

#endif
