/*
 * %kadu copyright begin%
 * Copyright 2010 Rafa≈Ç Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

GatewayPlus.prototype = {
	name: function() {
		return "Plus";
	},

	id: function() {
		return "01";
	},

	sendSms: function(recipient, sender, signature, content, callbackObject) {
		var sender = new PlusGatewaySmsSender();
		sender.sendSms(recipient, sender, signature, content, callbackObject);
	}
};

function GatewayPlus() {
	return this;
}

gatewayManager.addItem(new GatewayPlus());

PlusGatewaySmsSender.prototype = {

	failure: function(errorMessage) {
		this.callbackObject.failure(errorMessage);
	},

	finished: function() {
		this.callbackObject.result();
	},

	sendSms: function(recipient, sender, signature, content, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.failure("Network not available");
			return;
		}

		var separator = String.fromCharCode(0xef, 0xbf, 0xbf);
		var end = String.fromCharCode(0x0d, 0x0a, 0x0d, 0x0a);

		var postUrl = "http://www1.plus.pl/bsm/service/SendSmsService";
		var postData = 
		"3" + separator + "0" + separator + "11" + separator +
		"http://www1.plus.pl/bsm/" + separator +
		"0C1263E2047195AA1A9BA7FDDF8A4B74" + separator +
		"pl.plus.map.bsm.gwt.client.service.SendSmsService" + separator + "send" + separator +
		"pl.plus.map.bsm.core.gwt.dto.MessageTO" + separator +
		"pl.plus.map.bsm.core.gwt.dto.MessageTO/2025876352" + separator +
		"638fe7871b8f5e6bcb4e3fc6a44a69ce" + separator +
		content + separator +
		recipient + separator +
		signature + separator + separator + 
		"1" + separator +
		"2" + separator +
		"3" + separator +
		"4" + separator +
		"1" + separator +
		"5" + separator +
		"6" + separator +
		"0" + separator +
		"0" + separator +
		"7" + separator +
		"0" + separator +
		"0" + separator +
		"8" + separator +
		"0" + separator +
		"0" + separator +
		"1" + separator +
		"2" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"9" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"10"+ separator +
		"0" + separator +
		"0" + separator +
		"11"+ separator +
		"0" + separator +
		"0" + separator +
		"0" + separator + end;

		network.setUnicode(true);
		this.reply = network.post(postUrl, postData);
		this.reply.finished.connect(this, this.smsSent);
	},

	smsSent: function() {
		var content = this.reply.content();

		if (content.indexOf("SMS zostaÔøΩ wysÔøΩany") >= 0)
			this.finished();
		else if (content.indexOf("wiadomo∂Ê zosta≥a wys≥ana") >= 0)
			this.finished();
		else if (content.indexOf("wiadomo≈õƒá zosta≈Ça wys≈Çana") >= 0)
			this.finished();
		else if (content.indexOf("OK") >= 0)
			this.finished();
		else
			this.failure("Provider gateway results page looks strange. SMS was probably NOT sent.");
	}

};

function PlusGatewaySmsSender() {
	return this;
}