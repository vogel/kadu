/***************************************************************************
                          events.h  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

void eventRecvMsg(int, UinsList, unsigned char *,time_t,int=0,struct gg_msg_format * = NULL);
void ChangeUserStatus (unsigned int, int);
void eventGotUserlist(struct gg_event *);
void eventGotUserlistWithDescription(struct gg_event *);
void eventStatusChange(struct gg_event *);
