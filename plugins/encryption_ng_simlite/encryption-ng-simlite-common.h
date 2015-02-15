/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <stdint.h>

#define BEGIN_RSA_PRIVATE_KEY "-----BEGIN RSA PRIVATE KEY-----"
#define END_RSA_PRIVATE_KEY "-----END RSA PRIVATE KEY-----"

#define BEGIN_RSA_PUBLIC_KEY "-----BEGIN RSA PUBLIC KEY-----"
#define END_RSA_PUBLIC_KEY "-----END RSA PUBLIC KEY-----"

#define SIM_MAGIC_V1_1 0x91
#define SIM_MAGIC_V1_2 0x23

#define SIM_FLAG_SUPPORT_UTF8 0x01
#define SIM_FLAG_UTF8_MESSAGE 0x02

struct sim_message_header
{
	uint8_t init[8];
	uint8_t magicFirstPart;
	uint8_t magicSecondPart;
	uint8_t flags;
};
