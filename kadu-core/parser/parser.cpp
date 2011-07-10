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

// PT_CHECK_FILE_EXISTS and PT_CHECK_FILE_NOT_EXISTS checks need space to be encoded,
// and encoding searchChars shouldn't hurt also
#define ENCODE_INCLUDE_CHARS " %[{\\$@#}]`\'"

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

bool Parser::isActionParserTokenAtTop(const QStack<ParserToken> &parseStack, const QVector<ParserTokenType> &acceptedTokens)
{
	bool found = false;
	QStack<ParserToken>::const_iterator begin = parseStack.constBegin();
	QStack<ParserToken>::const_iterator it = parseStack.constEnd();

	while (it != begin)
	{
		--it;
		ParserTokenType t = it->type();

		if (acceptedTokens.contains(t))
		{
			found = true;
			break;
		}

		if (PT_STRING == t)
			continue;

		break;
	}

	return found;
}

ParserToken Parser::parsePercentSyntax(const QString &s, int &idx, const BuddyOrContact &buddyOrContact, bool escape)
{
	ParserToken pe;
	pe.setType(PT_STRING);

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
				pe.setContent(qApp->translate("@default", "Blocked"));
			else if (contact)
			{
				if (contact.isBlocking())
					pe.setContent(qApp->translate("@default", "Blocking"));
				else
				{
					StatusType *type = StatusTypeManager::instance()->statusType(contact.currentStatus().type());
					if (type)
						pe.setContent(type->displayName());
				}
			}

			break;
		case 'q':
			++idx;

			if (contact)
			{
				StatusContainer *container = contact.contactAccount().statusContainer();
				if (container)
					pe.setContent(container->statusIcon(contact.currentStatus().type()).path());
			}

			break;
		case 'd':
			++idx;

			if (contact)
			{
				QString description = contact.currentStatus().description();
				if (escape)
					HtmlDocument::escapeText(description);

				pe.setContent(description);

				if (config_file.readBoolEntry("Look", "ShowMultilineDesc"))
				{
					QString content = pe.decodedContent();
					content.replace('\n', QLatin1String("<br/>"));
					content.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
					pe.setContent(content);
				}
			}

			break;
		case 'i':
			++idx;

			if (contact)
				pe.setContent(contact.address().toString());

			break;
		case 'v':
			++idx;

			if (contact)
				pe.setContent(contact.dnsName());

			break;
		case 'p':
			++idx;

			if (contact && contact.port())
				pe.setContent(QString::number(contact.port()));

			break;
		case 'u':
			++idx;

			if (contact)
				pe.setContent(contact.id());
			else if (buddy)
				pe.setContent(buddy.mobile().isEmpty() ? buddy.email() : buddy.mobile());

			break;
		case 'h':
			++idx;

			if (contact && !contact.currentStatus().isDisconnected())
				pe.setContent(contact.protocolVersion());

			break;
		case 'n':
		{
			++idx;

			QString nickName = buddy.nickName();
			if (escape)
				HtmlDocument::escapeText(nickName);

			pe.setContent(nickName);

			break;
		}
		case 'a':
		{
			++idx;

			QString display = buddy.display();
			if (escape)
				HtmlDocument::escapeText(display);

			pe.setContent(display);

			break;
		}
		case 'f':
		{
			++idx;

			QString firstName = buddy.firstName();
			if (escape)
				HtmlDocument::escapeText(firstName);

			pe.setContent(firstName);

			break;
		}
		case 'r':
		{
			++idx;

			QString lastName = buddy.lastName();
			if (escape)
				HtmlDocument::escapeText(lastName);

			pe.setContent(lastName);

			break;
		}
		case 'm':
			++idx;

			pe.setContent(buddy.mobile());

			break;
		case 'g':
		{
			++idx;

			QStringList groups;
			foreach (const Group &group, buddy.groups())
				groups << group.name();

			pe.setContent(groups.join(","));

			break;
		}
		case 'e':
			++idx;

			pe.setContent(buddy.email());

			break;
		case 'x':
			++idx;

			if (contact)
				pe.setContent(QString::number(contact.maximumImageSize()));

			break;
		case 'z':
			++idx;

			if (buddy)
				pe.setContent(QString::number(buddy.gender()));

			break;
		case '%':
			++idx;
			// fall through
		default:
			pe.setContent("%");

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
		if (elem.type() != PT_STRING)
		{
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", elem.type());
		}

		switch (elem.type())
		{
			case PT_STRING:
				joined += elem.decodedContent();
				break;
			case PT_EXTERNAL_VARIABLE:
				joined += "#{";
				break;
			case PT_ICONPATH:
				joined += "@{";
				break;
			case PT_VARIABLE:
				joined += "${";
				break;
			case PT_CHECK_FILE_EXISTS:
				joined += '{';
				break;
			case PT_CHECK_FILE_NOT_EXISTS:
				joined += "{!";
				break;
			case PT_CHECK_ALL_NOT_NULL:
				joined += '[';
				break;
			case PT_CHECK_ANY_NULL:
				joined += "[!";
				break;
			case PT_EXECUTE:
				joined += '`';
				break;
			case PT_EXECUTE2:
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
			pe1.setType(PT_STRING);
			pe1.setContent(s.mid(prevIdx, idx - prevIdx));
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

			pe = parsePercentSyntax(s, idx, buddyOrContact, escape);
			pe.encodeContent(QByteArray(), ENCODE_INCLUDE_CHARS);

			parseStack.push(pe);
		}
		else if (c == '[')
		{
			++idx;
			if (idx == len)
				break;

			if (s.at(idx) == '!')
			{
				pe.setType(PT_CHECK_ANY_NULL);
				++idx;
			}
			else
				pe.setType(PT_CHECK_ALL_NOT_NULL);

			parseStack.push(pe);
		}
		else if (c == ']')
		{
			++idx;

			QVector<ParserTokenType> acceptedTokens;
			acceptedTokens
					<< PT_CHECK_ALL_NOT_NULL
					<< PT_CHECK_ANY_NULL;

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.setContent("]");
				pe.setType(PT_STRING);

				parseStack.push(pe);
			}
			else
			{
				bool anyNull = false;
				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.type() == PT_CHECK_ALL_NOT_NULL)
					{
						if (!anyNull)
						{
							pe.setType(PT_STRING);

							parseStack.push(pe);
						}

						break;
					}

					if (pe2.type() == PT_CHECK_ANY_NULL)
					{
						if (anyNull)
						{
							pe.setType(PT_STRING);

							parseStack.push(pe);
						}

						break;
					}

					// here we know for sure that pe2.type() == PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call
					anyNull = anyNull || pe2.decodedContent().isEmpty();
					QString content = pe.decodedContent();
					content.prepend(pe2.decodedContent());
					pe.setContent(content);
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
				pe.setType(PT_CHECK_FILE_NOT_EXISTS);
			}
			else
				pe.setType(PT_CHECK_FILE_EXISTS);

			parseStack.push(pe);
		}
		else if (c == '}')
		{
			++idx;

			QVector<ParserTokenType> acceptedTokens;
			acceptedTokens
					<< PT_CHECK_FILE_EXISTS
					<< PT_CHECK_FILE_NOT_EXISTS
					<< PT_VARIABLE
					<< PT_ICONPATH
					<< PT_EXTERNAL_VARIABLE
					<< PT_EXECUTE2;

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.setContent("}");
				pe.setType(PT_STRING);

				parseStack.push(pe);
			}
			else
			{
				QList<ParserToken> tokens;

				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.type() == PT_CHECK_FILE_EXISTS || pe2.type() == PT_CHECK_FILE_NOT_EXISTS)
					{
						int firstSpaceTokenIdx = 0, spacePos = -1;
						foreach (const ParserToken &token, tokens)
						{
							// encoded cannot contain space
							if (!token.isEncoded())
							{
								spacePos = token.rawContent().indexOf(' ');
								if (spacePos != -1)
									break;
							}

							++firstSpaceTokenIdx;
						}

						QString filePath;
						if (spacePos == -1)
							filePath = joinParserTokens(tokens);
						else
							filePath = joinParserTokens(tokens.mid(0, firstSpaceTokenIdx)) +
									tokens.at(firstSpaceTokenIdx).rawContent().left(spacePos);

						if (filePath.startsWith(QLatin1String("file://")))
							filePath = filePath.mid(7 /*strlen("file://")*/);

						bool checkFileExists = (pe2.type() == PT_CHECK_FILE_EXISTS);
						if (QFile::exists(filePath) == checkFileExists)
						{
							pe.setType(PT_STRING);

							if (spacePos == -1)
								pe.setContent(filePath);
							else
							{
								QString content = tokens.at(firstSpaceTokenIdx).rawContent().mid(spacePos + 1) +
										joinParserTokens(tokens.mid(firstSpaceTokenIdx + 1));

								pe.setContent(content);
							}

							parseStack.push(pe);
						}

						break;
					}

					if (pe2.type() == PT_VARIABLE)
					{
						QString content = joinParserTokens(tokens);

						pe.setType(PT_STRING);

						if (GlobalVariables.contains(content))
						{
							kdebugm(KDEBUG_INFO, "name: %s, value: %s\n", qPrintable(pe.decodedContent()), qPrintable(GlobalVariables[pe.decodedContent()]));
							pe.setContent(GlobalVariables[content]);
							pe.encodeContent(QByteArray(), ENCODE_INCLUDE_CHARS);
						}
						else
						{
							kdebugm(KDEBUG_WARNING, "variable %s undefined\n", qPrintable(pe.decodedContent()));
							pe.setContent(QString());
						}

						parseStack.push(pe);

						break;
					}

					if (pe2.type() == PT_ICONPATH)
					{
						QString content = joinParserTokens(tokens);

						pe.setType(PT_STRING);
						if (content.contains(':'))
						{
							QStringList parts = content.split(':');
							pe.setContent(KaduIcon(parts.at(0), parts.at(1)).webKitPath());
						}
						else
							pe.setContent(KaduIcon(content).webKitPath());

						parseStack.push(pe);

						break;
					}

					if (pe2.type() == PT_EXTERNAL_VARIABLE)
					{
						QString content = joinParserTokens(tokens);

						pe.setType(PT_STRING);

						if (RegisteredBuddyOrContactTags.contains(content))
							pe.setContent(RegisteredBuddyOrContactTags[content](buddyOrContact));
						else if (object && RegisteredObjectTags.contains(content))
							pe.setContent(RegisteredObjectTags[content](object));
						else
						{
							kdebugm(KDEBUG_WARNING, "tag %s not registered\n", qPrintable(pe.decodedContent()));
							pe.setContent(QString());
						}

						pe.encodeContent(QByteArray(), ENCODE_INCLUDE_CHARS);
						parseStack.push(pe);

						break;
					}

					if (pe2.type() == PT_EXECUTE2)
					{
						pe.setType(PT_STRING);
						pe.setContent(executeCmd(joinParserTokens(tokens)));

						parseStack.push(pe);

						break;
					}

					// here we know for sure that pe2.type() == PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call
					tokens.prepend(pe2);
				}
			}
		}
		else if (c == '`')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.setType(PT_EXECUTE);

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.setType(PT_EXECUTE2);

				parseStack.push(pe);
			}
		}
		else if (c == '\'')
		{
			++idx;

			pe.setContent(QString());

			QVector<ParserTokenType> acceptedTokens(PT_EXECUTE);

			if (!isActionParserTokenAtTop(parseStack, acceptedTokens))
			{
				pe.setContent("\'");
				pe.setType(PT_STRING);

				parseStack.push(pe);
			}
			else
				while (!parseStack.empty())
				{
					ParserToken pe2 = parseStack.pop();

					if (pe2.type() == PT_EXECUTE)
					{
						pe.setType(PT_STRING);
						pe.setContent(executeCmd(pe.decodedContent()));

						parseStack.push(pe);

						break;
					}

					// here we know for sure that pe2.type() == PT_STRING,
					// as it is guaranteed by isActionParserTokenAtTop() call
					QString content = pe.decodedContent();
					content.prepend(pe2.decodedContent());
					pe.setContent(content);
				}
		}
		else if (c == '\\')
		{
			++idx;
			if (idx == len)
				break;

			pe.setType(PT_STRING);
			pe.setContent(s.at(idx));

			++idx;

			parseStack.push(pe);
		}
		else if (c == '$')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.setType(PT_STRING);
				pe.setContent("$");

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.setType(PT_VARIABLE);

				parseStack.push(pe);
			}
		}
		else if (c == '@')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.setType(PT_STRING);
				pe.setContent("@");

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.setType(PT_ICONPATH);

				parseStack.push(pe);
			}
		}
		else if (c == '#')
		{
			++idx;

			if (idx == len || s.at(idx) != '{')
			{
				pe.setType(PT_STRING);
				pe.setContent("#");

				parseStack.push(pe);
			}
			else
			{
				++idx;

				pe.setType(PT_EXTERNAL_VARIABLE);

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
