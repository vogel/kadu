/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message-html-renderer-service.h"
#include "dom/dom-processor-service.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message.h"

#include <QtTest/QtTest>
#include <injeqt/injector.h>
#include <injeqt/module.h>

class MessageHtmlRendererServiceTest : public QObject
{
    Q_OBJECT

private:
    injeqt::injector makeInjector() const;

private slots:
    void shouldUsePreWrapCssStyle();
};

class DomProcessorServiceStub : public DomProcessorService
{
    Q_OBJECT

public:
    Q_INVOKABLE DomProcessorServiceStub()
    {
    }
    virtual QString process(const QString &xml) override
    {
        return xml;
    }
};

injeqt::injector MessageHtmlRendererServiceTest::makeInjector() const
{
    class module : public injeqt::module
    {
    public:
        module()
        {
            add_type<DomProcessorServiceStub>();
            add_type<MessageHtmlRendererService>();
        }
    };

    auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
    modules.emplace_back(std::make_unique<module>());

    return injeqt::injector{std::move(modules)};
}

void MessageHtmlRendererServiceTest::shouldUsePreWrapCssStyle()
{
    auto injector = makeInjector();
    auto messageHtmlRendererService = injector.get<MessageHtmlRendererService>();
    auto message = Message{new MessageShared{}};
    message.setContent(normalizeHtml(plainToHtml("html content")));

    auto renderedMessage = messageHtmlRendererService->renderMessage(message);
    QVERIFY(renderedMessage.contains(R"(html content)"));
    QVERIFY(renderedMessage.contains(R"(style="white-space: pre-wrap;")"));
}

QTEST_APPLESS_MAIN(MessageHtmlRendererServiceTest)
#include "message-html-renderer-service.test.moc"
