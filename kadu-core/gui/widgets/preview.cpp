/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QHBoxLayout>

#include "core/core.h"
#include "gui/widgets/kadu-web-view.h"
#include "parser/parser.h"

#include "preview.h"

#define PREVIEW_DEFAULT_HEIGHT 250

Preview::Preview(QWidget *parent) :
		QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setFixedHeight(PREVIEW_DEFAULT_HEIGHT);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	WebView = new KaduWebView(this);
	layout->addWidget(WebView);

	QPalette p = palette();
	p.setBrush(QPalette::Base, Qt::transparent);
	WebView->page()->setPalette(p);
	WebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

Preview::~Preview()
{
}

KaduWebView * Preview::webView() const
{
	return WebView;
}

void Preview::syntaxChanged(const QString &content)
{
	QString syntax = content;
	QString text = Parser::parse(syntax, Talkable(Buddy::dummy()), ParserEscape::HtmlEscape);
	emit needFixup(text);

	WebView->setHtml(text);
}

#include "moc_preview.cpp"
