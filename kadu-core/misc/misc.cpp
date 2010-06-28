/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QDesktopWidget>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"
#include "debug.h"
#include "html_document.h"

#include "misc.h"

QFont *defaultFont;
QFontInfo *defaultFontInfo;

long int startTime, beforeExecTime, endingTime, exitingTime;
bool measureTime = false;

void saveWindowGeometry(const QWidget *w, const QString &section, const QString &name)
{

#if defined(Q_OS_MAC) || defined(Q_WS_MAEMO_5)
	/* Dorr: on Mac OS X make sure the window will not be greater than desktop what
	 * sometimes happends during widget resizing (because of bug in Qt?),
	 * on Maemo prevent from widgets greater than screen.
	 */
	QRect screen = QApplication::desktop()->availableGeometry(w);
	QRect geometry(w->geometry());
	if (geometry.height() > screen.height())
		geometry.setHeight(screen.height());
	if (geometry.width() > screen.width())
		geometry.setWidth(screen.width());
	config_file.writeEntry(section, name, geometry);
#else
	config_file.writeEntry(section, name, w->geometry());
#endif
}

void loadWindowGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
{
	QRect rect = config_file.readRectEntry(section, name);
	if ((rect.height() == 0) || (rect.width() == 0))
	{
		rect.setRect(defaultX, defaultY, defaultWidth, defaultHeight);
	}
#ifdef Q_OS_MAC
	if (rect.y() < 20)
		rect.setY(20);
#endif
	w->setGeometry(rect);
}

QString pwHash(const QString &text)
{
	QString newText = text;
	for (unsigned int i = 0, textLength = text.length(); i < textLength; ++i)
		newText[i] = QChar(text[i].unicode() ^ i ^ 1);
	return newText;
}

QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n)
{
	static const int langSize = 5;
	static const char local[][3] = {"en",
		"de",
		"fr",
		"it",
		"pl"};

	static const char name[][sizeof("English") /*length of the longest*/] = {
		QT_TR_NOOP("English"),
		QT_TR_NOOP("German"),
		QT_TR_NOOP("French"),
		QT_TR_NOOP("Italian"),
		QT_TR_NOOP("Polish")};

	for (int i = 0; i < langSize; ++i)
	{
		if (l2n)
		{
			if (locale.mid(0, 2) == local[i])
				return application->translate("@default", name[i]);
		}
		else
			if (locale == application->translate("@default", name[i]))
				return local[i];
	}
	if (l2n)
		return application->translate("@default", QT_TR_NOOP("English"));
	else
		return "en";
}

void openWebBrowser(const QString &link)
{
	kdebugf();

	QString webBrowser = config_file.readEntry("Chat", "WebBrowser", QString::null);
	if (webBrowser.isEmpty())
	{
		if (!QDesktopServices::openUrl(QUrl(link)))
		{
			MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Web browser was not specified. Visit the configuration section")), false, "32x32/dialog-warning.png");
			kdebugmf(KDEBUG_INFO, "Web browser NOT specified.\n");
			return;
		}
		else
		{
			kdebugf2();
			return;
		}
	}
	if (!webBrowser.contains("%1"))
		webBrowser.append(" \"" + link + '"');
	else
		webBrowser.replace("%1", link);

	QProcess *browser = new QProcess(qApp);
	browser->start(webBrowser);

	if (!browser->waitForStarted())
		MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")), false, "32x32/dialog-error_big.png");

	kdebugf2();
}

void openMailClient(const QString &mail)
{
	kdebugf();
	QString email = mail;

	QString mailClient = config_file.readEntry("Chat", "MailClient", QString::null);
	if (mailClient.isEmpty())
	{
		if (!mail.startsWith("mailto:"))
			email = "mailto:" + mail;

		if (!QDesktopServices::openUrl(email))
		{
			MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Mail client was not specified. Visit the configuration section")), false, "32x32/dialog-warning.png");
			kdebugmf(KDEBUG_INFO, "Mail client NOT specified.\n");
			return;
		}
		else
		{
			kdebugf2();
			return;
		}
	}

	if (email.startsWith("mailto:"))
		email.remove(0, 7); // usuwamy "mailto:", je�li zosta�o dodane jako fragment adresu

	if (mailClient.contains("%1"))
		mailClient.replace("%1", email);
	else
		mailClient.append(email);

	QProcess *mailer = new QProcess(qApp);
	mailer->start(mailClient);

	if (!mailer->waitForStarted())
		MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Mail client process. Check if the Mail client is functional")), false, "32x32/dialog-error_big.png");

	kdebugf2();
}

QString versionToName(const unsigned int version)
{
	kdebugf();

	QString name;
	switch (version)
	{
		case 0x20: name = "GG 6.0b129"; break;
		case 0x21: name = "GG 6.0b133"; break;
		case 0x22: name = "GG 6.0b140"; break;
		case 0x24: name = "GG 6.1b155/7.6b1359"; break;
		case 0x25: name = "GG 7.0b1"; break;
		case 0x26: name = "GG 7.0b20"; break;
		case 0x27: name = "GG 7.0b22"; break;
		case 0x28: name = "GG 7.5b2201"; break;
		case 0x29: name = "GG 7.6b1688"; break;
		case 0x2a: name = "GG 7.7b3315"; break;
		case 0x2d: name = "GG 8.0b4881"; break;
		case 0x2e: name = "GG 8.0b8283"; break;
		default: name = "Unknown"; break;
	}
	return name;

	kdebugf2();
}

//internal usage
static void stringHeapSortPushDown(QString *heap, int first, int last)
{
	int r = first;
	while ( r <= last / 2 ) {
		if ( last == 2 * r ) {
			if ( heap[2 * r].localeAwareCompare(heap[r])<0 )
				qSwap( heap[r], heap[2 * r] );
			r = last;
		} else {
			if ( heap[2 * r].localeAwareCompare( heap[r] )<0 && !(heap[2 * r + 1].localeAwareCompare(heap[2 * r])<0) ) {
				qSwap( heap[r], heap[2 * r] );
				r *= 2;
			} else if ( heap[2 * r + 1].localeAwareCompare( heap[r] )<0 && heap[2 * r + 1].localeAwareCompare( heap[2 * r] )<0 ) {
				qSwap( heap[r], heap[2 * r + 1] );
				r = 2 * r + 1;
			} else {
				r = last;
			}
		}
    }
}

//internal usage
static void stringHeapSortHelper( QStringList::iterator b, QStringList::iterator e, QString, uint n )
{
	QStringList::iterator insert = b;
	QString *realheap = new QString[n];
	QString *heap = realheap - 1;
	int size = 0;
	for( ; insert != e; ++insert ) {
		heap[++size] = *insert;
		int i = size;
		while( i > 1 && heap[i].localeAwareCompare(heap[i / 2])<0 ) {
			qSwap( heap[i], heap[i / 2] );
			i /= 2;
		}
	}

	for( uint i = n; i > 0; --i ) {
		*b++ = heap[1];
		if ( i > 1 ) {
			heap[1] = heap[i];
			stringHeapSortPushDown( heap, 1, (int)i - 1 );
		}
	}

	delete[] realheap;
}

void stringHeapSort(QStringList &c)
{
	if (c.begin() == c.end())
		return;
	stringHeapSortHelper(c.begin(), c.end(), *(c.begin()), (uint)c.count());
}

QList<int> toIntList(const QList<QVariant> &in)
{
	QList<int> out;
	foreach(const QVariant &it, in)
		out.append(it.toInt());
	return out;
}

QList<QVariant> toVariantList(const QList<int> &in)
{
	QList<QVariant> out;
	foreach(const int &it, in)
		out.append(QVariant(it));
	return out;
}

QRect stringToRect(const QString &value, const QRect *def)
{
	QStringList stringlist;
	QRect rect(0,0,0,0);
	int l, t, w, h;
	bool ok;

	stringlist = value.split(',', QString::SkipEmptyParts);
	if (stringlist.count() != 4)
		return def ? *def : rect;
	l = stringlist[0].toInt(&ok); if (!ok) return def ? *def : rect;
	t = stringlist[1].toInt(&ok); if (!ok) return def ? *def : rect;
	w = stringlist[2].toInt(&ok); if (!ok) return def ? *def : rect;
	h = stringlist[3].toInt(&ok); if (!ok) return def ? *def : rect;
	rect.setRect(l, t, w, h);

	return rect;
}

QString rectToString(const QRect& rect)
{
	return QString("%1,%2,%3,%4").arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
}


QString narg(const QString &s, const QString **tab, int count)
{
	kdebugf();
	QString out;
	const QChar *d = s.unicode();
	const QChar *dend = d + s.length();
	int j = 0;
	char maxc = '0' + count;
	if (count > 9)
		return QString::null;

	while (d != dend)
	{
		if (*d == '%' && d + 1 < dend && *(d + 1) >= '1' && *(d + 1) <= maxc)
		{
			out.append(QString(d - j, j));
			++d;
			out.append(*(tab[d->digitValue() - 1]));
			j = 0;
		}
		else
			++j;
		++d;
	}
	out.append(QString(d - j, j));
//	kdebugm(KDEBUG_DUMP, "out: '%s'\n", qPrintable(out));
	kdebugf2();

	return out;
}

QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6,
				const QString &arg7, const QString &arg8,
				const QString &arg9)
{
	const QString *tab[9]={&arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9};
	return narg(s, tab, 9);
}

QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4)
{
	const QString *tab[4]={&arg1, &arg2, &arg3, &arg4};
	return narg(s, tab, 4);
}
