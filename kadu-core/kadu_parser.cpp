/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QApplication>
#include <QtNetwork/QHostAddress>

#include "accounts/account_manager.h"

#include "contacts/contact-account-data.h"

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons_manager.h"
#include "misc.h"

#include "kadu_parser.h"

QMap<QString, QString> KaduParser::globalVariables;
QMap<QString, QString (*)(const Contact &)> KaduParser::registeredTags;
QMap<QString, QString (*)(const QObject * const)> KaduParser::registeredObjectTags;

bool KaduParser::registerTag(const QString &name, QString (*func)(const Contact &))
{
	kdebugf();
	if (registeredTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", qPrintable(name));
		return false;
	}
	else
	{
		registeredTags.insert(name, func);
		kdebugf2();
		return true;
	}
}

bool KaduParser::unregisterTag(const QString &name, QString (* /*func*/)(const Contact &))
{
	kdebugf();
	if (!registeredTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s not registered!\n", qPrintable(name));
		return false;
	}
	else
	{
		registeredTags.remove(name);
		kdebugf2();
		return true;
	}
}

bool KaduParser::registerObjectTag(const QString &name, ObjectTagCallback func)
{
	kdebugf();
	if (registeredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", qPrintable(name));
		return false;
	}
	else
	{
		registeredObjectTags.insert(name, func);
		kdebugf2();
		return true;
	}
}

bool KaduParser::unregisterObjectTag(const QString &name, ObjectTagCallback)
{
	kdebugf();
	if (!registeredObjectTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s not registered!\n", qPrintable(name));
		return false;
	}
	else
	{
		registeredObjectTags.remove(name);
		kdebugf2();
		return true;
	}
}

QString KaduParser::executeCmd(const QString &cmd)
{
	kdebugf();

	QString s(cmd);
	s.remove(QRegExp("`|>|<"));
	s.append(" > " + ggPath("execoutput"));

	int ret = system(qPrintable(s));

	s = QString::null;

	if (ret != -1)
	{
		QFile *f = new QFile(ggPath("execoutput"));
		if (f->open(QIODevice::ReadOnly))
		{
			s = QString(f->readAll());
			f->close();
			QFile::remove(ggPath("execoutput"));
		}
		delete f;
	}

	kdebugf2();
	return s;
}

QString KaduParser::parse(const QString &s, Account *account, const Contact &contact, bool escape)
{
	return parse(s, account, contact, 0, escape);
}

QString KaduParser::parse(const QString &s, Account *account, const Contact &contact, const QObject * const object, bool escape)
{
	kdebugmf(KDEBUG_DUMP, "%s escape=%i\n", qPrintable(s), escape);
	int index = 0, i, len = s.length();
	QList<ParseElem> parseStack;

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

	while (index < len)
	{
		ParseElem pe1, pe;

		for(i = index; i < len; ++i)
			if (searchChars.value(s[i], false))
				break;

//		this is the same, but code above is muuuuch faster
//		i=s.find(QRegExp("%|`|\\{|\\[|'|\\}|\\]"), index);

		if (i != index)
		{
			pe1.type = ParseElem::PE_STRING;
			pe1.str = s.mid(index, i - index);
			parseStack.push_back(pe1);

			if (i == len)
				break;
		}

		char c = s[i].toAscii();
		if (c == '%')
		{
			++i;
			if (i == len)
				break;
			pe.type = ParseElem::PE_STRING;

			ContactAccountData *data = contact.accountData(account);

			switch (s[i].toAscii())
			{
				case 's':
					++i;
					if (data)
						pe.str = qApp->translate("UserStatus", Status::name(data->status().type()).toAscii().data());
					break;
				case 't':
					++i;
					if (data)
						pe.str = QString::number(data->status().type()); // TODO: 0.6.6
					break;
				case 'q':
					++i;
					if (data)
						pe.str = "" ; // ule.status("Gadu").pixmapName(); TODO: 0.6.6
					break;
				case 'd':
					++i;
					if (data)
						pe.str = data->status().description();

				 	if (escape)
			 			HtmlDocument::escapeText(pe.str);
					if (config_file.readBoolEntry("Look", "ShowMultilineDesc"))
					{
						pe.str.replace("\n", "<br/>");
						pe.str.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
					}
					break;
				case 'i':
					++i;
					if (data)
						pe.str = data->ip().toString();
					break;
				case 'v':
					++i;
					if (data)
						pe.str = data->dnsName();
					break;
				case 'o':
					++i;
					if (data && data->port() == 2)
						pe.str = " ";
					break;
				case 'p':
					++i;
					if (data && data->port())
						pe.str = QString::number(data->port());
					break;
				case 'u':
					++i;
					if (data)
						pe.str = data->id();
					break;
				case 'h':
					++i;
					if (data)
						if (data && !data->status().isOffline())
							pe.str = data->protocolVersion();
					break;
				case 'n':
					++i;
					pe.str = contact.nickName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'a':
					++i;
					pe.str = contact.display();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'f':
					++i;
					pe.str = contact.firstName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'r':
					++i;
					pe.str = contact.lastName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'm':
					++i;
					pe.str = contact.mobile();
					break;
				case 'g':
					++i;
					pe.str = ""; // TODO 0.6.6 contact.data("Groups").toStringList().join(",");
					break;
				case 'e':
					++i;
					pe.str = contact.email();
					break;
				case 'x':
					++i;
					//if (ule.usesProtocol("Gadu"))
						pe.str = ""; // TODO 0.6.6 QString::number(ule.protocolData("Gadu", "MaxImageSize").toUInt());
					break;
				case '%':
					++i;
				default:
					pe.str = "%";
			}
			parseStack.push_back(pe);
		}
		else if (c == '[')
		{
			++i;
			if (i == len)
				break;
			if (s[i] == '!')
			{
				pe.type = ParseElem::PE_CHECK_ANY_NULL;
				++i;
			}
			else
				pe.type = ParseElem::PE_CHECK_ALL_NOT_NULL;
			parseStack.push_back(pe);
		}
		else if (c == ']')
		{
			++i;
			bool anyNull = false;
			bool found = false;
			if (!parseStack.isEmpty())
			{
				QList<ParseElem>::const_iterator begin = parseStack.constBegin();
				QList<ParseElem>::const_iterator it = parseStack.constEnd();
				while (!found && it != begin)
				{
					--it;
					ParseElem::ParseElemType t = (*it).type;
					if (t == ParseElem::PE_STRING)
						continue;
					else if (t == ParseElem::PE_CHECK_ALL_NOT_NULL || t == ParseElem::PE_CHECK_ANY_NULL)
						found = true;
					else
						break;
				}
			}
			if (!found)
			{
				pe.str = ']';
				pe.type = ParseElem::PE_STRING;
				parseStack.push_back(pe);
			}
			else
				while (!parseStack.empty())
				{
					const ParseElem &pe2 = parseStack.last();
					if (pe2.type == ParseElem::PE_STRING)
					{
						anyNull = anyNull || pe2.str.isEmpty();
						pe.str.prepend(pe2.str);
						parseStack.pop_back();
					}
					else if (pe2.type == ParseElem::PE_CHECK_ALL_NOT_NULL)
					{
						parseStack.pop_back();
						if (!anyNull)
						{
							pe.type = ParseElem::PE_STRING;
							parseStack.push_back(pe);
						}
						break;
					}
					else if (pe2.type == ParseElem::PE_CHECK_ANY_NULL)
					{
						parseStack.pop_back();
						if (anyNull)
						{
							pe.type = ParseElem::PE_STRING;
							parseStack.push_back(pe);
						}
						break;
					}
				}
		}
		else if (c == '{')
		{
			++i;
			if (i == len)
				break;
			if (s[i] == '!' || s[i] == '~')
			{
				pe.type = ParseElem::PE_CHECK_FILE_NOT_EXISTS;
				++i;
			}
			else
				pe.type = ParseElem::PE_CHECK_FILE_EXISTS;
			parseStack.push_back(pe);
		}
		else if (c == '}')
		{
			++i;
			bool found = false;

			if (!parseStack.isEmpty())
			{
				QList<ParseElem>::const_iterator begin = parseStack.constBegin();
				QList<ParseElem>::const_iterator it = parseStack.constEnd();
				while (!found && it != begin)
				{
					--it;
					ParseElem::ParseElemType t = (*it).type;
					if (t == ParseElem::PE_STRING)
						continue;
					else if (t == ParseElem::PE_EXECUTE ||
								t == ParseElem::PE_CHECK_FILE_EXISTS ||
								t == ParseElem::PE_CHECK_FILE_NOT_EXISTS ||
								t == ParseElem::PE_VARIABLE ||
								t == ParseElem::PE_ICONPATH ||
								t == ParseElem::PE_EXTERNAL_VARIABLE ||
								t == ParseElem::PE_EXECUTE2)
						found = true;
					else
						break;
				}
			}

			if (!found)
			{
				pe.str = '}';
				pe.type = ParseElem::PE_STRING;
				parseStack.push_back(pe);
			}
			else
				while (!parseStack.empty())
				{
					const ParseElem &pe2 = parseStack.last();
					if (pe2.type == ParseElem::PE_STRING)
					{
						pe.str.prepend(pe2.str);
						parseStack.pop_back();
					}
					else if (pe2.type == ParseElem::PE_CHECK_FILE_EXISTS || pe2.type == ParseElem::PE_CHECK_FILE_NOT_EXISTS)
					{
						// zmienna potrzebna, bo pop_back() zniszczy nam zmienn� pe2, kt�r� pobrali�my przez referencj�
						bool check_file_exists = pe2.type == ParseElem::PE_CHECK_FILE_EXISTS;

						int spacePos = pe.str.indexOf(' ', 0);
						parseStack.pop_back();
						QString file;
//						kdebugm(KDEBUG_INFO, "spacePos: %d\n", spacePos);
						if (spacePos == -1)
							file = pe.str;
						else
							file = pe.str.left(spacePos);
//						kdebugm(KDEBUG_INFO, "file: %s\n", qPrintable(file));
						if (QFile::exists(file) == check_file_exists)
						{
							pe.str = pe.str.mid(spacePos + 1);
							pe.type = ParseElem::PE_STRING;
							parseStack.push_back(pe);
						}
						break;
					}
					else if (pe2.type == ParseElem::PE_VARIABLE)
					{
						parseStack.pop_back();
						pe.type = ParseElem::PE_STRING;
						if (KaduParser::globalVariables.contains(pe.str))
						{
							kdebugm(KDEBUG_INFO, "name: %s, value: %s\n", qPrintable(pe.str), qPrintable(KaduParser::globalVariables[pe.str]));
							pe.str = KaduParser::globalVariables[pe.str];
						}
						else
						{
							kdebugm(KDEBUG_WARNING, "variable %s undefined\n", qPrintable(pe.str));
							pe.str = QString::null;
						}
						parseStack.push_back(pe);
						break;
					}
					else if (pe2.type == ParseElem::PE_ICONPATH)
					{
						parseStack.pop_back();
						pe.type = ParseElem::PE_STRING;
						pe.str = icons_manager->iconPath(pe.str);
						parseStack.push_back(pe);
						break;
					}
					else if (pe2.type == ParseElem::PE_EXTERNAL_VARIABLE)
					{
						parseStack.pop_back();
						pe.type = ParseElem::PE_STRING;
						if (registeredTags.contains(pe.str))
							pe.str = registeredTags[pe.str](contact);
						else if (object && registeredObjectTags.contains(pe.str))
							pe.str = registeredObjectTags[pe.str](object);
						else
						{
							kdebugm(KDEBUG_WARNING, "tag %s not registered\n", qPrintable(pe.str));
							pe.str = QString::null;
						}
						parseStack.push_back(pe);
						break;
					}
					else if (pe2.type == ParseElem::PE_EXECUTE2)
					{
						parseStack.pop_back();
						pe.type = ParseElem::PE_STRING;
						pe.str = executeCmd(pe.str);
						parseStack.push_back(pe);
						break;
					}
				}
		}
		else if (c == '`')
		{
			++i;
			if (i == len || s[i] != '{')
			{
				pe.type = ParseElem::PE_EXECUTE;
				parseStack.push_back(pe);
			}
			else
			{
				++i;
				pe.type = ParseElem::PE_EXECUTE2;
				parseStack.push_back(pe);
			}
		}
		else if (c == '\'')
		{
			++i;
			pe.str = QString::null;
			bool found = false;
			if (!parseStack.isEmpty())
			{
				QList<ParseElem>::const_iterator begin = parseStack.constBegin();
				QList<ParseElem>::const_iterator it = parseStack.constEnd();
				while (!found && it != begin)
				{
					--it;
					ParseElem::ParseElemType t = (*it).type;
					if (t == ParseElem::PE_STRING)
						continue;
					else if (t == ParseElem::PE_EXECUTE)
						found = true;
					else
						break;
				}
			}
			if (!found)
			{
				pe.str = '\'';
				pe.type = ParseElem::PE_STRING;
				parseStack.push_back(pe);
			}
			else
				while (!parseStack.empty())
				{
					const ParseElem &pe2 = parseStack.last();
					if (pe2.type == ParseElem::PE_STRING)
					{
						pe.str.prepend(pe2.str);
						parseStack.pop_back();
					}
					else if (pe2.type == ParseElem::PE_EXECUTE)
					{
						parseStack.pop_back();
						pe.type = ParseElem::PE_STRING;
						pe.str = executeCmd(pe.str);
						parseStack.push_back(pe);
						break;
					}
				}
		}
		else if (c == '\\')
		{
			++i;
			if (i == len)
				break;
			pe.type = ParseElem::PE_STRING;
			pe.str = s[i++];
			parseStack.push_back(pe);
		}
		else if (c == '$')
		{
			++i;
			if (i == len || s[i] != '{')
			{
				pe.type = ParseElem::PE_STRING;
				pe.str = '$';
				parseStack.push_back(pe);
			}
			else
			{
				++i;
				pe.type = ParseElem::PE_VARIABLE;
				parseStack.push_back(pe);
			}
		}
		else if (c == '@')
		{
			++i;
			if (i == len || s[i] != '{')
			{
				pe.type = ParseElem::PE_STRING;
				pe.str = '@';
				parseStack.push_back(pe);
			}
			else
			{
				++i;
				pe.type = ParseElem::PE_ICONPATH;
				parseStack.push_back(pe);
			}
		}
		else if (c == '#')
		{
			++i;
			if (i == len || s[i] != '{')
			{
				pe.type = ParseElem::PE_STRING;
				pe.str = '#';
				parseStack.push_back(pe);
			}
			else
			{
				++i;
				pe.type = ParseElem::PE_EXTERNAL_VARIABLE;
				parseStack.push_back(pe);
			}
		}
		else
			kdebugm(KDEBUG_ERROR, "shit happens? %d %c %d\n", i, (char)c, (char)c);
		index = i;
	}
	QString ret;
	QString p;
	foreach(const ParseElem &elem, parseStack)
	{
		if (elem.type != ParseElem::PE_STRING)
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", elem.type);

		switch (elem.type)
		{
			case ParseElem::PE_STRING:					p = elem.str;	break;
			case ParseElem::PE_EXTERNAL_VARIABLE:		p = "#{";		break;
			case ParseElem::PE_ICONPATH:				p = "@{";		break;
			case ParseElem::PE_VARIABLE:				p = "${";		break;
			case ParseElem::PE_CHECK_FILE_EXISTS:		p = '{';		break;
			case ParseElem::PE_CHECK_FILE_NOT_EXISTS:	p = "{!";		break;
			case ParseElem::PE_CHECK_ALL_NOT_NULL:		p = '[';		break;
			case ParseElem::PE_CHECK_ANY_NULL:			p = "[!";		break;
			case ParseElem::PE_EXECUTE:					p = '`';		break;
			case ParseElem::PE_EXECUTE2:				p = "`{";		break;
		}
		ret += p;
	}
	kdebugm(KDEBUG_DUMP, "%s\n", qPrintable(ret));
	return ret;
}
