/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qfile.h>
#include <qregexp.h>
#include <stdlib.h>

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons_manager.h"
#include "kadu_parser.h"
#include "misc.h"
#include "userlistelement.h"

QMap<QString, QString> KaduParser::globalVariables;
QMap<QString, QString (*)(const UserListElement &)> registeredTags;

bool KaduParser::registerTag(const QString &name, QString (*func)(const UserListElement &))
{
	kdebugf();
	if (registeredTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s already registered!\n", name.local8Bit().data());
		return false;
	}
	else
	{
		registeredTags.insert(name, func);
		kdebugf2();
		return true;
	}
}

bool KaduParser::unregisterTag(const QString &name, QString (* /*func*/)(const UserListElement &))
{
	kdebugf();
	if (!registeredTags.contains(name))
	{
		kdebugmf(KDEBUG_ERROR | KDEBUG_FUNCTION_END, "tag %s not registered!\n", name.local8Bit().data());
		return false;
	}
	else
	{
		registeredTags.remove(name);
		kdebugf2();
		return true;
	}
}

static QString executeCmd(const QString &cmd)
{
	if (!config_file.readBoolEntry("General", "AllowExecutingFromParser", false))
	{
		fprintf(stderr, "executing commands from parser disabled\n");
		fflush(stderr);
		return QString::null;
	}
	QString s(cmd);
	s.remove(QRegExp("`|>|<"));
	s.append(" >");
	s.append(ggPath("execoutput"));

	system(s.local8Bit());
	QFile *f = new QFile(ggPath("execoutput"));
	if (f->open(IO_ReadOnly))
	{
		s = QString(f->readAll());
		f->close();
		QFile::remove(ggPath("execoutput"));
	}
	else
		s = QString::null;
	delete f;
	return s;
}

struct ParseElem
{
	enum {PE_STRING, PE_CHECK_ALL_NOT_NULL, PE_CHECK_ANY_NULL, PE_CHECK_FILE_EXISTS, PE_CHECK_FILE_NOT_EXISTS, PE_EXECUTE, PE_VARIABLE, PE_ICONPATH, PE_EXTERNAL_VARIABLE, PE_EXECUTE2} type;
	QString str;
	ParseElem() : type(PE_STRING), str() {}
};

QString KaduParser::parse(const QString &s, const UserListElement &ule, bool escape)
{
	kdebugmf(KDEBUG_DUMP, "%s escape=%i\n", s.local8Bit().data(), escape);
	int index = 0, i, len = s.length();
	QValueList<ParseElem> parseStack;

	static bool searchChars[256] = {false};
	const QCString slatin = unicode2latin(s);
	searchChars[(unsigned char)'%'] = true;
	searchChars[(unsigned char)'`'] = true;
	searchChars[(unsigned char)'['] = true;
	searchChars[(unsigned char)'{'] = true;
	searchChars[(unsigned char)'\''] = true;
	searchChars[(unsigned char)'\\'] = true;
	searchChars[(unsigned char)'$'] = true;
	searchChars[(unsigned char)'@'] = true;
	searchChars[(unsigned char)'#'] = true;
	searchChars[(unsigned char)'}'] = true;
	searchChars[(unsigned char)']'] = true;

	while (index < len)
	{
		ParseElem pe1, pe;

		for(i = index; i < len; ++i)
			if (searchChars[(unsigned char)slatin[i]])
				break;
		if (i == len)
			i = -1;

//		this is the same, but code above is muuuuch faster
//		i=s.find(QRegExp("%|`|\\{|\\[|'|\\}|\\]"), index);

		if (i == -1)
		{
			pe1.type = ParseElem::PE_STRING;
			pe1.str = s.mid(index);
			parseStack.push_back(pe1);
			break;
		}
		if (i != index)
		{
			pe1.type = ParseElem::PE_STRING;
			pe1.str = s.mid(index, i - index);
			parseStack.push_back(pe1);
		}

		const QChar &c = s[i];
		if (c == '%')
		{
			++i;
			if (i == len)
				break;
			pe.type = ParseElem::PE_STRING;

			switch (slatin[i])
			{
				case 's':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = qApp->translate("@default", ule.status("Gadu").name().ascii());
					break;
				case 't':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.status("Gadu").name();
					break;
				case 'd':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.status("Gadu").description();

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
					if (ule.usesProtocol("Gadu") && ule.hasIP("Gadu"))
						pe.str = ule.IP("Gadu").toString();
					break;
				case 'v':
					++i;
					if (ule.usesProtocol("Gadu") && ule.hasIP("Gadu"))
						pe.str = ule.DNSName("Gadu");
					break;
				case 'o':
					++i;
					if (ule.usesProtocol("Gadu") && ule.port("Gadu") == 2)
						pe.str = " ";
					break;
				case 'p':
					++i;
					if (ule.usesProtocol("Gadu") && ule.port("Gadu"))
						pe.str = QString::number(ule.port("Gadu"));
					break;
				case 'u':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.ID("Gadu");
					break;
				case 'n':
					++i;
					pe.str = ule.nickName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'a':
					++i;
					pe.str = ule.altNick();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'f':
					++i;
					pe.str = ule.firstName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'r':
					++i;
					pe.str = ule.lastName();
					if (escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'm':
					++i;
					pe.str = ule.mobile();
					break;
				case 'g':
					++i;
					pe.str = ule.data("Groups").toStringList().join(",");
					break;
				case 'e':
					++i;
					pe.str = ule.email();
					break;
				case 'x':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = QString::number(ule.protocolData("Gadu", "MaxImageSize").toUInt());
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
					// zmienna potrzebna, bo pop_back() zniszczy nam zmienn± pe2, któr± pobrali¶my przez referencjê
					bool check_file_exists = pe2.type == ParseElem::PE_CHECK_FILE_EXISTS;

					int spacePos = pe.str.find(' ', 0);
					parseStack.pop_back();
					QString file;
//					kdebugm(KDEBUG_INFO, "spacePos: %d\n", spacePos);
					if (spacePos == -1)
						file = pe.str;
					else
						file = pe.str.left(spacePos);
//					kdebugm(KDEBUG_INFO, "file: %s\n", file.local8Bit().data());
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
						kdebugm(KDEBUG_INFO, "name: %s, value: %s\n", pe.str.local8Bit().data(), KaduParser::globalVariables[pe.str].local8Bit().data());
						pe.str = KaduParser::globalVariables[pe.str];
					}
					else
					{
						kdebugm(KDEBUG_WARNING, "variable %s undefined\n", pe.str.local8Bit().data());
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
						pe.str = registeredTags[pe.str](ule);
					else
					{
						kdebugm(KDEBUG_WARNING, "tag %s not registered\n", pe.str.local8Bit().data());
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
			pe.str = s[i];
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
	while (!parseStack.empty())
	{
		const ParseElem &last = parseStack.last();
		if (last.type == ParseElem::PE_STRING)
			ret.prepend(last.str);
		else
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", last.type);
		parseStack.pop_back();
	}
	kdebugm(KDEBUG_DUMP, "%s\n", ret.local8Bit().data());
	return ret;
}
