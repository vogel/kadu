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

#include "chat/recent-chat-repository.h"

#include <QtTest/QtTest>

class RecentChatRepositoryTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldContainAddedElements();
	void shouldNotContainRemovedElements();
	void shouldEmitAddedAfterAdding();
	void shouldEmitRemovedAfterRemoving();

};

class RecentChatReceiver : public QObject
{
	Q_OBJECT

public:
	RecentChatReceiver(RecentChatRepository &repository) :
			m_repository{repository}
	{
		connect(&m_repository, &RecentChatRepository::recentChatAdded, this, &RecentChatReceiver::recentChatAdded);
		connect(&m_repository, &RecentChatRepository::recentChatRemoved, this, &RecentChatReceiver::recentChatRemoved);
	}

	Chat m_addedChat;
	Chat m_removedChat;

private slots:
	void recentChatAdded(Chat chat)
	{
		m_addedChat = chat;
		QVERIFY(std::find(m_repository.begin(), m_repository.end(), m_addedChat) != m_repository.end());
	}

	void recentChatRemoved(Chat chat)
	{
		m_removedChat = chat;
		QVERIFY(std::find(m_repository.begin(), m_repository.end(), m_removedChat) == m_repository.end());
	}

private:
	RecentChatRepository &m_repository;

};

void RecentChatRepositoryTest::shouldBeEmptyAfterCreation()
{
	RecentChatRepository recentChatRepository{};

	QCOMPARE(recentChatRepository.size(), size_t{0});
	QVERIFY(recentChatRepository.begin() == recentChatRepository.end());
}

void RecentChatRepositoryTest::shouldContainAddedElements()
{
	RecentChatRepository recentChatRepository{};
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	recentChatRepository.addRecentChat(chat1);
	QCOMPARE(recentChatRepository.size(), size_t{1});
	QVERIFY(std::find(recentChatRepository.begin(), recentChatRepository.end(), chat1) != recentChatRepository.end());

	recentChatRepository.addRecentChat(chat2);
	QCOMPARE(recentChatRepository.size(), size_t{2});
	QVERIFY(std::find(recentChatRepository.begin(), recentChatRepository.end(), chat1) != recentChatRepository.end());
	QVERIFY(std::find(recentChatRepository.begin(), recentChatRepository.end(), chat2) != recentChatRepository.end());
}

void RecentChatRepositoryTest::shouldNotContainRemovedElements()
{
	RecentChatRepository recentChatRepository{};
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	recentChatRepository.addRecentChat(chat1);
	recentChatRepository.addRecentChat(chat2);

	recentChatRepository.removeRecentChat(chat1);
	QCOMPARE(recentChatRepository.size(), size_t{1});
	QVERIFY(std::find(recentChatRepository.begin(), recentChatRepository.end(), chat1) == recentChatRepository.end());

	recentChatRepository.removeRecentChat(chat2);
	QCOMPARE(recentChatRepository.size(), size_t{0});
	QVERIFY(std::find(recentChatRepository.begin(), recentChatRepository.end(), chat2) == recentChatRepository.end());
}

void RecentChatRepositoryTest::shouldEmitAddedAfterAdding()
{
	RecentChatRepository recentChatRepository{};
	auto chat = Chat{new ChatShared{}};

	RecentChatReceiver receiver{recentChatRepository};
	recentChatRepository.addRecentChat(chat);

	QCOMPARE(receiver.m_addedChat, chat);
	QCOMPARE(receiver.m_removedChat, Chat::null);
}

void RecentChatRepositoryTest::shouldEmitRemovedAfterRemoving()
{
	RecentChatRepository recentChatRepository{};
	auto chat = Chat{new ChatShared{}};

	recentChatRepository.addRecentChat(chat);

	RecentChatReceiver receiver{recentChatRepository};
	recentChatRepository.removeRecentChat(chat);

	QCOMPARE(receiver.m_addedChat, Chat::null);
	QCOMPARE(receiver.m_removedChat, chat);
}

QTEST_APPLESS_MAIN(RecentChatRepositoryTest)
#include "recent-chat-repository.test.moc"
