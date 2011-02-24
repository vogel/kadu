/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QTextCodec>

#include "coding-conversion.h"
#include "exports.h"

KADUAPI QTextCodec *codec_cp1250 = QTextCodec::codecForName("CP1250");
KADUAPI QTextCodec *codec_latin2 = QTextCodec::codecForName("ISO8859-2");

QString cp2unicode(const QByteArray &buf)
{
	return codec_cp1250->toUnicode(buf);
}

QByteArray unicode2cp(const QString &buf)
{
	return codec_cp1250->fromUnicode(buf);
}

QString latin2unicode(const QByteArray &buf)
{
	return codec_latin2->toUnicode(buf);
}

QByteArray unicode2latin(const QString &buf)
{
	return codec_latin2->fromUnicode(buf);
}

QString unicode2std(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QChar(0x0119), 'e');
	tmp.replace(QChar(0x00f3), 'o');
	tmp.replace(QChar(0x0105), 'a');
	tmp.replace(QChar(0x015b), 's');
	tmp.replace(QChar(0x0142), 'l');
	tmp.replace(QChar(0x017c), 'z');
	tmp.replace(QChar(0x017a), 'z');
	tmp.replace(QChar(0x0107), 'c');
	tmp.replace(QChar(0x0144), 'n');
	tmp.replace(QChar(0x0118), 'E');
	tmp.replace(QChar(0x00d3), 'O');
	tmp.replace(QChar(0x0104), 'A');
	tmp.replace(QChar(0x015a), 'S');
	tmp.replace(QChar(0x0141), 'L');
	tmp.replace(QChar(0x017b), 'Z');
	tmp.replace(QChar(0x0179), 'Z');
	tmp.replace(QChar(0x0106), 'C');
	tmp.replace(QChar(0x0143), 'N');
	return tmp;
}

QString unicode2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QChar(0x0119), "%EA");
	tmp.replace(QChar(0x00f3), "%F3");
	tmp.replace(QChar(0x0105), "%B1");
	tmp.replace(QChar(0x015b), "%B6");
	tmp.replace(QChar(0x0142), "%B3");
	tmp.replace(QChar(0x017c), "%BF");
	tmp.replace(QChar(0x017a), "%BC");
	tmp.replace(QChar(0x0107), "%E6");
	tmp.replace(QChar(0x0144), "%F1");
	tmp.replace(QChar(0x0118), "%CA");
	tmp.replace(QChar(0x00d3), "%D3");
	tmp.replace(QChar(0x0104), "%A1");
	tmp.replace(QChar(0x015a), "%A6");
	tmp.replace(QChar(0x0141), "%A3");
	tmp.replace(QChar(0x017b), "%AF");
	tmp.replace(QChar(0x0179), "%AC");
	tmp.replace(QChar(0x0106), "%C3");
	tmp.replace(QChar(0x0143), "%D1");
	return tmp;
}

QString unicodeUrl2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace("%C4%99", "%EA"); //�
	tmp.replace("%C3%B3", "%F3"); //�
	tmp.replace("%C4%85", "%B1"); //�
	tmp.replace("%C5%9B", "%B6"); //�
	tmp.replace("%C5%82", "%B3"); //�
	tmp.replace("%C5%BC", "%BF"); //�
	tmp.replace("%C5%BA", "%BC"); //�
	tmp.replace("%C4%87", "%E6"); //�
	tmp.replace("%C5%84", "%F1"); //�
	tmp.replace("%C4%98", "%CA"); //�
	tmp.replace("%C3%93", "%D3"); //�
	tmp.replace("%C4%84", "%A1"); //�
	tmp.replace("%C5%9A", "%A6"); //�
	tmp.replace("%C5%81", "%A3"); //�
	tmp.replace("%C5%BB", "%AF"); //�
	tmp.replace("%C5%B9", "%AC"); //�
	tmp.replace("%C4%86", "%C3"); //�
	tmp.replace("%C5%83", "%D1"); //�
	return tmp;
}
