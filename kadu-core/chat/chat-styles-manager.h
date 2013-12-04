/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QObject>

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
class FormattedStringFactory;
class KaduChatStyleEngine;
class Preview;

//TODO: review
struct StyleInfo
{
	bool global;
	ChatStyleEngine *engine;

	StyleInfo() : global(false), engine(0) {}
};

class KADUAPI ChatStylesManager : public QObject, ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

	static ChatStylesManager *Instance;

	QWeakPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	ChatStylesManager();

	void init();

	QMap<QString, ChatStyleEngine *> RegisteredEngines;
	QList<ChatMessagesView *> ChatViews;
	QMap<QString, StyleInfo> AvailableStyles;

	ChatStyleEngine *CurrentEngine;

	bool CompositingEnabled;

	bool CfgNoHeaderRepeat; /*!< Remove repeated message headers. */
	int CfgHeaderSeparatorHeight; /*!< Header separator height. */
	int CfgNoHeaderInterval; /*!< Time Interval, in which headers will not be repeated*/
	int ParagraphSeparator; /*!< Message separator height. */

	int Prune; /*!< Maximal number of visible messages. */

	bool NoServerTime; /*!< Remove server time */
	int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be removed */

	QString MainStyle;

	KaduChatStyleEngine *KaduEngine;
	AdiumChatStyleEngine *AdiumEngine;

	//configuration
	QComboBox *SyntaxListCombo;
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
	void configurationWindowDestroyed();
	void configurationApplied();

protected:
	virtual void configurationUpdated();

public:
	static ChatStylesManager * instance();

	~ChatStylesManager();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void chatViewCreated(ChatMessagesView * view);
	void chatViewDestroyed(ChatMessagesView * view);
	void registerChatStyleEngine(const QString &name, ChatStyleEngine *engine);
	void unregisterChatStyleEngine(const QString &name);

	bool hasChatStyle(const QString &name) { return  AvailableStyles.contains(name); }
	StyleInfo chatStyleInfo(const QString &name);

	ChatStyleEngine * currentEngine() { return CurrentEngine; }

	void loadStyles();

	bool cfgNoHeaderRepeat() { return CfgNoHeaderRepeat; }
	int cfgHeaderSeparatorHeight() { return CfgHeaderSeparatorHeight; }
	int cfgNoHeaderInterval() { return CfgNoHeaderInterval; }
	int paragraphSeparator() { return ParagraphSeparator; }

	bool noServerTime() { return NoServerTime; }
	int noServerTimeDiff() { return NoServerTimeDiff; }

	int prune() { return Prune; }

	const QString & mainStyle() { return MainStyle; }

	void mainConfigurationWindowCreated(MainConfigurationWindow *window);
	QComboBox * syntaxListCombo() { return SyntaxListCombo; }

	void preparePreview(Preview *preview);
	void addStyle(const QString &syntaxName, ChatStyleEngine *engine);

public slots:
	void syntaxUpdated(const QString &syntaxName);

signals:
	void previewSyntaxChanged(const QString &syntaxName);

};

#endif
