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

#include "chat/open-chat-service.h"

#include "chat/open-chat-service.h"
#include "chat/open-chat-repository.h"
#include "gui/widgets/chat-widget/chat-widget-repository-impl.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "message/sorted-messages.h"
#include "misc/memory.h"

#include <QtTest/QtTest>
#include <injeqt/injector.h>
#include <injeqt/module.h>

class OpenChatServiceTest : public QObject
{
	Q_OBJECT

private:
	injeqt::injector makeInjector() const;

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldAddChatFromWidgetToOpenWhenServiceAlreadyInstantiated();
	void shouldAddChatFromWidgetToOpenWhenServiceInstantiatedLater();
	void shouldRemoteChatFromWidgetFromOpenWhenServiceAlreadyInstantiated();
	void shouldRemoteChatFromWidgetFromOpenWhenServiceInstantiatedLater();

};

class ChatWidgetStub : public ChatWidget
{
	Q_OBJECT

public:
	explicit ChatWidgetStub(Chat chat) : m_chat{chat} {}
	~ChatWidgetStub() {}

	virtual Chat chat() const override { return m_chat; }
	virtual ChatState chatState() const override { return {}; }

	virtual void addMessages(const SortedMessages &) override { }
	virtual void addMessage(const Message &) override { }
	virtual void appendSystemMessage(QString) override { }
	virtual SortedMessages messages() const override { return {}; }

	virtual const QDateTime & lastReceivedMessageTime() const override { return m_dateTime; }

	virtual void kaduStoreGeometry() override { }
	virtual void kaduRestoreGeometry() override { }

	virtual ChatEditBox * getChatEditBox() const override { return {}; }
	virtual ChatWidgetTitle * title() const override { return {}; }
	virtual CustomInput * edit() const override { return {}; }
	virtual TalkableProxyModel * talkableProxyModel() const override { return {}; }
	virtual WebkitMessagesView * chatMessagesView() const override { return {}; }

public slots:
	virtual void sendMessage() override { }
	virtual void colorSelectorAboutToClose() override { }
	virtual void clearChatWindow() override { }

	virtual void requestClose() override { }


private:
	Chat m_chat;
	QDateTime m_dateTime;

};

injeqt::injector OpenChatServiceTest::makeInjector() const
{
	class module : public injeqt::module
	{
	public:
		module()
		{
			add_type<ChatWidgetRepositoryImpl>();
			add_type<OpenChatRepository>();
			add_type<OpenChatService>();
		}
	};

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(std::make_unique<module>());

	return injeqt::injector{std::move(modules)};
}

void OpenChatServiceTest::shouldBeEmptyAfterCreation()
{
	auto injector = makeInjector();
	auto openChatRepository = injector.get<OpenChatRepository>();

	QCOMPARE(openChatRepository->size(), size_t{0});
	QVERIFY(openChatRepository->begin() == openChatRepository->end());
}

void OpenChatServiceTest::shouldAddChatFromWidgetToOpenWhenServiceAlreadyInstantiated()
{
	Chat chat1{new ChatShared()};
	ChatWidgetStub chatWidget1{chat1};
	Chat chat2{new ChatShared()};
	ChatWidgetStub chatWidget2{chat2};

	auto injector = makeInjector();
	auto chatWidgetRepository = injector.get<ChatWidgetRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();
	injector.instantiate<OpenChatService>();

	chatWidgetRepository->addChatWidget(&chatWidget1);

	QCOMPARE(openChatRepository->size(), size_t{1});
	QVERIFY(openChatRepository->begin() != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat1) != openChatRepository->end());

	chatWidgetRepository->addChatWidget(&chatWidget2);

	QCOMPARE(openChatRepository->size(), size_t{2});
	QVERIFY(openChatRepository->begin() != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat2) != openChatRepository->end());
}

void OpenChatServiceTest::shouldAddChatFromWidgetToOpenWhenServiceInstantiatedLater()
{
	Chat chat1{new ChatShared()};
	ChatWidgetStub chatWidget1{chat1};
	Chat chat2{new ChatShared()};
	ChatWidgetStub chatWidget2{chat2};

	auto injector = makeInjector();
	auto chatWidgetRepository = injector.get<ChatWidgetRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	chatWidgetRepository->addChatWidget(&chatWidget1);
	chatWidgetRepository->addChatWidget(&chatWidget2);

	injector.instantiate<OpenChatService>();

	QCOMPARE(openChatRepository->size(), size_t{2});
	QVERIFY(openChatRepository->begin() != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat1) != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat2) != openChatRepository->end());
}

void OpenChatServiceTest::shouldRemoteChatFromWidgetFromOpenWhenServiceAlreadyInstantiated()
{
	Chat chat1{new ChatShared()};
	ChatWidgetStub chatWidget1{chat1};
	Chat chat2{new ChatShared()};
	ChatWidgetStub chatWidget2{chat2};

	auto injector = makeInjector();
	auto chatWidgetRepository = injector.get<ChatWidgetRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();
	injector.instantiate<OpenChatService>();

	chatWidgetRepository->addChatWidget(&chatWidget1);
	chatWidgetRepository->addChatWidget(&chatWidget2);

	chatWidgetRepository->removeChatWidget(&chatWidget2);

	QCOMPARE(openChatRepository->size(), size_t{1});
	QVERIFY(openChatRepository->begin() != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat1) != openChatRepository->end());
	QVERIFY(std::find(openChatRepository->begin(), openChatRepository->end(), chat2) == openChatRepository->end());

	chatWidgetRepository->removeChatWidget(chat1);

	QCOMPARE(openChatRepository->size(), size_t{0});
	QVERIFY(openChatRepository->begin() == openChatRepository->end());
}

void OpenChatServiceTest::shouldRemoteChatFromWidgetFromOpenWhenServiceInstantiatedLater()
{
	Chat chat1{new ChatShared()};
	ChatWidgetStub chatWidget1{chat1};
	Chat chat2{new ChatShared()};
	ChatWidgetStub chatWidget2{chat2};

	auto injector = makeInjector();
	auto chatWidgetRepository = injector.get<ChatWidgetRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	chatWidgetRepository->addChatWidget(&chatWidget1);
	chatWidgetRepository->addChatWidget(&chatWidget2);

	chatWidgetRepository->removeChatWidget(&chatWidget2);
	chatWidgetRepository->removeChatWidget(chat1);

	injector.instantiate<OpenChatService>();

	QCOMPARE(openChatRepository->size(), size_t{0});
	QVERIFY(openChatRepository->begin() == openChatRepository->end());
}

QTEST_MAIN(OpenChatServiceTest)
#include "open-chat-service.test.moc"
