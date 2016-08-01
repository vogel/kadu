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

#include "chat/open-chat-repository.h"

#include <QtTest/QtTest>

class OpenChatRepositoryTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldContainAddedElementsOnce();
	void shouldNotContainRemovedElements();
	void shouldEmitAddedAfterAdding();
	void shouldEmitRemovedAfterRemoving();

};

class OpenChatReceiver : public QObject
{
	Q_OBJECT

public:
	OpenChatReceiver(OpenChatRepository &repository) :
			m_repository{repository}
	{
		connect(&m_repository, &OpenChatRepository::openChatAdded, this, &OpenChatReceiver::openChatAdded);
		connect(&m_repository, &OpenChatRepository::openChatRemoved, this, &OpenChatReceiver::openChatRemoved);
	}

	Chat m_addedChat;
	Chat m_removedChat;

private slots:
	void openChatAdded(Chat chat)
	{
		m_addedChat = chat;
		QVERIFY(std::find(m_repository.begin(), m_repository.end(), m_addedChat) != m_repository.end());
	}

	void openChatRemoved(Chat chat)
	{
		m_removedChat = chat;
		QVERIFY(std::find(m_repository.begin(), m_repository.end(), m_removedChat) == m_repository.end());
	}

private:
	OpenChatRepository &m_repository;

};

void OpenChatRepositoryTest::shouldBeEmptyAfterCreation()
{
	OpenChatRepository openChatRepository{};

	QCOMPARE(openChatRepository.size(), size_t{0});
	QVERIFY(openChatRepository.begin() == openChatRepository.end());
}

void OpenChatRepositoryTest::shouldContainAddedElementsOnce()
{
	OpenChatRepository openChatRepository{};
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	openChatRepository.addOpenChat(chat1);
	QCOMPARE(openChatRepository.size(), size_t{1});
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat1) != openChatRepository.end());

	openChatRepository.addOpenChat(chat2);
	QCOMPARE(openChatRepository.size(), size_t{2});
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat1) != openChatRepository.end());
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat2) != openChatRepository.end());

	openChatRepository.addOpenChat(chat2);
	QCOMPARE(openChatRepository.size(), size_t{2});
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat1) != openChatRepository.end());
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat2) != openChatRepository.end());
}

void OpenChatRepositoryTest::shouldNotContainRemovedElements()
{
	OpenChatRepository openChatRepository{};
	auto chat1 = Chat{new ChatShared{}};
	auto chat2 = Chat{new ChatShared{}};

	openChatRepository.addOpenChat(chat1);
	openChatRepository.addOpenChat(chat2);

	openChatRepository.removeOpenChat(chat1);
	QCOMPARE(openChatRepository.size(), size_t{1});
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat1) == openChatRepository.end());

	openChatRepository.removeOpenChat(chat2);
	QCOMPARE(openChatRepository.size(), size_t{0});
	QVERIFY(std::find(openChatRepository.begin(), openChatRepository.end(), chat2) == openChatRepository.end());
}

void OpenChatRepositoryTest::shouldEmitAddedAfterAdding()
{
	OpenChatRepository openChatRepository{};
	auto chat = Chat{new ChatShared{}};

	OpenChatReceiver receiver{openChatRepository};
	openChatRepository.addOpenChat(chat);

	QCOMPARE(receiver.m_addedChat, chat);
	QCOMPARE(receiver.m_removedChat, Chat::null);
}

void OpenChatRepositoryTest::shouldEmitRemovedAfterRemoving()
{
	OpenChatRepository openChatRepository{};
	auto chat = Chat{new ChatShared{}};

	openChatRepository.addOpenChat(chat);

	OpenChatReceiver receiver{openChatRepository};
	openChatRepository.removeOpenChat(chat);

	QCOMPARE(receiver.m_addedChat, Chat::null);
	QCOMPARE(receiver.m_removedChat, chat);
}

QTEST_APPLESS_MAIN(OpenChatRepositoryTest)
#include "open-chat-repository.test.moc"
