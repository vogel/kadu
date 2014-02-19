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

	maxLength: function() {
		return 0;
	},

	signatureRequired: function() {
		return true;
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
			this.failure(translator.tr("Network not available"));
			return;
		}

		var gateway = this.callbackObject.readFromConfiguration("SMS", "EraGateway", "Sponsored");
		var getUrl = "http://www.eraomnix.pl/msg/api/do/tinker/";
		var login = this.callbackObject.readFromConfiguration("SMS", "EraGateway_" + gateway + "_User", "");
		var password = this.callbackObject.readFromConfiguration("SMS", "EraGateway_" + gateway + "_Password", "");

		if (login == "" || password == "") {
			this.failure(translator.tr("Login or password not provided"));
			return;
		}

		var getData =
			"?number=" + encodeURI(recipient) +
			"&message=" + encodeURI(signature + ":" + content) +
			"&login=" + encodeURI(login) +
			"&password=" + encodeURI(password) +
			"&failure=http://www.t-mobile.pl&success=http://rozrywka.t-mobile.pl" +
			"&mms=false";

		if (gateway == "Sponsored") {
			getUrl += "sponsored";
		}
		else if (gateway == "OmnixMultimedia") {
			getUrl += "omnix"
		}
		else {
			this.failure("Invalid gateway type");
			return;
		}

		network.setUtf8(false);
		this.reply = network.get(getUrl + getData);
		this.reply.finished.connect(this, this.smsSent);
	},

	errorToString: function(errorNumber) {
		switch (errorNumber) {
			case "0": return translator.tr("No error");
			case "1": return translator.tr("System failure");
			case "2": return translator.tr("Unauthorised user");
			case "3": return translator.tr("Access forbidden");
			case "5": return translator.tr("Syntax error");
			case "7": return translator.tr("Limit of the sms run-down");
			case "8": return translator.tr("Wrong receiver address");
			case "9": return translator.tr("Message too long");
			case "10": return translator.tr("You don't have enough tokens");
		}

		return translator.tr("Unknown error");
	},

	smsSent: function() {
		var content = this.reply.redirect();
		if (content.match(/X-ERA-error=([0-9]+)/)) {
			var errorCode = RegExp.$1;
			if (errorCode != 0) {
				var message = translator.tr("An error occured");
				message += ": " + this.errorToString(RegExp.$1);
				this.failure(message);
			}
			else
				this.finished();
		}
		else
			this.failure(translator.tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
	}
};

function EraGatewaySmsSender() {
	return this;
}
