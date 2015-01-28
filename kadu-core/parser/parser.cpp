/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QStack>
#include <QtCore/QVariant>
#include <QtGui/QTextDocument>
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QApplication>

#include "accounts/account-manager.h"
#include "buddies/group.h"
#include "chat/model/chat-data-extractor.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "parser/parser-token.h"
#include "status/status-container-manager.h"
#include "status/status-container.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "debug.h"

#include "parser.h"

#define SEARCH_CHARS "%[{\\$@#}]"
#define EXEC_SEARCH_CHARS "`\'"

// PT_CHECK_FILE_EXISTS and PT_CHECK_FILE_NOT_EXISTS checks need space to be encoded,
// and encoding search chars shouldn't hurt also
#define ENCODE_INCLUDE_CHARS " " SEARCH_CHARS EXEC_SEARCH_CHARS

Q_GLOBAL_STATIC(QSet<QChar>, searchChars)

static void prepareSearchChars(bool forceExecSeachChars = false)
{
	QSet<QChar> &chars = *searchChars();
	if (chars.isEmpty())
		foreach (QChar c, QString(SEARCH_CHARS))
			chars.insert(c);

	bool allowExec = forceExecSeachChars || Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "AllowExecutingFromParser", false);
	foreach (QChar c, QString(EXEC_SEARCH_CHARS))
		if (allowExec)
			chars.insert(c);
		else
			chars.remove(c);
}

QMap<QString, QString> Parser::GlobalVariables;
QMap<QString, Parser::TalkableTagCallback> Parser::RegisteredTalkableTags;
QMap<QString, Parser::ObjectTagCallback> Parser::RegisteredObjectTags;

QString Parser::escape(const QString &string)
{
	prepareSearchChars(true);

	QString escaped;
	escaped.reserve(string.size() * 2);
	QSet<QChar> &chars = *searchChars();
	foreach (QChar c, string)
	{
		if (chars.contains(c))
			escaped.append('\'');
		escaped.append(c);
	}

	return escaped;
}

bool Parser::registerTag(const QString &name, TalkableTagCallback func)
{
	kdebugf();

	if (RegisteredTalkableTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", qPrintable(name));
		return false;
	}

	if (RegisteredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered (as object tag)!\n", qPrintable(name));
		return false;
	}

	RegisteredTalkableTags.insert(name, func);

	kdebugf2();
	return true;
}

bool Parser::unregisterTag(const QString &name)
{
	kdebugf();

	if (!RegisteredTalkableTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "Talkable tag %s not registered!\n", qPrintable(name));
		return false;
	}

	RegisteredTalkableTags.remove(name);

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

	if (RegisteredTalkableTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered (as Talkable tag)!\n", qPrintable(name));
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

ParserToken Parser::parsePercentSyntax(const QString &s, int &idx, const Talkable &talkable, ParserEscape escape)
{
	ParserToken pe;
	pe.setType(PT_STRING);

	Chat chat = talkable.toChat();
	Buddy buddy = talkable.toBuddy();
	Contact contact = talkable.toContact();

	switch (s.at(idx).toAscii())
	{
		// 'o' does not work so we should just ignore it
		// see bug #2199
		case 'o':
		// 't' was removed in commit 48d3cd65 during 0.9 (aka 0.6.6) release cycle
		case 't':
			++idx;
			break;
		case 's':
			++idx;

			if (buddy && buddy.isBlocked())
				pe.setContent(QCoreApplication::translate("@default", "Blocked"));
			else if (contact)
			{
				if (contact.isBlocking())
					pe.setContent(QCoreApplication::translate("@default", "Blocking"));
				else
				{
					const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(contact.currentStatus().type());
					pe.setContent(typeData.displayName());
				}
			}
			else if (chat && chat.chatAccount().statusContainer())
			{
				const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(chat.chatAccount().statusContainer()->status().type());
				pe.setContent(typeData.displayName());
			}

			break;
		case 'q':
			++idx;

			if (contact)
			{
				StatusContainer *container = contact.contactAccount().statusContainer();
				if (container)
					pe.setContent(container->statusIcon(contact.currentStatus().type()).path());
				else
					pe.setContent(StatusContainerManager::instance()->statusIcon(contact.currentStatus().type()).path());
			}
			else if (chat)
			{
				StatusContainer *container = chat.chatAccount().statusContainer();
				if (container)
					pe.setContent(container->statusIcon().path());
				else
					pe.setContent(StatusContainerManager::instance()->statusIcon(Status()).path());
			}

			break;
		case 'd':
			++idx;

			if (contact)
			{
				QString description = contact.currentStatus().description();
				if (escape == ParserEscape::HtmlEscape)
					description = Qt::escape(description);

				pe.setContent(description);

				if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowMultilineDesc"))
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

			QString nickName = chat ? ChatDataExtractor::data(chat, Qt::DisplayRole).toString() : buddy.nickName();
			if (escape == ParserEscape::HtmlEscape)
				nickName = Qt::escape(nickName);

			pe.setContent(nickName);

			break;
		}
		case 'a':
		{
			++idx;

			QString display = chat ? ChatDataExtractor::data(chat, Qt::DisplayRole).toString() : buddy.display();
			if (escape == ParserEscape::HtmlEscape)
				display = Qt::escape(display);

			pe.setContent(display);

			break;
		}
		case 'f':
		{
			++idx;

			QString firstName = buddy.firstName();
			if (escape == ParserEscape::HtmlEscape)
				firstName = Qt::escape(firstName);

			pe.setContent(firstName);

			break;
		}
		case 'r':
		{
			++idx;

			QString lastName = buddy.lastName();
			if (escape == ParserEscape::HtmlEscape)
				lastName = Qt::escape(lastName);

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
			if (chat)
				foreach (const Group &group, chat.groups())
					groups << group.name();
			else
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

QString Parser::parse(const QString &s, Talkable talkable, const ParserData * const parserData, ParserEscape escape = ParserEscape::HtmlEscape)
{
	kdebugmf(KDEBUG_DUMP, "%s htmlEscape=%i\n", qPrintable(s), escape);

	prepareSearchChars();

	QStack<ParserToken> parseStack;
	int idx = 0, len = s.length();
	while (idx < len)
	{
		ParserToken pe1, pe;

		int prevIdx = idx;
		for (; idx < len; ++idx)
			if (searchChars()->contains(s.at(idx)))
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

			pe = parsePercentSyntax(s, idx, talkable, escape);
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

#ifdef Q_OS_WIN
						if (filePath.startsWith(QLatin1String("file:///")))
							filePath = filePath.mid(static_cast<int>(qstrlen("file:///")));
#else
						if (filePath.startsWith(QLatin1String("file:///")))
							filePath = filePath.mid(static_cast<int>(qstrlen("file://")));
#endif

						bool checkFileExists = (pe2.type() == PT_CHECK_FILE_EXISTS);
						if (QFileInfo::exists(filePath) == checkFileExists)
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

						if (RegisteredTalkableTags.contains(content))
							pe.setContent(RegisteredTalkableTags[content](talkable));
						else if (parserData && RegisteredObjectTags.contains(content))
							pe.setContent(RegisteredObjectTags[content](parserData));
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
