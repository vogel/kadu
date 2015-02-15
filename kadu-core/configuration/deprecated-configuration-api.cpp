/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "deprecated-configuration-api.h"

#include "configuration/configuration-api.h"
#include "misc/misc.h"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtWidgets/QApplication>

static QMutex GlobalMutex;

DeprecatedConfigurationApi::DeprecatedConfigurationApi(ConfigurationApi *xmlConfigFile, const QString &fileName) :
		m_xmlConfigFile{xmlConfigFile},
		m_fileName{fileName.section('/', -1)}
{
}

bool DeprecatedConfigurationApi::changeEntry(const QString &group, const QString &name, const QString &value)
{
	QMutexLocker locker(&GlobalMutex);

	auto root_elem = m_xmlConfigFile->rootElement();
	auto deprecated_elem = m_xmlConfigFile->accessElement(root_elem, "Deprecated");
	auto config_file_elem = m_xmlConfigFile->accessElementByFileNameProperty(deprecated_elem, "ConfigFile", "name", m_fileName);
	auto group_elem = m_xmlConfigFile->accessElementByProperty(config_file_elem, "Group", "name", group);
	auto entry_elem = m_xmlConfigFile->accessElementByProperty(group_elem, "Entry", "name", name);
	entry_elem.setAttribute("value", value);

	return true;
}

QString DeprecatedConfigurationApi::getEntry(const QString &group, const QString &name) const
{
	QMutexLocker locker(&GlobalMutex);

	auto root_elem = m_xmlConfigFile->rootElement();
	auto deprecated_elem = m_xmlConfigFile->findElement(root_elem, "Deprecated");

	if (deprecated_elem.isNull())
		return {};

	auto config_file_elem = m_xmlConfigFile->findElementByFileNameProperty(deprecated_elem, "ConfigFile", "name", m_fileName);
	if (config_file_elem.isNull())
		return {};

	auto group_elem = m_xmlConfigFile->findElementByProperty(config_file_elem, "Group", "name", group);
	if (group_elem.isNull())
		return {};

	auto entry_elem = m_xmlConfigFile->findElementByProperty(group_elem, "Entry", "name", name);
	return entry_elem.attribute("value");
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const QString &value)
{
	changeEntry(group, name, value);
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const char *value)
{
	changeEntry(group, name, QString::fromLocal8Bit(value));
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const int value)
{
	changeEntry(group, name, QString::number(value));
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const bool value)
{
	changeEntry(group, name, value ? "true" : "false");
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const QRect &value)
{
	changeEntry(group, name, rectToString(value));
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const QColor &value)
{
	changeEntry(group, name, value.name());
}

void DeprecatedConfigurationApi::writeEntry(const QString &group,const QString &name, const QFont &value)
{
	changeEntry(group, name, value.toString());
}

QString DeprecatedConfigurationApi::readEntry(const QString &group,const QString &name, const QString &def) const
{
	auto string = getEntry(group, name);
	if (string.isNull())
		return def;
	return string;
}

unsigned int DeprecatedConfigurationApi::readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def) const
{
	auto ok = false;
	auto string = getEntry(group, name);
	if (string.isNull())
		return def;
	auto num = string.toUInt(&ok);
	if (!ok)
		return def;
	return num;
}

int DeprecatedConfigurationApi::readNumEntry(const QString &group,const QString &name, int def) const
{
	auto ok = false;
	auto string = getEntry(group, name);
	if (string.isNull())
		return def;
	auto num = string.toInt(&ok);
	if (!ok)
		return def;
	return num;
}

bool DeprecatedConfigurationApi::readBoolEntry(const QString &group,const QString &name, bool def) const
{
	auto string = getEntry(group, name);
	if (string.isNull())
		return def;
	return string=="true";
}

QRect DeprecatedConfigurationApi::readRectEntry(const QString &group,const QString &name, const QRect *def) const
{
	return stringToRect(getEntry(group, name), def);
}

QColor DeprecatedConfigurationApi::readColorEntry(const QString &group,const QString &name, const QColor *def) const
{
	auto str = getEntry(group, name);
	if (str.isNull())
		return def ? *def : QColor(0, 0, 0);
	else
		return QColor(str);
}


QFont DeprecatedConfigurationApi::readFontEntry(const QString &group,const QString &name, const QFont *def) const
{
	auto string = getEntry(group, name);
	if (string.isNull())
		return def ? *def : QApplication::font();
	auto font = QFont{};
	if(font.fromString(string))
		return font;
	return def ? *def : QApplication::font();
}

void DeprecatedConfigurationApi::removeVariable(const QString &group, const QString &name)
{
	QMutexLocker locker(&GlobalMutex);

	auto root_elem = m_xmlConfigFile->rootElement();
	auto deprecated_elem = m_xmlConfigFile->accessElement(root_elem, "Deprecated");
	auto config_file_elem = m_xmlConfigFile->accessElementByFileNameProperty(deprecated_elem, "ConfigFile", "name", m_fileName);
	auto group_elem = m_xmlConfigFile->accessElementByProperty(config_file_elem, "Group", "name", group);
	auto entry_elem = m_xmlConfigFile->accessElementByProperty(group_elem, "Entry", "name", name);
	group_elem.removeChild(entry_elem);
}

void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const QString &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const char *defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const int defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const bool defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const QColor &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
void DeprecatedConfigurationApi::addVariable(const QString &group, const QString &name, const QFont &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group, name, defvalue);
}
