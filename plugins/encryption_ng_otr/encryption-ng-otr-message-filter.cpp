/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#	include <libotr/userstate.h>
}

#include "contacts/contact.h"
#include "formatted-string/formatted-string.h"
#include "message/message.h"

#include "encryption-ng-otr-message-filter.h"

OtrlPolicy kadu_enomf_policy(void *opdata, ConnContext *context)
{
	printf("kadu_enomf_policy %p %p\n", opdata, context);
	return OTRL_POLICY_DEFAULT;
}

void kadu_enomf_create_privkey(void *opdata, const char *accountname, const char *protocol)
{
	printf("kadu_enomf_create_privkey %p %s %s\n", opdata, accountname, protocol);
}

int kadu_enomf_is_logged_in(void *opdata, const char *accountname, const char *protocol, const char *recipient)
{
	printf("kadu_enomf_is_logged_in %p %s %s %s\n", opdata, accountname, protocol, recipient);
	return -1;
}

void kadu_enomf_inject_message(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message)
{
	printf("kadu_enomf_inject_message %p %s %s %s %s\n", opdata, accountname, protocol, recipient, message);
}

void kadu_enomf_notify(void *opdata, OtrlNotifyLevel level, const char *accountname, const char *protocol, const char *username,
					   const char *title, const char *primary, const char *secondary)
{
	printf("kadu_enomf_notify %p %d %s %s %s %s %s %s\n", opdata, level, accountname, protocol, username, title, primary, secondary);
}

int kadu_enomf_display_otr_message(void *opdata, const char *accountname, const char *protocol, const char *username, const char *msg)
{
	printf("kadu_enomf_display_otr_message %p %s %s %s %s %s\n", opdata, accountname, protocol, username, username, msg);
	return 0;
}

void kadu_enomf_update_context_list(void *opdata)
{
	printf("kadu_enomf_update_context_list %p\n", opdata);
}

const char * kadu_enomf_protocol_name(void *opdata, const char *protocol)
{
	printf("kadu_enomf_protocol_name %p %s\n", opdata, protocol);
	return protocol;
}

void kadu_enomf_protocol_name_free(void *opdata, const char *protocol_name)
{
	printf("kadu_enomf_protocol_name_free %p %s\n", opdata, protocol_name);
}

void kadu_enomf_new_fingerprint(void *opdata, OtrlUserState us, const char *accountname, const char *protocol,
								const char *username, unsigned char fingerprint[20])
{
	Q_UNUSED(fingerprint);
	printf("kadu_enomf_new_fingerprint %p %p %s %s %s\n", opdata, us->context_root, accountname, protocol, username);
}

void kadu_enomf_write_fingerprints(void *opdata)
{
	printf("kadu_enomf_write_fingerprints %p\n", opdata);
}

void kadu_enomf_gone_secure(void *opdata, ConnContext *context)
{
	printf("kadu_enomf_gone_secure %p %p\n", opdata, context);
}

void kadu_enomf_gone_insecure(void *opdata, ConnContext *context)
{
	printf("kadu_enomf_gone_insecure %p %p\n", opdata, context);
}

void kadu_enomf_still_secure(void *opdata, ConnContext *context, int is_reply)
{
	printf("kadu_enomf_still_secure %p %p %d\n", opdata, context, is_reply);
}

void kadu_enomf_log_message(void *opdata, const char *message)
{
	printf("kadu_enomf_log_message %p %s\n", opdata, message);
}

int kadu_enomf_max_message_size(void *opdata, ConnContext *context)
{
	printf("kadu_enomf_max_message_size %p %p\n", opdata, context);
	return 20000;
}

const char * kadu_enomf_account_name(void *opdata, const char *account, const char *protocol)
{
	printf("kadu_enomf_account_name %p %s %s\n", opdata, account, protocol);
	return protocol;
}

void kadu_enomf_account_name_free(void *opdata, const char *account_name)
{
	printf("kadu_enomf_account_name_free %p %s\n", opdata, account_name);
}

EncryptionNgOtrMessageFilter::EncryptionNgOtrMessageFilter(QObject *parent) :
		MessageFilter(parent)
{
	OTRL_INIT;

	ops.policy = kadu_enomf_policy;
	ops.create_privkey = kadu_enomf_create_privkey;
	ops.is_logged_in = kadu_enomf_is_logged_in;
	ops.inject_message = kadu_enomf_inject_message;
	ops.notify = kadu_enomf_notify;
	ops.display_otr_message = kadu_enomf_display_otr_message;
	ops.update_context_list = kadu_enomf_update_context_list;
	ops.protocol_name = kadu_enomf_protocol_name;
	ops.protocol_name_free = kadu_enomf_protocol_name_free;
	ops.new_fingerprint = kadu_enomf_new_fingerprint;
	ops.write_fingerprints = kadu_enomf_write_fingerprints;
	ops.gone_secure = kadu_enomf_gone_secure;
	ops.gone_insecure = kadu_enomf_gone_insecure;
	ops.still_secure = kadu_enomf_still_secure;
	ops.log_message = kadu_enomf_log_message;
	ops.max_message_size = kadu_enomf_max_message_size;
	ops.account_name = kadu_enomf_account_name;
	ops.account_name_free = kadu_enomf_account_name_free;
}

EncryptionNgOtrMessageFilter::~EncryptionNgOtrMessageFilter()
{
}

bool EncryptionNgOtrMessageFilter::acceptMessage(const Message &message)
{
	Q_UNUSED(message);

	if (MessageTypeSent == message.type())
		return true;

	char *newmessage = 0;
	int otrMessageResult = otrl_message_receiving(otrl_userstate_create(), &ops, 0, "2964574", "gadu",
			strdup(message.messageSender().id().toUtf8().data()),
			strdup(message.htmlContent().toUtf8().data()),
			&newmessage, 0, 0, 0);

	return (otrMessageResult == 0) && (newmessage == 0);
}
