/*****************************************************************************
	System Info

	Copyright (c) 2007-2008 by Remko Tronçon
		      2008 by Nigmatullin Ruslan <euroelessar@gmail.com>
		      2009 by Kadu Team

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#include <QtCore/QSysInfo>
#include <QtCore/QTextStream>
#include <QtNetwork/QHostInfo>

#if defined(Q_WS_X11) || defined(Q_WS_MAC)
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_MAC
#include <CoreServices/CoreServices.h>
#endif

#include "system-info.h"

SystemInfo * SystemInfo::Instance = 0;

SystemInfo * SystemInfo::instance()
{
	if (!Instance)
		Instance = new SystemInfo();
	return Instance;
}

#if defined(Q_WS_X11)
QString SystemInfo::lsbRelease(const QStringList  &args)
{
	QStringList path = QString(qgetenv("PATH")).split(':');
	QString found;

	foreach (const QString &dirname, path)
	{
		QDir dir(dirname);
		QFileInfo cand(dir.filePath("lsb_release"));
		if (cand.isExecutable())
		{
			found = cand.absoluteFilePath();
			break;
		}
	}

	if (found.isEmpty())
		return QString();

	QProcess process;
	process.start(found, args, QIODevice::ReadOnly);

	if (!process.waitForStarted())
		return QString();   // process failed to start

	QTextStream stream(&process);
	QString ret;

	while (process.waitForReadyRead())
		ret += stream.readAll();

	process.close();
	return ret.trimmed();
}


QString SystemInfo::linuxHeuristicDetect()
{
	QString ret;

	struct utsname u;
	uname(&u);
	ret.sprintf("%s", u.sysname);

	// get description about os
	enum LinuxName
	{
		LinuxNone = 0,

		LinuxMandrake,
		LinuxDebian,
		LinuxRedHat,
		LinuxGentoo,
		LinuxSlackware,
		LinuxSuSE,
		LinuxConectiva,
		LinuxCaldera,
		LinuxLFS,

		LinuxASP, // Russian Linux distros
		LinuxALT,

		LinuxPLD, // Polish Linux distros
		LinuxAurox,
		LinuxArch
	};

	enum OsFlags
	{
		OsUseName = 0,
		OsUseFile,
		OsAppendFile
	};

	struct OsInfo
	{
		LinuxName id;
		OsFlags flags;
		QString file;
		QString name;
	} osInfo[] =
	    {
		{ LinuxALT,			OsUseFile,		"/etc/altlinux-release",	"Alt Linux"			},
		{ LinuxMandrake,	OsUseFile,		"/etc/mandrake-release",	"Mandrake Linux"	},
		{ LinuxDebian,		OsAppendFile,	"/etc/debian_version",		"Debian GNU/Linux"	},
		{ LinuxGentoo,		OsUseFile,		"/etc/gentoo-release",		"Gentoo Linux"		},
		{ LinuxSlackware,	OsAppendFile,	"/etc/slackware-version",	"Slackware Linux"	},
		{ LinuxPLD,			OsUseFile,		"/etc/pld-release",			"PLD Linux"			},
		{ LinuxAurox,		OsUseName,		"/etc/aurox-release",		"Aurox Linux"		},
		{ LinuxArch,		OsUseFile,		"/etc/arch-release",		"Arch Linux"		},
		{ LinuxLFS,			OsAppendFile,	"/etc/lfs-release",			"LFS Linux"			},

		// untested
		{ LinuxSuSE,		OsUseFile,		"/etc/SuSE-release",		"SuSE Linux"		},
		{ LinuxConectiva,	OsUseFile,		"/etc/conectiva-release",	"Conectiva Linux"	},
		{ LinuxCaldera,		OsUseFile,		"/etc/.installed",			"Caldera Linux"		},

		// many distros use the /etc/redhat-release for compatibility, so RedHat will be the last :)
		{ LinuxRedHat,		OsUseFile,		"/etc/redhat-release",		"RedHat Linux"		},

		{ LinuxNone,		OsUseName,		"",							""					}
	    };

	for (int i = 0; osInfo[i].id != LinuxNone; i++)
	{
		QFileInfo fi(osInfo[i].file);
		if (fi.exists())
		{
			char buffer[128];

			QFile f(osInfo[i].file);
			f.open(QIODevice::ReadOnly);
			f.readLine(buffer, 128);
			QString desc(buffer);

			desc = desc.simplified();//stripWhiteSpace ();

			switch (osInfo[i].flags)
			{
				case OsUseFile:
					ret = desc;
					if (!ret.isEmpty())
						break;
				case OsUseName:
					ret = osInfo[i].name;
					break;
				case OsAppendFile:
					ret = osInfo[i].name + " (" + desc + ')';
					break;
			}
			break;
		}
	}
	return ret;
}
#endif

SystemInfo::SystemInfo()
{
	// Initialize
	TimezoneOffset = 0;
	Timezone = "N/A";
	OsFullName = "Unknown";

	// Detect
#if defined(Q_WS_X11) || defined(Q_WS_MAC)
	time_t x;
	time(&x);
	char str[256];
	char fmt[32];
	strcpy(fmt, "%z");
	strftime(str, 256, fmt, localtime(&x));
	if (strcmp(fmt, str))
	{
		QString s = str;
		if(s.at(0) == '+')
			s.remove(0,1);
		s.truncate(s.length() - 2);
		TimezoneOffset = s.toInt();
	}
	strcpy(fmt, "%Z");
	strftime(str, 256, fmt, localtime(&x));
	if (strcmp(fmt, str))
		Timezone = str;
#endif
#if defined(Q_WS_X11)
	#if defined(Q_OS_FREEBSD)
		OsName = "FreeBSD";
	#elif defined(Q_OS_NETBSD)
		OsName = "NetBSD";
	#elif defined(Q_OS_OPENBSD)
		OsName = "OpenBSD";
	#elif defined(Q_OS_SOLARIS)
		OsName = "Solaris";
	#elif defined(Q_OS_LINUX)
		OsName = "Linux";
		// attempt to get LSB version before trying the distro-specific approach
		OsFullName = lsbRelease(QStringList() << "--description" << "--short");

		if (OsFullName.isEmpty())
			OsFullName = linuxHeuristicDetect();
	#endif
#elif defined(Q_WS_MAC)
	long minor_version, major_version, bug_fix;
	Gestalt(gestaltSystemVersionMajor, &major_version);
	Gestalt(gestaltSystemVersionMinor, &minor_version);
	Gestalt(gestaltSystemVersionBugFix, &bug_fix);
	OsFullName = QString("MacOS X %1.%2.%3").arg(major_version).arg(minor_version).arg(bug_fix);
	OsName = "MacOS X";
	OsVersion = QString("%1.%2.%3").arg(major_version, minor_version, bug_fix);
#endif

#if defined(Q_WS_WIN)
	TIME_ZONE_INFORMATION i;
	//GetTimeZoneInformation(&i);
	//TimezoneOffset = (-i.Bias) / 60;
	memset(&i, 0, sizeof(i));
	bool inDST = (GetTimeZoneInformation(&i) == TIME_ZONE_ID_DAYLIGHT);
	int bias = i.Bias;
	if(inDST)
		bias += i.DaylightBias;
	TimezoneOffset = -bias;
	Timezone.clear();
	for(int n = 0; n < 32; ++n)
	{
		int w = inDST ? i.DaylightName[n] : i.StandardName[n];
		if(w == 0)
			break;
		Timezone += QChar(w);
	}
	OsFullName.clear();
	OsName = "Windows";
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return;
	}

	switch (osvi.dwPlatformId)
	{
		// Check fo family Windows NT.
		case VER_PLATFORM_WIN32_NT:
		// Check product version

		if (osvi.dwMajorVersion <= 4)
			OsFullName.append(" NT");

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			OsFullName.append(" 2000");


		if (bOsVersionInfoEx)  // Use information from GetVersionEx.
		{
		// Check workstation's type
			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				{
					OsFullName.append(" XP");
					if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
						OsFullName.append(" Home Edition");
					else
						OsFullName.append(" Professional");
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
				{
					OsFullName.append(" Vista");
					if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
						OsFullName.append(" Home");
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
				{
					OsFullName.append(" 7 beta");
				}
				else if (osvi.dwMajorVersion == 7 && osvi.dwMinorVersion == 0)
				{
					OsFullName.append(" 7");
				}
				else
					OsFullName.append(QString(" NT %1.%2").arg(osvi.dwMajorVersion).arg(osvi.dwMinorVersion));
			}

		// Check server version
			else if (osvi.wProductType == VER_NT_SERVER)
			{
				if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
					OsFullName.append(" 2003");
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
					OsFullName.append(" 2003 R2");
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
					OsFullName.append(" 2008");
				else
					OsFullName.append(QString(" NT %1.%2").arg(osvi.dwMajorVersion).arg(osvi.dwMinorVersion));
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					OsFullName.append(" DataCenter Server");
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					if (osvi.dwMajorVersion == 4 )
						OsFullName.append(" Advanced Server");
					else
						OsFullName.append(" Enterprise Server");
				}
				else if (osvi.wSuiteMask == VER_SUITE_BLADE)
					OsFullName.append(" Web Server");
				else
					OsFullName.append(" Server");
			}
			else
				OsFullName = QString(" Unknown Shit %1.%2").arg(osvi.dwMajorVersion).arg(osvi.dwMinorVersion);
		}
		else	// Use register for earlier versions of Windows NT
			OsFullName.append(" NT");

		if (osvi.dwMajorVersion <= 4)
			OsFullName.append(QString(" %1.%2").arg(osvi.dwMajorVersion).arg(osvi.dwMinorVersion));

		break;

	// Check for family Windows 95.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			 OsFullName.append(" 95");
			 if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
				 OsFullName.append(" OSR2");
		}
		else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			 OsFullName.append(" 98");
			 if (osvi.szCSDVersion[1] == 'A')
				 OsFullName.append(" SE");
		}
		else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			 OsFullName.append(" Millennium Edition");
		}
		else
			OsFullName.append(" 9x/Me");
		break;
	}
	OsVersion = OsFullName.mid(1);
	OsFullName = OsFullName.prepend("Windows");
#endif
}

QString SystemInfo::localHostName()
{
	QString hostname = QHostInfo::localHostName();
	int i = hostname.indexOf('.');
	if (i != -1)
		return hostname.left(hostname.indexOf('.'));
	else
		return hostname;
}
