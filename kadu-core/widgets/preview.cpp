/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "preview.h"

#include "buddies/buddy-dummy-factory.h"
#include "parser/parser.h"
#include "widgets/kadu-web-view.h"

#include <QtWidgets/QHBoxLayout>

#define PREVIEW_DEFAULT_HEIGHT 250

Preview::Preview(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setFixedHeight(PREVIEW_DEFAULT_HEIGHT);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_webView = new KaduWebView(this);
    layout->addWidget(m_webView);

    QPalette p = palette();
    p.setBrush(QPalette::Base, Qt::transparent);
    m_webView->page()->setPalette(p);
    m_webView->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

Preview::~Preview()
{
}

void Preview::setBuddyDummyFactory(BuddyDummyFactory *buddyDummyFactory)
{
    m_buddyDummyFactory = buddyDummyFactory;
}

void Preview::setParser(Parser *parser)
{
    m_parser = parser;
}

KaduWebView *Preview::webView() const
{
    return m_webView;
}

void Preview::syntaxChanged(const QString &content)
{
    QString syntax = content;
    QString text = m_parser->parse(syntax, Talkable(m_buddyDummyFactory->dummy()), ParserEscape::HtmlEscape);
    emit needFixup(text);

    m_webView->setHtml(text);
}

#include "moc_preview.cpp"
