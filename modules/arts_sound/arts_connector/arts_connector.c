/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <artsc.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <stdio.h>
#include <stdarg.h>

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))

#define MAXDEV 16

#include "../../../kadu-config.h"
#ifdef DEBUG_ENABLED
#define kdebugf() _kdebug_with_mask(__FILE__,__LINE__,"%s\n",__PRETTY_FUNCTION__)
#define kdebugmf(mask,format,args...) _kdebug_with_mask(__FILE__,__LINE__,"%s: " format,__PRETTY_FUNCTION__,##args)
#else
#define kdebugf()
#define kdebugmf(mask,format,args...)
#endif

void _kdebug_with_mask(const char* file, const int line, const char* format,...)
{
	if (0)
	{
		fprintf(stderr, "<%s:%i>\t", file, line);
		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}
}

#include "common.c"

struct aRtsSoundDevice
{
	arts_stream_t player;
	arts_stream_t recorder;
};

struct aRtsSoundDevice *devices[MAXDEV];

int openDevice(int rate, int channels)
{
	int i;
	kdebugf();
	for(i=0; i<MAXDEV; ++i)
		if (devices[i]==NULL)
		{
			devices[i] = (struct aRtsSoundDevice *) malloc(sizeof(struct aRtsSoundDevice));
			devices[i]->recorder = arts_record_stream(rate, 16, channels, "kadu_recorder");
//			devices[i]->player = NULL;
			devices[i]->player = arts_play_stream(rate, 16, channels, "kadu_player");
			return i;
		}
	return -1;
}

void closeDevice(int i)
{
	kdebugf();
	if (i<0 || i>=MAXDEV)
		return;
	if (devices[i])
	{
		if (devices[i]->player)
			arts_close_stream(devices[i]->player);
		if (devices[i]->recorder)
			arts_close_stream(devices[i]->recorder);
		free(devices[i]);
		devices[i] = NULL;
	}
}

void setFlushing(int devno, int enabled)
{
	kdebugf();
	struct aRtsSoundDevice *dev;
	if (devno<0 || devno>=MAXDEV)
		return;
	
	dev = devices[devno];
	if (dev)
	{
		if (dev->player)
		{
			if (enabled)
				arts_stream_set(dev->player, ARTS_P_BUFFER_SIZE, 0);
			else
				arts_stream_set(dev->player, ARTS_P_BUFFER_SIZE, 65536);
		}
		if (dev->recorder)
		{
			if (enabled)
				arts_stream_set(dev->recorder, ARTS_P_BUFFER_SIZE, 0);
			else
				arts_stream_set(dev->recorder, ARTS_P_BUFFER_SIZE, 65536);
		}
	}
}

void freeRes(int fd)
{
	kdebugf();
	int i;
	for (i=0; i<MAXDEV; ++i)
		closeDevice(i);
	close(fd);
	arts_free();
}

void endApp(int fd)
{
	kdebugf();
	freeRes(fd);
	exit(-10);
}

char name[100];
static void sigHandler(int s)
{
	kdebugf();
	unlink(name);
	if (s==SIGALRM)
		exit(0);
	else
		abort();
}

#define BUFSIZE 65536
int main()
{
	const char *newLine="\n";
	char buffer[BUFSIZE];
	struct sockaddr addr;
	socklen_t len;
	struct sockaddr_un unix_sock_name;
	unsigned long long int pass, p;
	int lsock, err, sock, end, buflen, i;
	int uin, channels, rate, enabled, length, offset,num;

	memset(devices, 0, MAXDEV*sizeof(struct aRtsSoundDevice));
	scanf("%d %llu %d", &uin, &pass, &num);
//sock=1;
	lsock = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (lsock == -1)
	{
		fprintf(stderr, "ERR: sock()==-1 %d %s\n", errno, strerror(errno));
		fflush(stderr);
		printf("ERROR\n");
		fflush(stdout);
		return -1;
	}
	
	unix_sock_name.sun_family = AF_LOCAL;

	sprintf(name, "/tmp/kadu-arts-connector-%d-%d", uin, num);
	strncpy(unix_sock_name.sun_path, name, sizeof(unix_sock_name.sun_path));
	unix_sock_name.sun_path[sizeof(unix_sock_name.sun_path)-1] = 0;	

	err = bind(lsock, (struct sockaddr *)&unix_sock_name, SUN_LEN(&unix_sock_name));
	if (err == -1)
	{
		fprintf(stderr, "ERR: bind(%d)==-1 %d %s\n", lsock, errno, strerror(errno));
		fflush(stderr);
		close(lsock);
		printf("ERROR\n");
		fflush(stdout);
		return -2;
	}
	printf("OK\n");
	fflush(stdout);

	signal(SIGSEGV, sigHandler);
	signal(SIGINT,  sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGALRM, sigHandler);
	
	//je¿eli nikt siê nie po³±czy w ci±gu 10 sekund, to znaczy,
	//¿e klient siê wy³o¿y³, wiêc i my zakoñczmy siê (¿eby zwolniæ gniazdko)
	alarm(10);

	err = listen(lsock, 2);
	if (err == -1)
	{
		fprintf(stderr, "ERR: listen(%d)==-1 %d %s\n", lsock, errno, strerror(errno));
		fflush(stderr);
		close(lsock);
		unlink(name);
		return -3;
	}
	
	sock = accept(lsock, &addr, &len);
	unlink(name);
	if (sock == -1)
	{
		fprintf(stderr, "ERR: accept(%d)==-1 %d %s\n", lsock, errno, strerror(errno));
		fflush(stderr);
		close(lsock);
		return -4;
	}

	alarm(0);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGINT,  SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	end = (read_line(sock, buffer, BUFSIZE)==-1 || memcmp(buffer,"PASS ", 5)!=0);
	
	if (!end)
	{
		sscanf(buffer+5, "%llu", &p);
		end = (p!=pass);
	}

	if (end)
	{
		sprintf(buffer, "go away!\n");
		write(sock, buffer, strlen(buffer));
		close(sock);
		return -5;
	}

//	raise(SIGALRM);
	err = arts_init();
	if (err!=0)
	{
		snprintf(buffer, BUFSIZE-1, "cannot initialize artsc: errorcode=%d description:%s\n", err, arts_error_text(err));
		buffer[BUFSIZE-1] = 0;
		write(sock, buffer, strlen(buffer));
		close(sock);
		return -6;
	}

	while (!end)
	{
		alarm(60*60);//po godzinie siê wy³±czamy
		struct aRtsSoundDevice *tmpdev; int devno;
		buflen = read_line(sock, buffer, BUFSIZE);
		end = (buflen==-1);
		if (!end)
		{
			switch(buffer[0])
			{
				case 'O': //OPEN RATE CHANNELS
//					printf(">>>%d\n", sscanf(buffer, "OPEN %d %d", &rate, &channels));fflush(stdout);
					if (sscanf(buffer, "OPEN %d %d", &rate, &channels)!=2)
						endApp(sock);
					sprintf(buffer, "OPENED %d\n", openDevice(rate, channels));
					if (write_all(sock, buffer, strlen(buffer), 32)==-1)
						endApp(sock);
					break;
				case 'C': //CLOSE DEVNUMBER
					if (sscanf(buffer, "CLOSE %d", &devno)!=1)
						endApp(sock);
					closeDevice(devno);
					sprintf(buffer, "CLOSED %d\n", devno);
					if (write_all(sock, buffer, strlen(buffer), 32)==-1)
						endApp(sock);
					break;
				case 'S': //SETFLUSHING DEVNUMBER 0/1
					if (sscanf(buffer, "SETFLUSHING %d %d", &devno, &enabled)!=2)
						endApp(sock);
					setFlushing(devno, enabled);
					sprintf(buffer, "FLUSHING FOR %d SET TO %d\n", devno, enabled);
					if (write_all(sock, buffer, strlen(buffer), 32)==-1)
						endApp(sock);
					break;
				case 'P': //PLAY DEVNUMBER LENGTH
					if (sscanf(buffer, "PLAY %d %d", &devno, &length)!=2)
						endApp(sock);
					offset = 0;
					err = 0;
					while (offset < length)
					{
						int c = read_all(sock, buffer, min(BUFSIZE, length - offset));
						if (c == -1)
							endApp(sock);
						offset += c;
						if (devno>=0 && devno<=MAXDEV && devices[devno] && devices[devno]->player)
							arts_write(devices[devno]->player, buffer, c);
						else
							err = 1;
					}
					sprintf(buffer, "PLAY SUCCESS: %d\n", !err);
					if (write_all(sock, buffer, strlen(buffer), BUFSIZE)==-1)
						endApp(sock);
					break;
				case 'R': //RECORD DEVNUMBER LENGTH
					if (sscanf(buffer, "RECORD %d %d", &devno, &length)!=2)
						endApp(sock);
					offset = 0;
					err = 0;
					while (offset < length)
					{
						int c;
						if (devno>=0 && devno<=MAXDEV && devices[devno] && devices[devno]->recorder)
							c = arts_read(devices[devno]->recorder, buffer, min(BUFSIZE, length - offset));
						else
						{
							//udajemy ¿e siê uda³o
							c = min(BUFSIZE, length - offset);
							memset(buffer, 0, c);
							err = 1;
						}
						if (c == -1)
							endApp(sock);
						offset += c;
						if (write_all(sock, buffer, c, BUFSIZE)==-1)
							endApp(sock);
					}
					sprintf(buffer, "RECORD SUCCESS: %d\n", !err);
					if (write_all(sock, buffer, strlen(buffer), BUFSIZE)==-1)
						endApp(sock);
					break;
				case 'Q': //QUIT
					end = 1;
					break;
				default:
					write_all(sock, "UNKNOWN COMMAND:", 16, 16);
					write_all(sock, buffer, buflen, 16);
					write_all(sock, newLine, 1, 1);
					endApp(sock);
			}
		}
	}
	freeRes(sock);
	return 0;
}
