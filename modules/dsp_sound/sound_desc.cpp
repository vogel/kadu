/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sound_desc.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

using namespace std;

int readF(int fd, char *buffer, int count)
{
	int reed=0,c;
	while (reed<count)
	{
		c=read(fd, buffer+reed, count-reed);
		if (c==-1)
			return -1;
		reed+=c;
	}
	return reed;
}

sound_desc::sound_desc(const char *path):length(0),data(NULL),channels(-1),speed(0),unit(0)
{
	int fd, count;
	char buffer[32];
	int int_length1,int_length2,int_length3, int_speed, int_buffer;
	short int int_format, int_channels, int_segment, int_unit;
	
	fd=open(path, O_RDONLY);
	if (fd==-1)
	{
		cerr<<"open "<<strerror(errno)<<endl;
		return;
	}

	count=readF(fd, buffer, 4);
	if (count!=4) {close(fd);return;}
	if (strncmp(buffer, "RIFF",4)!=0) {close(fd);return;}

	count=readF(fd, (char *)&int_length1, 4);

	count=readF(fd, buffer, 8);
	if (strncmp(buffer, "WAVEfmt ",8)!=0) {close(fd);return;}

	count=readF(fd, (char *)&int_length2, 4);
	if (count!=4) {close(fd);return;}

	count=readF(fd, (char *)&int_format, 2);
	if (count!=2) {close(fd);return;}
	if (int_format!=1){close(fd);cerr<<"compressed sounds not supported"<<endl;}
	
	count=readF(fd, (char *)&int_channels, 2);
	if (count!=2) {close(fd);return;}

	count=readF(fd, (char *)&int_speed, 4);
	if (count!=4) {close(fd);return;}

	count=readF(fd, (char *)&int_buffer, 4);
	if (count!=4) {close(fd);return;}

	count=readF(fd, (char *)&int_segment, 2);
	if (count!=2) {close(fd);return;}

	count=readF(fd, (char *)&int_unit, 2);
	if (count!=2) {close(fd);return;}

	count=readF(fd, buffer, 4);
	if (count!=4) {close(fd);return;}
	if (strncmp(buffer, "data",4)!=0) {close(fd);return;}

	count=readF(fd, (char *)&int_length3, 4);
	if (count!=4) {close(fd);return;}

	data=new char[int_length3];
	count=readF(fd, data, int_length3);
	if (count!=int_length3) {close(fd);delete data;return;}
	
	channels=int_channels;
	speed=int_speed;
	unit=int_unit;
	length=int_length3;
	
//	qWarning("l1:%d l2:%d l3:%d format:%d chann:%d speed:%d buff: %d segm:%d unit:%d\n",
//	int_length1, int_length2, int_length3, int_format, int_channels,
//	int_speed, int_buffer, int_segment, int_unit);

	close(fd);
}

sound_desc::~sound_desc()
{
	if (length!=0)
		delete data;
}

bool sound_desc::isOk()
{
	return (length!=0);
}
