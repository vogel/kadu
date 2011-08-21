/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

GatewayEra.prototype = {
	name: function() {
		return "T-Mobile.PL";
	},

	id: function() {
		return "02";
	},

	sendSms: function(recipient, signature, content, callbackObject) {
		var sender = new EraGatewaySmsSender();
		sender.sendSms(recipient, signature, content, callbackObject);
	}
};

function GatewayEra() {
	return this;
}

gatewayManager.addItem(new GatewayEra());

EraGatewaySmsSender.prototype = {

	failure: function(errorMessage) {
		this.callbackObject.failure(errorMessage);
	},

	finished: function() {
		this.callbackObject.result();
	},

	sendSms: function(recipient, signature, content, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.failure("Network not available");
			return;
		}

		var gateway = this.callbackObject.readFromConfiguration("SMS", "EraGateway", "Sponsored")

		var postUrl = "http://www.eraomnix.pl/msg/api/do/tinker/";

		var postData =
			"&message=" + signature + ":" + content +
			"&password=" + this.callbackObject.readFromConfiguration("SMS", "EraGateway_" + gateway + "_Password", "") +
			"&login=" + this.callbackObject.readFromConfiguration("SMS", "EraGateway_" + gateway + "_User", "") +
			"&success=OK&failure=ERROR" +
			"&mms=false";

		if (gateway == "Sponsored") {
			postUrl += "sponsored";
			postData +=	"&number=" + recipient;
		}
		else if (gateway == "OmnixMultimedia")
		{
			postUrl += "omnix"
			postData +=	"&numbers=" + recipient;
		}
		else {
			this.failure("Invalid gateway type");
			return;
		}

		network.setUtf8(false);
		this.reply = network.post(postUrl, postData);
		this.reply.finished.connect(this, this.smsSent);
	},

	errorToString: function(errorNumber) {
		switch (errorNumber) {
			case "0": return "No error";
			case "1": return "System failure";
			case "2": return "Unauthorised user";
			case "3": return "Access forbidden";
			case "5": return "Syntax error";
			case "7": return "Limit of the sms run-down";
			case "8": return "Wrong receiver address";
			case "9": return "Message too long";
			case "10": return "You don't have enough tokens";
		}

		return "Unknown error";
	},

	smsSent: function() {
		var content = this.reply.redirect();
		if (content.indexOf("error?") >= 0) {
			var message = "An error occured";
			if (content.match(/error\?X-ERA-error=([0-9]+)/)) {
				message += ": " + this.errorToString(RegExp.$1);
			}
			this.failure(message);
		} else if (content.indexOf("ok?") >= 0)
			this.finished();
		else
			this.failure("Provider gateway results page looks strange. SMS was probably NOT sent.");
	}
};

function EraGatewaySmsSender() {
	return this;
}
