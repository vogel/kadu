/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/recent-chat-service.h"

#include "chat/chat-manager-impl.h"
#include "chat/recent-chat-repository.h"
#include "message/message-manager.h"
#include "misc/memory.h"
#include "storage/storage-point-factory.h"

#include <QtTest/QtTest>
#include <injeqt/injector.h>
#include <injeqt/module.h>

class RecentChatServiceTest : public QObject
{
	Q_OBJECT

private:
	injeqt::injector makeInjector() const;

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldIncludeChatWithSentMessage();
	void shouldIncludeChatWithReceivedMessage();
	void shouldIncludeChatWithMessageOnlyOnce();
	void shouldIncludeLoadedChatWithRecentMessage();
	void shouldIncludeAddedChatWithRecentMessage();
	void shouldNotIncludeLoadedChatWithTooOldRecentMessage();
	void shouldNotIncludeAddedChatWithTooOldRecentMessage();
	void shouldRemoveRemovedChat();
	void shouldCleanUpOld();

};

class ChatManagerStub : public ChatManager
{
	Q_OBJECT

public:
	Q_INVOKABLE ChatManagerStub() {}
	~ChatManagerStub() {}

	virtual Chat byDisplay(const QString &) override { return {}; }
	virtual QVector< Chat > chats(const Account &) override { return {}; }
	virtual Chat loadStubFromStorage(const std::shared_ptr<StoragePoint> &) override { return {}; }
	virtual QString storageNodeItemName() override { return {}; }
	virtual QString storageNodeName() override { return {}; }

};

class MessageManagerStub : public MessageManager
{
	Q_OBJECT

public:
	Q_INVOKABLE MessageManagerStub() {}
	~MessageManagerStub() {}

	virtual bool sendMessage(const Chat &, NormalizedHtmlString, bool) override { return {}; }
	virtual bool sendRawMessage(const Chat &, const QByteArray &) override { return {}; }

	void emulateSent(Chat chat)
	{
		auto message = Message{new MessageShared{}};
		message.setMessageChat(chat);
		emit messageSent(message);
	}

	void emulateReceived(Chat chat)
	{
		auto message = Message{new MessageShared{}};
		message.setMessageChat(chat);
		emit messageReceived(message);
	}

};

class StoragePointFactoryStub : public StoragePointFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE StoragePointFactoryStub() {}
	~StoragePointFactoryStub() {}

	virtual std::unique_ptr<StoragePoint> createStoragePoint(const QString &, StoragePoint *) { return {}; }

};

injeqt::injector RecentChatServiceTest::makeInjector() const
{
	class module : public injeqt::module
	{
	public:
		module()
		{
			add_type<ChatManagerStub>();
			add_type<MessageManagerStub>();
			add_type<RecentChatRepository>();
			add_type<RecentChatService>();
			add_type<StoragePointFactoryStub>();
		}
	};

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(std::make_unique<module>());

	return injeqt::injector{std::move(modules)};
}

void RecentChatServiceTest::shouldBeEmptyAfterCreation()
{
	auto injector = makeInjector();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	injector.instantiate<RecentChatService>();

	QCOMPARE(recentChatRepository->size(), size_t{0});
	QVERIFY(recentChatRepository->begin() == recentChatRepository->end());
}

void RecentChatServiceTest::shouldIncludeChatWithSentMessage()
{
	Chat chat{new ChatShared{}};

	auto injector = makeInjector();
	auto messageManager = injector.get<MessageManagerStub>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	injector.instantiate<RecentChatService>();

	QVERIFY(!chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));

	messageManager->emulateSent(chat);

	QVERIFY(chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat) != recentChatRepository->end());
}

void RecentChatServiceTest::shouldIncludeChatWithReceivedMessage()
{
	Chat chat{new ChatShared{}};

	auto injector = makeInjector();
	auto messageManager = injector.get<MessageManagerStub>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	injector.instantiate<RecentChatService>();

	QVERIFY(!chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));

	messageManager->emulateReceived(chat);

	QVERIFY(chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat) != recentChatRepository->end());
}

void RecentChatServiceTest::shouldIncludeChatWithMessageOnlyOnce()
{
	Chat chat{new ChatShared{}};

	auto injector = makeInjector();
	auto messageManager = injector.get<MessageManagerStub>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	injector.instantiate<RecentChatService>();

	messageManager->emulateSent(chat);
	messageManager->emulateReceived(chat);

	QCOMPARE(recentChatRepository->size(), size_t{1});
}

void RecentChatServiceTest::shouldIncludeLoadedChatWithRecentMessage()
{
	Chat chat1{new ChatShared{}};
	Chat chat2{new ChatShared{}};
	chat1.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	chatManager->addItem(chat1);
	chatManager->addItem(chat2);
	injector.instantiate<RecentChatService>();

	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat1) != recentChatRepository->end());
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat2) == recentChatRepository->end());
}

void RecentChatServiceTest::shouldIncludeAddedChatWithRecentMessage()
{
	Chat chat1{new ChatShared{}};
	Chat chat2{new ChatShared{}};
	chat1.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	injector.instantiate<RecentChatService>();
	chatManager->addItem(chat1);
	chatManager->addItem(chat2);

	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat1) != recentChatRepository->end());
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat2) == recentChatRepository->end());
}

void RecentChatServiceTest::shouldNotIncludeLoadedChatWithTooOldRecentMessage()
{
	Chat chat{new ChatShared{}};
	chat.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc().addSecs(-1 * RecentChatService::KEEP_RECENT_FOR_SECONDS - 1), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	chatManager->addItem(chat);
	injector.instantiate<RecentChatService>();

	QVERIFY(!chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{0});
	QVERIFY(recentChatRepository->begin() == recentChatRepository->end());
}

void RecentChatServiceTest::shouldNotIncludeAddedChatWithTooOldRecentMessage()
{
	Chat chat{new ChatShared{}};
	chat.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc().addSecs(-1 * RecentChatService::KEEP_RECENT_FOR_SECONDS - 1), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	injector.instantiate<RecentChatService>();
	chatManager->addItem(chat);

	QVERIFY(!chat.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{0});
	QVERIFY(recentChatRepository->begin() == recentChatRepository->end());
}

void RecentChatServiceTest::shouldRemoveRemovedChat()
{
	Chat chat1{new ChatShared{}};
	Chat chat2{new ChatShared{}};
	chat1.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);
	chat2.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	injector.instantiate<RecentChatService>();

	chatManager->addItem(chat1);
	chatManager->addItem(chat2);

	chatManager->removeItem(chat2);

	QVERIFY(chat1.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QVERIFY(!chat2.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat1) != recentChatRepository->end());
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat2) == recentChatRepository->end());
}

void RecentChatServiceTest::shouldCleanUpOld()
{
	Chat chat1{new ChatShared{}};
	Chat chat2{new ChatShared{}};
	chat1.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);
	chat2.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);

	auto injector = makeInjector();
	auto chatManager = injector.get<ChatManager>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	auto recentChatService = injector.get<RecentChatService>();

	chatManager->addItem(chat1);
	chatManager->addItem(chat2);

	chat1.addProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc().addSecs(-1 * RecentChatService::KEEP_RECENT_FOR_SECONDS - 1), CustomProperties::Storable);

	recentChatService->cleanUp();

	QVERIFY(!chat1.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QVERIFY(chat2.hasProperty(RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY));
	QCOMPARE(recentChatRepository->size(), size_t{1});
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat1) == recentChatRepository->end());
	QVERIFY(std::find(recentChatRepository->begin(), recentChatRepository->end(), chat2) != recentChatRepository->end());
}

QTEST_MAIN(RecentChatServiceTest)
#include "recent-chat-service.test.moc"
