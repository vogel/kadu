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

#include "jump-list.h"
#include "windows-jump-list-service.h"

#include "chat/chat.h"
#include "chat/open-chat-repository.h"
#include "chat/recent-chat-repository.h"
#include "misc/memory.h"

#include <QtTest/QtTest>
#include <injeqt/injector.h>
#include <injeqt/module.h>

class WindowsJumpListServiceTest : public QObject
{
	Q_OBJECT

private:
	injeqt::injector makeInjector() const;

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldShowRecentChats();
	void shouldHideWhenAllRecentChatClose();
	void shouldShowOpenChats();
	void shouldHideWhenAllOpenChatClose();
	void shouldShowSeparatorBetweenRecentAndOpen();
	void shouldIgnoreOpenChatsInRemote();

};

class JumpListStub : public JumpList
{
	Q_OBJECT

public:
	Q_INVOKABLE JumpListStub() {}
	~JumpListStub() {}

	std::set<Chat> chats1() const { return m_chats1; }
	std::set<Chat> chats2() const { return m_chats2; }
	bool visible() const { return m_visible; }
	bool hasSeparator() const { return m_hasSeparator; }

private:
	std::set<Chat> m_chats1;
	std::set<Chat> m_chats2;
	bool m_visible {false};
	bool m_hasSeparator {false};

	virtual void clear() override { m_chats1.clear(); m_chats2.clear(); m_hasSeparator = false; }
	virtual void addChat(Chat chat) override { m_hasSeparator ? m_chats2.insert(chat) : m_chats1.insert(chat); }
	virtual void addSeparator() override { m_hasSeparator = true; }
	virtual void setVisible(bool visible) override { m_visible = visible; }

};

injeqt::injector WindowsJumpListServiceTest::makeInjector() const
{
	class module : public injeqt::module
	{
	public:
		module()
		{
			add_type<JumpListStub>();
			add_type<OpenChatRepository>();
			add_type<RecentChatRepository>();
			add_type<WindowsJumpListService>();
		}
	};

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(std::make_unique<module>());

	return injeqt::injector{std::move(modules)};
}

void WindowsJumpListServiceTest::shouldBeEmptyAfterCreation()
{
	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();

	QVERIFY(jumpList->chats1().empty());
	QVERIFY(jumpList->chats2().empty());
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(!jumpList->visible());
}

void WindowsJumpListServiceTest::shouldShowRecentChats()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	recentChatRepository->addRecentChat(chat1);
	QCOMPARE(jumpList->chats1(), std::set<Chat>{ chat1 });
	QCOMPARE(jumpList->chats2(), std::set<Chat>{});
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());

	recentChatRepository->addRecentChat(chat2);
	QCOMPARE(jumpList->chats1(), (std::set<Chat>{ chat1, chat2 }));
	QCOMPARE(jumpList->chats2(), std::set<Chat>{});
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());
}

void WindowsJumpListServiceTest::shouldHideWhenAllRecentChatClose()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto recentChatRepository = injector.get<RecentChatRepository>();

	recentChatRepository->addRecentChat(chat1);
	recentChatRepository->addRecentChat(chat2);
	recentChatRepository->removeRecentChat(chat1);

	QCOMPARE(jumpList->chats1(), std::set<Chat>{ chat2 });
	QCOMPARE(jumpList->chats2(), std::set<Chat>{});
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());

	recentChatRepository->removeRecentChat(chat2);

	QVERIFY(jumpList->chats1().empty());
	QVERIFY(jumpList->chats2().empty());
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(!jumpList->visible());
}

void WindowsJumpListServiceTest::shouldShowOpenChats()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	openChatRepository->addOpenChat(chat1);
	QCOMPARE(jumpList->chats1(), std::set<Chat>{ chat1 });
	QCOMPARE(jumpList->chats2(), std::set<Chat>{});
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());

	openChatRepository->addOpenChat(chat2);
	QCOMPARE(jumpList->chats1(), (std::set<Chat>{ chat1, chat2 }));
	QCOMPARE(jumpList->chats2(), (std::set<Chat>{}));
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());
}

void WindowsJumpListServiceTest::shouldHideWhenAllOpenChatClose()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	openChatRepository->addOpenChat(chat1);
	openChatRepository->addOpenChat(chat2);
	openChatRepository->removeOpenChat(chat1);

	QCOMPARE(jumpList->chats1(), std::set<Chat>{ chat2 });
	QCOMPARE(jumpList->chats2(), std::set<Chat>{});
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(jumpList->visible());

	openChatRepository->removeOpenChat(chat2);

	QVERIFY(jumpList->chats1().empty());
	QVERIFY(jumpList->chats2().empty());
	QVERIFY(!jumpList->hasSeparator());
	QVERIFY(!jumpList->visible());
}

void WindowsJumpListServiceTest::shouldShowSeparatorBetweenRecentAndOpen()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	recentChatRepository->addRecentChat(chat1);
	openChatRepository->addOpenChat(chat2);

	QCOMPARE(jumpList->chats1(), (std::set<Chat>{ chat1 }));
	QCOMPARE(jumpList->chats2(), (std::set<Chat>{ chat2 }));
	QVERIFY(jumpList->hasSeparator());
	QVERIFY(jumpList->visible());
}

void WindowsJumpListServiceTest::shouldIgnoreOpenChatsInRemote()
{
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	auto injector = makeInjector();
	auto jumpList = injector.get<JumpListStub>();
	injector.instantiate<WindowsJumpListService>();
	auto recentChatRepository = injector.get<RecentChatRepository>();
	auto openChatRepository = injector.get<OpenChatRepository>();

	recentChatRepository->addRecentChat(chat1);
	recentChatRepository->addRecentChat(chat2);
	openChatRepository->addOpenChat(chat1);

	QCOMPARE(jumpList->chats1(), std::set<Chat>{ chat2 });
	QCOMPARE(jumpList->chats2(), std::set<Chat>{ chat1 });
	QVERIFY(jumpList->hasSeparator());
	QVERIFY(jumpList->visible());
}

QTEST_APPLESS_MAIN(WindowsJumpListServiceTest)
#include "windows-jump-list-service.test.moc"
