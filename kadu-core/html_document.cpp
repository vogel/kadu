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

#include <QtCore/QRegExp>

#include "configuration/configuration-file.h"

#include "misc/misc.h"
#include "debug.h"

#include "html_document.h"

HtmlDocument::HtmlDocument() : Elements()
{
}

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

void HtmlDocument::addElement(Element e)
{
	unescapeText(e.text);
	Elements.append(e);
}

void HtmlDocument::addTag(const QString &text)
{
	Element e;
	e.text = text;
	e.tag = true;
	Elements.append(e);
}

void HtmlDocument::addText(const QString &text)
{
	Element e;
	e.text = text;
	e.tag = false;
	Elements.append(e);
}

void HtmlDocument::insertTag(const int pos, const QString &text)
{
	Element e;
	e.text = text;
	e.tag = true;
	Elements.insert(pos,e);
}

void HtmlDocument::insertText(const int pos, const QString &text)
{
	Element e;
	e.text = text;
	e.tag = false;
	Elements.append(e);
	Elements.insert(pos,e);
}

void HtmlDocument::parseHtml(const QString &html)
{
//	kdebugm(KDEBUG_FUNCTION_START | KDEBUG_INFO, "%s\n", qPrintable(html));
	Element e;
	e.tag = false;
	int pos1, pos2;
	int len = html.length();
	for(unsigned int i = 0, htmllength = html.length(); i < htmllength; ++i)
	{
		const QChar ch = html.at(i);
		switch (ch.toAscii())
		{
			case '<':
				if (!e.tag)
				{
					if (!e.text.isEmpty())
						addElement(e);
					e.tag = true;
					e.text = ch;
				}
				break;
			case '>':
				if (e.tag)
				{
					e.text += ch;
					addElement(e);
					e.tag = false;
					e.text.clear();
				}
				break;
			default:
				pos1 = html.indexOf('>', i + 1);
				if (pos1 == -1)
					pos1 = len;
				pos2 = html.indexOf('<', i + 1);
				if (pos2 == -1)
					pos2 = len;
				if (pos2 < pos1)
					pos1 = pos2;
				e.text += html.mid(i, pos1 - i);
				i = pos1 - 1;
		}
	}
	if (!e.text.isEmpty())
		addElement(e);
}

QString HtmlDocument::generateHtml() const
{
	QString html,tmp;
	foreach(const Element &e, Elements)
	{
		tmp = e.text;
		if (!e.tag)
			escapeText(tmp);
		html += tmp;
	}
	return html;
}

int HtmlDocument::countElements() const
{
	return Elements.size();
}

bool HtmlDocument::isTagElement(int index) const
{
	return Elements[index].tag;
}

const QString & HtmlDocument::elementText(int index) const
{
	return Elements[index].text;
}

QString & HtmlDocument::elementText(int index)
{
	return Elements[index].text;
}

void HtmlDocument::setElementValue(int index, const QString &text, bool tag)
{
	Element& e=Elements[index];
	e.text=text;
	e.tag=tag;
}

void HtmlDocument::splitElement(int &index, int start, int length)
{
	Element &e = Elements[index];
	if (start > 0)
	{
		Element pre;
		pre.tag = e.tag;
		pre.text = e.text.left(start);
		Elements.insert(index, pre);
		++index;
	}
	if (start + length < e.text.length())
	{
		Element post;
		post.tag = e.tag;
		post.text = e.text.right(e.text.length() - (start+length));
		if (index + 1 < Elements.size())
			Elements.insert(index + 1, post);
		else
			Elements.append(post);
	}
	e.text = e.text.mid(start, length);
}
