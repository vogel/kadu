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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtGui/QIcon>

class Chat;
class ChatWidget;

enum class ChatWidgetTitleComposingStatePosition;

class KADUAPI ChatWidgetTitle : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetTitle(ChatWidget *parent = nullptr);
	virtual ~ChatWidgetTitle();

	ChatWidget * chatWidget() const;
	QString title() const;
	QString shortTitle() const;
	QString fullTitle() const;
	QString blinkingFullTitle() const;
	QString tooltip() const;

	QIcon icon() const;
	QIcon blinkingIcon() const;

	void setComposingStatePosition(ChatWidgetTitleComposingStatePosition composingStatePosition);
	void setShowUnreadMessagesCount(bool showUnreadMessagesCount);
	void setBlinkTitleWhenUnreadMessages(bool blinkTitleWhenUnreadMessages);
	void setBlinkIconWhenUnreadMessages(bool blinkIconWhenUnreadMessages);

signals:
	void titleChanged(ChatWidget *chatWidget);

private:
	QString m_title;
	QString m_fullTitle;
	QString m_tooltip;
	QIcon m_icon;

	ChatWidgetTitleComposingStatePosition m_composingStatePosition;
	bool m_showUnreadMessagesCount;
	bool m_blinkTitleWhenUnreadMessages;
	bool m_blinkIconWhenUnreadMessages;

	bool m_blink;
	QPointer<QTimer> m_blinkTimer;

	QString chatTitle(const Chat &chat) const;
	QString withDescription(const Chat &chat, const QString &title) const;
	QString withCompositionInfo(const QString &title) const;
	QString withUnreadMessagesCount(QString title) const;
	QString cleanUp(QString title) const;

	QIcon chatIcon(const Chat &chat) const;

private slots:
	void startBlinking();
	void stopBlinking();
	void blink();
	void update();

};
