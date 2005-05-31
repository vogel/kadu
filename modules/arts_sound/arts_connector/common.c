int read_all(int fd, char *buffer, int count)
{
	int /*offset = 0,*/ c;
	kdebugf();
	if (fd < 0)
		return -1;
/*	while (offset < count)
	{
		c = read(fd, buffer + offset, count - offset);
		if (c == -1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			return -1;
		}
		offset += c;
	}*/
	c = recv(fd, buffer, count, MSG_WAITALL);
	return c;
//	return offset;
}

int write_all(int fd, const char *data, int length, int chunksize)
{
	int res = 0, written = 0;
	kdebugf();
	if (fd < 0)
		return -1;
	
	while (written < length)
	{
		int towrite = (chunksize < length - written) ? chunksize : length - written;
		res = write(fd, data + written, towrite);
		if (res == -1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			if (errno == EAGAIN)
				continue;
			else
				break;
		}
		else
		{
			written += towrite;
			res = written;
		}
	}
	return res;
}

int read_line(int fd, char *data, int length)
{
	int offset = 0;
	kdebugf();
	if (fd < 0)
		return -1;
	while (offset < length)
	{
//		if (read(fd, data + offset, 1) == -1)
		if (recv(fd, data + offset, 1, MSG_WAITALL) < 1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			return -1;
		}
		if (data[offset] == '\n')
		{
			data[offset] = 0;
			return offset;
		}
		++offset;
	}
	data[length - 1] = 0;
	return -1;
}
