/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "misc.h"

void HtmlDocument::escapeText(QString& text)
{
	//UWAGA: &amp; MUSI byæ na pocz±tku!
	text.replace("&", "&amp;");
	text.replace("<", "&lt;");
	text.replace(">", "&gt;");
	text.replace("\"", "&quot;");
	text.replace("'", "&apos;");
	text.replace("  ", "&nbsp; ");
}

void HtmlDocument::unescapeText(QString& text)
{
	//UWAGA: &amp; MUSI byæ na koñcu!
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

void HtmlDocument::insertTag(const int pos,const QString &text)
{
	Element e;
	e.text = text;
	e.tag = true;
	Elements.insert(Elements.at(pos),e);
}

void HtmlDocument::insertText(const int pos,const QString &text)
{
	Element e;
	e.text = text;
	e.tag = false;
	Elements.append(e);
	Elements.insert(Elements.at(pos),e);
}

void HtmlDocument::parseHtml(const QString& html)
{
	Element e;
	e.tag = false;
	for(unsigned int i = 0, htmllength = html.length(); i < htmllength; ++i)
	{
		const QChar &ch = html[i];
		switch (ch)
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
					e.text.truncate(0);
				}
				break;
			default:
				e.text += ch;
		}
	}
	if (!e.text.isEmpty())
		addElement(e);
}

QString HtmlDocument::generateHtml() const
{
	QString html,tmp;
	CONST_FOREACH(e, Elements)
	{
		tmp = (*e).text;
		if (!(*e).tag)
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

void HtmlDocument::setElementValue(int index,const QString& text,bool tag)
{
	Element& e=Elements[index];
	e.text=text;
	e.tag=tag;
}

void HtmlDocument::splitElement(int& index,int start,int length)
{
	Element& e=Elements[index];
	if(start>0)
	{
		Element pre;
		pre.tag=e.tag;
		pre.text=e.text.left(start);
		Elements.insert(Elements.at(index),pre);
		++index;
	}
	if(uint(start+length)<e.text.length())
	{
		Element post;
		post.tag=e.tag;
		post.text=e.text.right(e.text.length()-(start+length));
		if(uint(index+1)<Elements.size())
			Elements.insert(Elements.at(index+1),post);
		else
			Elements.append(post);
	}
	e.text=e.text.mid(start,length);
}

void HtmlDocument::convertUrlsToHtml()
{
//	QRegExp url_regexp("(http://|https://|www\\.|ftp://|ftp\\.|sftp://|smb://|file:/|rsync://|mailto:)[a-zA-Z0-9\\-\\._/~?=&#\\+%:;,!@\\\\]+");
	QRegExp url_regexp(latin2unicode((const unsigned char *)"(http://|https://|www\\.|ftp://|ftp\\.|gg:|sftp://|smb://|file:/|rsync://|mailto:|svn://|svn\\+ssh://)[a-zA-Z0-9êó±¶³¿¼æñÊÓ¡¦£¯¬ÆÑ\\*\\-\\._/~?=&#\\+%\\(\\):;,!@\\\\]*"));
	for(int i = 0; i < countElements(); ++i)
	{
		if(isTagElement(i))
			continue;
		QString text=elementText(i);
		int p=url_regexp.search(text);
		if (p < 0)
			continue;
		int l=url_regexp.matchedLength();
		QString link;
		int lft = config_file.readNumEntry("Chat","LinkFoldTreshold");
		QString link2=text.mid(p,l);
		link2.replace("%20", "%2520");//obej¶cie buga w operze :|, która nie potrafi otworzyæ linka ze spacj±
		if (l-p > lft && config_file.readBoolEntry("Chat","FoldLink"))
			link="<a href=\""+link2+"\">"+text.mid(p,p+(lft/2))+"..."+text.mid(l-(lft/2),lft/2)+"</a>";
		else
			link="<a href=\""+link2+"\">"+text.mid(p,l)+"</a>";
		splitElement(i,p,l);
		setElementValue(i,link,true);
	}
}
