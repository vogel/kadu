/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QStack>
#include <QtCore/QVariant>
#include <QtGui/QApplication>
#include <QtNetwork/QHostAddress>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "parser/parser-token.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "debug.h"
#include "html_document.h"
#include "icons/icons-manager.h"

#include "parser.h"

QMap<QString, QString> Parser::GlobalVariables;
QMap<QString, Parser::BuddyOrContactTagCallback> Parser::RegisteredBuddyOrContactTags;
QMap<QString, Parser::ObjectTagCallback> Parser::RegisteredObjectTags;

bool Parser::registerTag(const QString &name, BuddyOrContactTagCallback func)
{
	kdebugf();

	if (RegisteredBuddyOrContactTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", qPrintable(name));
		return false;
	}

	if (RegisteredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered (as object tag)!\n", qPrintable(name));
		return false;
	}

	RegisteredBuddyOrContactTags.insert(name, func);

	kdebugf2();
	return true;
}

bool Parser::unregisterTag(const QString &name)
{
	kdebugf();

	if (!RegisteredBuddyOrContactTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "BuddyOrContact tag %s not registered!\n", qPrintable(name));
		return false;
	}

	RegisteredBuddyOrContactTags.remove(name);

	kdebugf2();
	return true;
}

bool Parser::registerObjectTag(const QString &name, ObjectTagCallback func)
{
	kdebugf();

	if (RegisteredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", qPrintable(name));
		return false;
	}

	if (RegisteredBuddyOrContactTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered (as BuddyOrContact tag)!\n", qPrintable(name));
		return false;
	}

	RegisteredObjectTags.insert(name, func);

	kdebugf2();
	return true;
}

bool Parser::unregisterObjectTag(const QString &name)
{
	kdebugf();

	if (!RegisteredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "object tag %s not registered!\n", qPrintable(name));
		return false;
	}

	RegisteredObjectTags.remove(name);

	kdebugf2();
	return true;
}

QString Parser::executeCmd(const QString &cmd)
{
	kdebugf();

	QString s(cmd);
	// TODO: check if Qt escapes these
	s.remove(QRegExp("`|>|<"));

	QProcess executor;
	executor.start(s);
	executor.closeWriteChannel();
	
	QString ret;
	if (executor.waitForFinished())
		ret = executor.readAll();

	kdebugf2();
	return ret;
}

bool Parser::isActionParserTokenAtTop(const QStack<ParserToken> &parseStack, const QVector<ParserToken::ParserTokenType> &acceptedTokens)
{
	bool found = false;
	QStack<ParserToken>::const_iterator begin = parseStack.constBegin();
	QStack<ParserToken>::const_iterator it = parseStack.constEnd();

	while (it != begin)
	{
		--it;
		ParserToken::ParserTokenType t = it->Type;

		if (acceptedTokens.contains(t))
		{
			found = true;
			break;
		}

		if (ParserToken::PT_STRING == t)
			continue;

		break;
	}

	return found;
}

ParserToken Parser::parsePercentSyntax(const QString &s, int &idx, const BuddyOrContact &buddyOrContact, bool escape)
{
	ParserToken pe;
	pe.Type = ParserToken::PT_STRING;

	Buddy buddy = buddyOrContact.buddy();
	Contact contact = buddyOrContact.contact();

	switch (s.at(idx).toAscii())
	{
		// 'o' does not work so we should just ignore it
		// see: http://kadu.net/mantis/view.php?id=2199
		case 'o':
		// 't' was removed in commit 48d3cd65 during 0.9 (aka 0.6.6) release cycle
		case 't':
			++idx;
			break;
		case 's':
			++idx;

			if (buddy && buddy.isBlocked())
				pe.Content = qApp->translate("@default", "Blocked");
			else if (contact)
			{
				if (contact.isBlocking())
					pe.Content = qApp->translate("@default", "Blocking");
				else
				{
					StatusType *type = StatusTypeManager::instance()->statusType(contact.currentStatus().type());
					if (type)
						pe.Content = type->displayName();
				}
			}

			break;
		case 'q':
			++idx;

			if (contact)
			{
				StatusContainer *container = contact.contactAccount().statusContainer();
				if (container)
					pe.Content = container->statusIcon(contact.currentStatus().type()).path();
			}

			break;
		case 'd':
			++idx;

			if (contact)
			{
				pe.Content = contact.currentStatus().description();

				if (escape)
					HtmlDocument::escapeText(pe.Content);

				if (config_file.readBoolEntry("Look", "ShowMultilineDesc"))
				{
					pe.Content.replace('\n', QLatin1String("<br/>"));
					pe.Content.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
				}
			}

			break;
		case 'i':
			++idx;

			if (contact)
				pe.Content = contact.address().toString();

			break;
		case 'v':
			++idx;

			if (contact)
				pe.Content = contact.dnsName();

			break;
		case 'p':
			++idx;

			if (contact && contact.port())
				pe.Content = QString::number(contact.port());

			break;
		case 'u':
			++idx;

			if (contact)
				pe.Content = contact.id();
			else if (buddy)
				pe.Content = buddy.mobile().isEmpty() ? buddy.email() : buddy.mobile();

			break;
		case 'h':
			++idx;

			if (contact && !contact.currentStatus().isDisconnected())
				pe.Content = contact.protocolVersion();

			break;
		case 'n':
			++idx;

			pe.Content = buddy.nickName();
			if (escape)
				HtmlDocument::escapeText(pe.Content);

			break;
		case 'a':
			++idx;

			pe.Content = buddy.display();
			if (escape)
				HtmlDocument::escapeText(pe.Content);

			break;
		case 'f':
			++idx;

			pe.Content = buddy.firstName();
			if (escape)
				HtmlDocument::escapeText(pe.Content);

			break;
		case 'r':
			++idx;

			pe.Content = buddy.lastName();
			if (escape)
				HtmlDocument::escapeText(pe.Content);

			break;
		case 'm':
			++idx;

			pe.Content = buddy.mobile();

			break;
		case 'g':
		{
			++idx;

			QStringList groups;
			foreach (const Group &group, buddy.groups())
				groups << group.name();

			pe.Content = groups.join(",");

			break;
		}
		case 'e':
			++idx;

			pe.Content = buddy.email();

			break;
		case 'x':
			++idx;

			if (contact)
				pe.Content = QString::number(contact.maximumImageSize());

			break;
		case 'z':
			++idx;

			if (buddy)
				pe.Content = QString::number(buddy.gender());

			break;
		case '%':
			++idx;
			// fall through
		default:
			pe.Content = '%';

			break;
	}

	return pe;
}

template<typename ContainerClass>
QString Parser::joinParserTokens(const ContainerClass &parseStack)
{
	QString joined;
	foreach(const ParserToken &elem, parseStack)
	{
		if (elem.Type != ParserToken::PT_STRING)
		{
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", elem.Type);
		}

		switch (elem.Type)
		{
			case ParserToken::PT_STRING:
				joined += elem.Content;
				break;
			case ParserToken::PT_EXTERNAL_VARIABLE:
				joined += "#{";
				break;
			case ParserToken::PT_ICONPATH:
				joined += "@{";
				break;
			case ParserToken::PT_VARIABLE:
				joined += "${";
				break;
			case ParserToken::PT_CHECK_FILE_EXISTS:
				joined += '{';
				break;
			case ParserToken::PT_CHECK_FILE_NOT_EXISTS:
				joined += "{!";
				break;
			case ParserToken::PT_CHECK_ALL_NOT_NULL:
				joined += '[';
				break;
			case ParserToken::PT_CHECK_ANY_NULL:
				joined += "[!";
				break;
			case ParserToken::PT_EXECUTE:
				joined += '`';
				break;
			case ParserToken::PT_EXECUTE2:
				joined += "`{";
				break;
		}
	}

	return joined;
}

QString Parser::parse(const QString &s, BuddyOrContact buddyOrContact, const QObject * const object, bool escape)
{
	kdebugmf(KDEBUG_DUMP, "%s escape=%i\n", qPrintable(s), escape);

	static QHash<QChar, bool> searchChars;

	if (!searchChars.value('%', false))
	{
		searchChars['%'] = true;
		searchChars['['] = true;
		searchChars['{'] = true;
		searchChars['\\'] = true;
		searchChars['$'] = true;
		searchChars['@'] = true;
		searchChars['#'] = true;
		searchChars['}'] = true;
		searchChars[']'] = true;
	}

	bool allowExec = config_file.readBoolEntry("General", "AllowExecutingFromParser", false);
	searchChars['`'] = allowExec;
	searchChars['\''] = allowExec;

	QStack<ParserToken> parseStack;
	int idx = 0, len = s.length();
	while (idx < len)
	{
		ParserToken pe1, pe;

		int prevIdx = idx;
		for (; idx < len; ++idx)
			if (searchChars.value(s.at(idx), false))
				break;

		if (idx != prevIdx)
		{
			pe1.Type = ParserToken::PT_STRING;
			pe1.Content = s.mid(prevIdx, idx - prevIdx);
			parseStack.push(pe1);

			if (idx == len)
				break;
		}

		const QChar c(s.at(idx));
		if (c == '%')
		{
			++idx;
			if (idx == len)
				break;

			parseStack.push(parsePercentSyntax(s, idx, buddyOrContact, escape));
		}
		else if (c == '[')
		{
			++idx;
			if (idx == len)
				break;

			if (s.at(idx) == '!')
			{
				pe.Type = ParserToken::PT_CHECK_ANY_NULL;
				++idx;
			}
			else
				pe.Type = ParserToken::PT_CHECK_ALL_NOT_NULL;

			parseStack.push(pe);
		}
		else if (c == ']')
		{
			++idx;

			QVector<ParserToken::ParserTokenType> acceptedTokens;
			acceptedTokens
					<< ParserToken::PT_CHECK_ALL_NOT_NULL
					<< ParserToken::PT_CHECK_ANY_NULL;

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.Content = ']';
				pe.Type = ParserToken::PT_STRING;

				parseStack.push(pe);
			}
			else
			{
				bool anyNull = false;
				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.Type == ParserToken::PT_CHECK_ALL_NOT_NULL)
					{
						if (!anyNull)
						{
							pe.Type = ParserToken::PT_STRING;

							parseStack.push(pe);
						}

						break;
					}

					if (pe2.Type == ParserToken::PT_CHECK_ANY_NULL)
					{
						if (anyNull)
						{
							pe.Type = ParserToken::PT_STRING;

							parseStack.push(pe);
						}

						break;
					}

					// here we know for sure that pe2.Type == ParserToken::PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call
					anyNull = anyNull || pe2.Content.isEmpty();
					pe.Content.prepend(pe2.Content);
				}
			}
		}
		else if (c == '{')
		{
			++idx;
			if (idx == len)
				break;

			if (s.at(idx) == '!' || s.at(idx) == '~')
			{
				++idx;
				pe.Type = ParserToken::PT_CHECK_FILE_NOT_EXISTS;
			}
			else
				pe.Type = ParserToken::PT_CHECK_FILE_EXISTS;

			parseStack.push(pe);
		}
		else if (c == '}')
		{
			++idx;

			QVector<ParserToken::ParserTokenType> acceptedTokens;
			acceptedTokens
					<< ParserToken::PT_EXECUTE
					<< ParserToken::PT_CHECK_FILE_EXISTS
					<< ParserToken::PT_CHECK_FILE_NOT_EXISTS
					<< ParserToken::PT_VARIABLE
					<< ParserToken::PT_ICONPATH
					<< ParserToken::PT_EXTERNAL_VARIABLE
					<< ParserToken::PT_EXECUTE2;

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.Content = '}';
				pe.Type = ParserToken::PT_STRING;

				parseStack.push(pe);
			}
			else
				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.Type == ParserToken::PT_CHECK_FILE_EXISTS || pe2.Type == ParserToken::PT_CHECK_FILE_NOT_EXISTS)
					{
						int spacePos = pe.Content.indexOf(' ', 0);
						QString filePath;
						if (spacePos == -1)
							filePath = pe.Content;
						else
							filePath = pe.Content.left(spacePos);

						if (filePath.startsWith(QLatin1String("file://")))
							filePath = filePath.mid(7 /*strlen("file://")*/);

						bool checkFileExists = (pe2.Type == ParserToken::PT_CHECK_FILE_EXISTS);
						if (QFile::exists(filePath) == checkFileExists)
						{
							pe.Content = pe.Content.mid(spacePos + 1);
							pe.Type = ParserToken::PT_STRING;

							parseStack.push(pe);
						}

						break;
					}

					if (pe2.Type == ParserToken::PT_VARIABLE)
					{
						pe.Type = ParserToken::PT_STRING;

						if (GlobalVariables.contains(pe.Content))
						{
							kdebugm(KDEBUG_INFO, "name: %s, value: %s\n", qPrintable(pe.Content), qPrintable(GlobalVariables[pe.Content]));
							pe.Content = GlobalVariables[pe.Content];
						}
						else
						{
							kdebugm(KDEBUG_WARNING, "variable %s undefined\n", qPrintable(pe.Content));
							pe.Content.clear();
						}

						parseStack.push(pe);

						break;
					}

					if (pe2.Type == ParserToken::PT_ICONPATH)
					{
						pe.Type = ParserToken::PT_STRING;
						if (pe.Content.contains(':'))
						{
							QStringList parts = pe.Content.split(':');
							pe.Content = KaduIcon(parts.at(0), parts.at(1)).webKitPath();
						}
						else
							pe.Content = KaduIcon(pe.Content).webKitPath();

						parseStack.push(pe);

						break;
					}

					if (pe2.Type == ParserToken::PT_EXTERNAL_VARIABLE)
					{
						pe.Type = ParserToken::PT_STRING;

						if (RegisteredBuddyOrContactTags.contains(pe.Content))
							pe.Content = RegisteredBuddyOrContactTags[pe.Content](buddyOrContact);
						else if (object && RegisteredObjectTags.contains(pe.Content))
							pe.Content = RegisteredObjectTags[pe.Content](object);
						else
						{
							kdebugm(KDEBUG_WARNING, "tag %s not registered\n", qPrintable(pe.Content));
							pe.Content.clear();
						}

						parseStack.push(pe);

						break;
					}

					if (pe2.Type == ParserToken::PT_EXECUTE2)
					{
						pe.Type = ParserToken::PT_STRING;
						pe.Content = executeCmd(pe.Content);

						parseStack.push(pe);

						break;
					}

					// here we know for sure that pe2.Type == ParserToken::PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call

					// do not execute any of the above actions on a multi-line string
					if (pe2.Content.contains('\n'))
					{
						pe.Type = ParserToken::PT_STRING;
						pe.Content.prepend(pe2.Content);
						pe.Content.append('}');

						parseStack.push(pe);

						break;
					}

					pe.Content.prepend(pe2.Content);
				}
		}
		else if (c == '`')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.Type = ParserToken::PT_EXECUTE;

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.Type = ParserToken::PT_EXECUTE2;

				parseStack.push(pe);
			}
		}
		else if (c == '\'')
		{
			++idx;

			pe.Content.clear();

			QVector<ParserToken::ParserTokenType> acceptedTokens(ParserToken::PT_EXECUTE);

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.Content = '\'';
				pe.Type = ParserToken::PT_STRING;

				parseStack.push(pe);
			}
			else
				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.Type == ParserToken::PT_EXECUTE)
					{
						pe.Type = ParserToken::PT_STRING;
						pe.Content = executeCmd(pe.Content);

						parseStack.push(pe);

						break;
					}

					// here we know for sure that pe2.Type == ParserToken::PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call
					pe.Content.prepend(pe2.Content);
				}
		}
		else if (c == '\\')
		{
			++idx;
			if (idx == len)
				break;

			pe.Type = ParserToken::PT_STRING;
			pe.Content = s.at(idx);

			++idx;

			parseStack.push(pe);
		}
		else if (c == '$')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '$';

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.Type = ParserToken::PT_VARIABLE;

				parseStack.push(pe);
			}
		}
		else if (c == '@')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '@';

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.Type = ParserToken::PT_ICONPATH;

				parseStack.push(pe);
			}
		}
		else if (c == '#')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '#';

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.Type = ParserToken::PT_EXTERNAL_VARIABLE;

				parseStack.push(pe);
			}
		}
		else
		{
			kdebugm(KDEBUG_ERROR, "shit happens? %d %c (ascii %d, unicode 0x%hx)\n", idx, c.toAscii(), (int)c.toAscii(), c.unicode());
		}
	}

	QString ret = joinParserTokens(parseStack);

	kdebugm(KDEBUG_DUMP, "%s\n", qPrintable(ret));

	return ret;
}
