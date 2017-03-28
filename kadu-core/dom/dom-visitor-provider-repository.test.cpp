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
    virtual const DomVisitor *provide() const override
    {
        return {};
    };
};

void DomVisitorProviderRepositoryTest::shouldBeEmptyAfterCreation()
{
    DomVisitorProviderRepository domVisitorProviderRepository{};

    QCOMPARE(domVisitorProviderRepository.size(), size_t{0});
    QVERIFY(domVisitorProviderRepository.begin() == domVisitorProviderRepository.end());
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

    QCOMPARE(domVisitorProviderRepository.size(), size_t{5});
    QVERIFY(domVisitorProviderRepository.begin() != domVisitorProviderRepository.end());

    auto it = domVisitorProviderRepository.begin();
    QCOMPARE(*it++, &provider3);
    QCOMPARE(*it++, &provider4);
    QCOMPARE(*it++, &provider1);
    QCOMPARE(*it++, &provider5);
    QCOMPARE(*it++, &provider2);
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

    QCOMPARE(domVisitorProviderRepository.size(), size_t{2});
    QVERIFY(domVisitorProviderRepository.begin() != domVisitorProviderRepository.end());

    auto it = domVisitorProviderRepository.begin();
    QCOMPARE(*it++, &provider4);
    QCOMPARE(*it++, &provider5);
}

QTEST_APPLESS_MAIN(DomVisitorProviderRepositoryTest)
#include "dom-visitor-provider-repository.test.moc"
