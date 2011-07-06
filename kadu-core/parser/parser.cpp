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

	RegisteredBuddyOrContactTags.insert(name, func);

	kdebugf2();
	return true;
}

bool Parser::unregisterTag(const QString &name)
{
	kdebugf();

	if (!RegisteredBuddyOrContactTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s not registered!\n", qPrintable(name));
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

	RegisteredObjectTags.insert(name, func);

	kdebugf2();
	return true;
}

bool Parser::unregisterObjectTag(const QString &name)
{
	kdebugf();

	if (!RegisteredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s not registered!\n", qPrintable(name));
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

QString Parser::parse(const QString &s, const QObject * const object, bool escape)
{
	return parse(s, BuddyOrContact(), object, escape);
}

QString Parser::parse(const QString &s, BuddyOrContact buddyOrContact, bool escape)
{
	return parse(s, buddyOrContact, 0, escape);
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

	Buddy buddy = buddyOrContact.buddy();
	Contact contact = buddyOrContact.contact();
	QList<ParserToken> parseStack;
	int i, len = s.length();
	for (int index = 0; index < len; index = i)
	{
		ParserToken pe1, pe;

		for (i = index; i < len; ++i)
			if (searchChars.value(s.at(i), false))
				break;

		if (i != index)
		{
			pe1.Type = ParserToken::PT_STRING;
			pe1.Content = s.mid(index, i - index);

			parseStack.append(pe1);

			if (i == len)
				break;
		}

		const QChar c(s.at(i));
		if (c == '%')
		{
			++i;
			if (i == len)
				break;

			pe.Type = ParserToken::PT_STRING;

			switch (s.at(i).toAscii())
			{
				case 's':
					++i;

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

					break; // TODO: 't' removed
				case 'q':
					++i;

					if (contact)
					{
						StatusContainer *container = contact.contactAccount().statusContainer();
						if (container)
							pe.Content = container->statusIcon(contact.currentStatus().type()).path();
					}

					break;
				case 'd':
					++i;

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
					++i;

					if (contact)
						pe.Content = contact.address().toString();

					break;
				case 'v':
					++i;

					if (contact)
						pe.Content = contact.dnsName();

					break;
				case 'o':
					++i;

					// it does not work so we should just ignore it
					// see: http://kadu.net/mantis/view.php?id=2199
					break;
				case 'p':
					++i;

					if (contact && contact.port())
						pe.Content = QString::number(contact.port());

					break;
				case 'u':
					++i;

					if (contact)
						pe.Content = contact.id();
					else if (buddy)
						pe.Content = buddy.mobile().isEmpty() ? buddy.email() : buddy.mobile();

					break;
				case 'h':
					++i;

					if (contact && !contact.currentStatus().isDisconnected())
						pe.Content = contact.protocolVersion();

					break;
				case 'n':
					++i;

					pe.Content = buddy.nickName();
					if (escape)
						HtmlDocument::escapeText(pe.Content);

					break;
				case 'a':
					++i;

					pe.Content = buddy.display();
					if (escape)
						HtmlDocument::escapeText(pe.Content);

					break;
				case 'f':
					++i;

					pe.Content = buddy.firstName();
					if (escape)
						HtmlDocument::escapeText(pe.Content);

					break;
				case 'r':
					++i;

					pe.Content = buddy.lastName();
					if (escape)
						HtmlDocument::escapeText(pe.Content);

					break;
				case 'm':
					++i;

					pe.Content = buddy.mobile();

					break;
				case 'g':
				{
					++i;

					QStringList groups;
					foreach (const Group &group, buddy.groups())
						groups << group.name();

					pe.Content = groups.join(",");

					break;
				}
				case 'e':
					++i;

					pe.Content = buddy.email();

					break;
				case 'x':
					++i;

					if (contact)
						pe.Content = QString::number(contact.maximumImageSize());

					break;
				case 'z':
					++i;

					if (buddy)
						pe.Content = QString::number(buddy.gender());

					break;
				case '%':
					++i;
					// fall through
				default:
					pe.Content = '%';

					break;
			}

			parseStack.append(pe);
		}
		else if (c == '[')
		{
			++i;
			if (i == len)
				break;

			if (s.at(i) == '!')
			{
				pe.Type = ParserToken::PT_CHECK_ANY_NULL;
				++i;
			}
			else
				pe.Type = ParserToken::PT_CHECK_ALL_NOT_NULL;

			parseStack.append(pe);
		}
		else if (c == ']')
		{
			++i;

			bool found = false;
			if (!parseStack.isEmpty())
			{
				QList<ParserToken>::const_iterator begin = parseStack.constBegin();
				QList<ParserToken>::const_iterator it = parseStack.constEnd();

				while (!found && it != begin)
				{
					--it;
					ParserToken::ParserTokenType t = it->Type;

					if (t == ParserToken::PT_STRING)
						continue;
					else if (t == ParserToken::PT_CHECK_ALL_NOT_NULL || t == ParserToken::PT_CHECK_ANY_NULL)
						found = true;
					else
						break;
				}
			}

			if (!found)
			{
				pe.Content = ']';
				pe.Type = ParserToken::PT_STRING;

				parseStack.append(pe);
			}
			else
			{
				bool anyNull = false;
				while (!parseStack.empty())
				{
					const ParserToken &pe2 = parseStack.at(parseStack.length() - 1);

					if (pe2.Type == ParserToken::PT_STRING)
					{
						anyNull = anyNull || pe2.Content.isEmpty();
						pe.Content.prepend(pe2.Content);

						parseStack.removeLast();
					}
					else if (pe2.Type == ParserToken::PT_CHECK_ALL_NOT_NULL)
					{
						parseStack.removeLast();

						if (!anyNull)
						{
							pe.Type = ParserToken::PT_STRING;

							parseStack.append(pe);
						}

						break;
					}
					else if (pe2.Type == ParserToken::PT_CHECK_ANY_NULL)
					{
						parseStack.removeLast();

						if (anyNull)
						{
							pe.Type = ParserToken::PT_STRING;

							parseStack.append(pe);
						}

						break;
					}
				}
			}
		}
		else if (c == '{')
		{
			++i;
			if (i == len)
				break;

			if (s.at(i) == '!' || s.at(i) == '~')
			{
				++i;
				pe.Type = ParserToken::PT_CHECK_FILE_NOT_EXISTS;
			}
			else
				pe.Type = ParserToken::PT_CHECK_FILE_EXISTS;

			parseStack.append(pe);
		}
		else if (c == '}')
		{
			++i;

			bool found = false;
			if (!parseStack.isEmpty())
			{
				QList<ParserToken>::const_iterator begin = parseStack.constBegin();
				QList<ParserToken>::const_iterator it = parseStack.constEnd();

				while (!found && it != begin)
				{
					--it;

					ParserToken::ParserTokenType t = it->Type;

					if (t == ParserToken::PT_STRING)
						continue;
					else if (t == ParserToken::PT_EXECUTE ||
							t == ParserToken::PT_CHECK_FILE_EXISTS ||
							t == ParserToken::PT_CHECK_FILE_NOT_EXISTS ||
							t == ParserToken::PT_VARIABLE ||
							t == ParserToken::PT_ICONPATH ||
							t == ParserToken::PT_EXTERNAL_VARIABLE ||
							t == ParserToken::PT_EXECUTE2)
						found = true;
					else
						break;
				}
			}

			if (!found)
			{
				pe.Content = '}';
				pe.Type = ParserToken::PT_STRING;

				parseStack.append(pe);
			}
			else
				while (!parseStack.empty())
				{
					const ParserToken &pe2 = parseStack.at(parseStack.length() - 1);

					if (pe2.Type == ParserToken::PT_STRING)
					{
						pe.Content.prepend(pe2.Content);

						parseStack.removeLast();
					}
					else if (pe.Content.contains('\n'))
					{
						pe.Type = ParserToken::PT_STRING;
						pe.Content.append('}');

						parseStack.append(pe);

						break;
					}
					else if (pe2.Type == ParserToken::PT_CHECK_FILE_EXISTS || pe2.Type == ParserToken::PT_CHECK_FILE_NOT_EXISTS)
					{
						// we need that because pe2 is a reference which will be destroyed by parseStack.removeLast()
						bool checkFileExists = (pe2.Type == ParserToken::PT_CHECK_FILE_EXISTS);

						parseStack.removeLast();

						int spacePos = pe.Content.indexOf(' ', 0);
						QString filePath;
						if (spacePos == -1)
							filePath = pe.Content;
						else
							filePath = pe.Content.left(spacePos);

						if (filePath.startsWith(QLatin1String("file://")))
							filePath = filePath.mid(7 /*strlen("file://")*/);

						if (QFile::exists(filePath) == checkFileExists)
						{
							pe.Content = pe.Content.mid(spacePos + 1);
							pe.Type = ParserToken::PT_STRING;

							parseStack.append(pe);
						}

						break;
					}
					else if (pe2.Type == ParserToken::PT_VARIABLE)
					{
						parseStack.removeLast();

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

						parseStack.append(pe);

						break;
					}
					else if (pe2.Type == ParserToken::PT_ICONPATH)
					{
						parseStack.removeLast();

						pe.Type = ParserToken::PT_STRING;
						if (pe.Content.contains(':'))
						{
							QStringList parts = pe.Content.split(':');
							pe.Content = KaduIcon(parts.at(0), parts.at(1)).webKitPath();
						}
						else
							pe.Content = KaduIcon(pe.Content).webKitPath();

						parseStack.append(pe);

						break;
					}
					else if (pe2.Type == ParserToken::PT_EXTERNAL_VARIABLE)
					{
						parseStack.removeLast();

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

						parseStack.append(pe);

						break;
					}
					else if (pe2.Type == ParserToken::PT_EXECUTE2)
					{
						parseStack.removeLast();

						pe.Type = ParserToken::PT_STRING;
						pe.Content = executeCmd(pe.Content);

						parseStack.append(pe);

						break;
					}
				}
		}
		else if (c == '`')
		{
			++i;

			if (i == len || s.at(i) != '{')
			{
				pe.Type = ParserToken::PT_EXECUTE;

				parseStack.append(pe);
			}
			else
			{
				++i;

				pe.Type = ParserToken::PT_EXECUTE2;

				parseStack.append(pe);
			}
		}
		else if (c == '\'')
		{
			++i;

			pe.Content.clear();

			bool found = false;
			if (!parseStack.isEmpty())
			{
				QList<ParserToken>::const_iterator begin = parseStack.constBegin();
				QList<ParserToken>::const_iterator it = parseStack.constEnd();

				while (!found && it != begin)
				{
					--it;

					ParserToken::ParserTokenType t = it->Type;

					if (t == ParserToken::PT_STRING)
						continue;
					else if (t == ParserToken::PT_EXECUTE)
						found = true;
					else
						break;
				}
			}

			if (!found)
			{
				pe.Content = '\'';
				pe.Type = ParserToken::PT_STRING;

				parseStack.append(pe);
			}
			else
				while (!parseStack.empty())
				{
					const ParserToken &pe2 = parseStack.at(parseStack.length() - 1);

					if (pe2.Type == ParserToken::PT_STRING)
					{
						pe.Content.prepend(pe2.Content);

						parseStack.removeLast();
					}
					else if (pe2.Type == ParserToken::PT_EXECUTE)
					{
						parseStack.removeLast();

						pe.Type = ParserToken::PT_STRING;
						pe.Content = executeCmd(pe.Content);

						parseStack.append(pe);

						break;
					}
				}
		}
		else if (c == '\\')
		{
			++i;
			if (i == len)
				break;

			pe.Type = ParserToken::PT_STRING;
			pe.Content = s.at(i);

			++i;

			parseStack.append(pe);
		}
		else if (c == '$')
		{
			++i;

			if (i == len || s.at(i) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '$';

				parseStack.append(pe);
			}
			else
			{
				++i;

				pe.Type = ParserToken::PT_VARIABLE;

				parseStack.append(pe);
			}
		}
		else if (c == '@')
		{
			++i;

			if (i == len || s.at(i) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '@';

				parseStack.append(pe);
			}
			else
			{
				++i;

				pe.Type = ParserToken::PT_ICONPATH;

				parseStack.append(pe);
			}
		}
		else if (c == '#')
		{
			++i;

			if (i == len || s.at(i) != '{')
			{
				pe.Type = ParserToken::PT_STRING;
				pe.Content = '#';

				parseStack.append(pe);
			}
			else
			{
				++i;

				pe.Type = ParserToken::PT_EXTERNAL_VARIABLE;

				parseStack.append(pe);
			}
		}
		else
		{
			kdebugm(KDEBUG_ERROR, "shit happens? %d %c (ascii %d, unicode 0x%hx)\n", i, c.toAscii(), (int)c.toAscii(), c.unicode());
		}
	}

	QString ret;
	foreach(const ParserToken &elem, parseStack)
	{
		if (elem.Type != ParserToken::PT_STRING)
		{
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", elem.Type);
		}

		switch (elem.Type)
		{
			case ParserToken::PT_STRING:
				ret += elem.Content;
				break;
			case ParserToken::PT_EXTERNAL_VARIABLE:
				ret += "#{";
				break;
			case ParserToken::PT_ICONPATH:
				ret += "@{";
				break;
			case ParserToken::PT_VARIABLE:
				ret += "${";
				break;
			case ParserToken::PT_CHECK_FILE_EXISTS:
				ret += '{';
				break;
			case ParserToken::PT_CHECK_FILE_NOT_EXISTS:
				ret += "{!";
				break;
			case ParserToken::PT_CHECK_ALL_NOT_NULL:
				ret += '[';
				break;
			case ParserToken::PT_CHECK_ANY_NULL:
				ret += "[!";
				break;
			case ParserToken::PT_EXECUTE:
				ret += '`';
				break;
			case ParserToken::PT_EXECUTE2:
				ret += "`{";
				break;
		}
	}

	kdebugm(KDEBUG_DUMP, "%s\n", qPrintable(ret));

	return ret;
}
