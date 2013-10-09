/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "dom/ignore-links-dom-visitor.h"
#include "url-handlers/mail-url-expander.h"

#include "mail-url-dom-visitor-provider.h"

MailUrlDomVisitorProvider::MailUrlDomVisitorProvider()
{
	MailUrlExpander *expander = new MailUrlExpander(QRegExp("\\b[a-zA-Z0-9_\\.\\-]+@[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,4}\\b"));
	IgnoreLinks.reset(new IgnoreLinksDomVisitor(expander));
}

MailUrlDomVisitorProvider::~MailUrlDomVisitorProvider()
{
}

DomVisitor * MailUrlDomVisitorProvider::provide() const
{
	return IgnoreLinks.data();
}
