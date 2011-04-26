/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <io.h>
#else
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <libgadu.h>

#include "debug.h"

#include "gadu-resolver.h"

#ifdef Q_OS_WIN
int pipe(int *fds)
{
	int sock_fd;
	struct sockaddr_in sock_addr;
	int sock_addr_len = sizeof(sock_addr);

	fds[0] = fds[1] = -1;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return -1;
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = 0;
	sock_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (bind(sock_fd, (SOCKADDR *)&sock_addr, sock_addr_len) < 0)
	{
		closesocket(sock_fd);
		return -1;
	}

	if (listen(sock_fd, 1) < 0)
	{
		closesocket(sock_fd);
		return -1;
	}

	if (getsockname(sock_fd, (SOCKADDR *)&sock_addr, &sock_addr_len) < 0)
	{
		closesocket(sock_fd);
		return -1;
	}

	fds[1] = socket(AF_INET, SOCK_STREAM, 0);
	if (fds[1] < 0)
	{
		closesocket(sock_fd);
		return -1;
	}

	if (connect(fds[1], (SOCKADDR *)&sock_addr, sock_addr_len) < 0)
	{
		closesocket(fds[1]);
		closesocket(sock_fd);
		fds[1] = -1;
		return -1;
	}

	fds[0] = accept(sock_fd, (SOCKADDR *)&sock_addr, &sock_addr_len);
	if (fds[0] < 0)
	{
		closesocket(fds[1]);
		closesocket(sock_fd);
		fds[1] = -1;
		return -1;
	}

	closesocket(sock_fd);
	return 0;
}
#endif

GaduResolver::GaduResolver(gadu_resolver_data *data, QObject *parent) :
		QObject(parent), LookupId(-1), Data(data)
{
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(abort()));
}

GaduResolver::~GaduResolver()
{
	reset();
}

void GaduResolver::reset()
{
	if (Timer->isActive())
		Timer->stop();

	if (LookupId >= 0)
	{
		QHostInfo::abortHostLookup(LookupId);
		LookupId = -1;
	}
}

void GaduResolver::abort()
{
	reset();

	deleteLater();
}

void GaduResolver::resolve(const QString &hostname)
{
	reset();

	Timer->start(10000);
	LookupId = QHostInfo::lookupHost(hostname, this, SLOT(resolved(QHostInfo)));
}

void GaduResolver::resolved(const QHostInfo &host)
{
	struct in_addr addr[6];
	int count = 0;
	int i, size;

	if (Timer->isActive())
		Timer->stop();

	if (host.error() == QHostInfo::NoError)
	{
		QList<QHostAddress> addr_list = host.addresses();
		for (i = 0; i < addr_list.size(); ++i) {
			addr[count++].s_addr = htonl(host.addresses().at(i).toIPv4Address());
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Address[%d] = %s\n", i, 
				qPrintable(host.addresses().at(i).toString()));
			if (count == 5) break;
		}
		addr[i].s_addr = INADDR_NONE;
	}
	else
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Error while resolving: %s\n", qPrintable(host.errorString()));
		addr[0].s_addr = INADDR_NONE;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Returning %d addresses\n", count);
	size = sizeof(struct in_addr) * (count + 1);
#ifdef Q_OS_WIN
	if (send(Data->wfd, (const char *)&addr, size, 0) != size)
#else
	if (write(Data->wfd, &addr, size) != size)
#endif
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Writing to pipe failed\n");
	}

	LookupId = -1;

	deleteLater();
}

void gadu_resolver_cleanup(void **priv_data, int force)
{
	Q_UNUSED(force)

	if (!priv_data || !*priv_data)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "data is null. nothing to do\n");
		return;
	}

	struct gadu_resolver_data *data = static_cast<struct gadu_resolver_data *>(*priv_data);
	*priv_data = 0;

	if (data->wfd != -1)
	{
		close(data->wfd);
		data->wfd = -1;
	}
	delete data;
}

int gadu_resolver_start(int *fd, void **priv_data, const char *hostname)
{
	int pipes[2];

	if (pipe(pipes) == -1)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unable to create pipes\n");
		return -1;
	}

	struct gadu_resolver_data *data = new struct gadu_resolver_data;
	if (!data)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unable to allocate data\n");
		return -1;
	}

	data->rfd = pipes[0];
	data->wfd = pipes[1];

	GaduResolver *resolver = new GaduResolver(data);
	resolver->resolve(QString(hostname));

	*fd = pipes[0];
	*priv_data = data;

	return 0;
}
