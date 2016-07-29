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

#include "dom/dom-visitor-provider-repository.h"
#include "dom/dom-visitor-provider.h"

#include <QtTest/QtTest>

class DomVisitorProviderRepositoryTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldBeEmptyAfterCreation();
	void shouldContainAddedElementsInProperOrder();
	void shouldNotContainRemovedElements();

};

class MockDomVisitorProvider : public DomVisitorProvider
{
	virtual DomVisitor * provide() const override { return {}; };
};

void DomVisitorProviderRepositoryTest::shouldBeEmptyAfterCreation()
{
	DomVisitorProviderRepository domVisitorProviderRepository{};

	auto visitorProviders = domVisitorProviderRepository.getVisitorProviders();
	QCOMPARE(visitorProviders.size(), int{0});
	QVERIFY(visitorProviders.begin() == visitorProviders.end());
}

void DomVisitorProviderRepositoryTest::shouldContainAddedElementsInProperOrder()
{
	DomVisitorProviderRepository domVisitorProviderRepository{};
	auto provider1 = MockDomVisitorProvider{};
	auto provider2 = MockDomVisitorProvider{};
	auto provider3 = MockDomVisitorProvider{};
	auto provider4 = MockDomVisitorProvider{};
	auto provider5 = MockDomVisitorProvider{};

	domVisitorProviderRepository.addVisitorProvider(&provider1, 0);
	domVisitorProviderRepository.addVisitorProvider(&provider2, 1000);
	domVisitorProviderRepository.addVisitorProvider(&provider3, -1000);
	domVisitorProviderRepository.addVisitorProvider(&provider4, -500);
	domVisitorProviderRepository.addVisitorProvider(&provider5, 500);

	auto visitorProviders = domVisitorProviderRepository.getVisitorProviders();
	QCOMPARE(visitorProviders.size(), int{5});
	QVERIFY(visitorProviders.begin() != visitorProviders.end());
	QCOMPARE(visitorProviders[0], &provider3);
	QCOMPARE(visitorProviders[1], &provider4);
	QCOMPARE(visitorProviders[2], &provider1);
	QCOMPARE(visitorProviders[3], &provider5);
	QCOMPARE(visitorProviders[4], &provider2);
}

void DomVisitorProviderRepositoryTest::shouldNotContainRemovedElements()
{
	DomVisitorProviderRepository domVisitorProviderRepository{};
	auto provider1 = MockDomVisitorProvider{};
	auto provider2 = MockDomVisitorProvider{};
	auto provider3 = MockDomVisitorProvider{};
	auto provider4 = MockDomVisitorProvider{};
	auto provider5 = MockDomVisitorProvider{};

	domVisitorProviderRepository.addVisitorProvider(&provider1, 0);
	domVisitorProviderRepository.addVisitorProvider(&provider2, 1000);
	domVisitorProviderRepository.addVisitorProvider(&provider3, -1000);
	domVisitorProviderRepository.addVisitorProvider(&provider4, -500);
	domVisitorProviderRepository.addVisitorProvider(&provider5, 500);

	domVisitorProviderRepository.removeVisitorProvider(&provider1);
	domVisitorProviderRepository.removeVisitorProvider(&provider3);
	domVisitorProviderRepository.removeVisitorProvider(&provider2);

	auto visitorProviders = domVisitorProviderRepository.getVisitorProviders();
	QCOMPARE(visitorProviders.size(), int{2});
	QVERIFY(visitorProviders.begin() != visitorProviders.end());
	QCOMPARE(visitorProviders[0], &provider4);
	QCOMPARE(visitorProviders[1], &provider5);
}

QTEST_APPLESS_MAIN(DomVisitorProviderRepositoryTest)
#include "dom-visitor-provider-repository.test.moc"
