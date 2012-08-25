/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "html_document.h"

void HtmlDocument::escapeText(QString &text)
{
	//UWAGA: &amp; MUSI by� na pocz�tku!
	text.replace('&', QLatin1String("&amp;"));
	text.replace('<', QLatin1String("&lt;"));
	text.replace('>', QLatin1String("&gt;"));
	text.replace('\"', QLatin1String("&quot;"));
	text.replace('\'', QLatin1String("&apos;"));
	text.replace("  ", "&nbsp; ");
}

void HtmlDocument::unescapeText(QString &text)
{
	//UWAGA: &amp; MUSI by� na ko�cu!
	text.replace("<span style='color:#000000;'>", "<span>");
	text.replace("&nbsp;", " ");
	text.replace("&lt;", "<");
	text.replace("&gt;", ">");
	text.replace("&quot;", "\"");
	text.replace("&apos;", "'");
	text.replace("&amp;", "&");
}
