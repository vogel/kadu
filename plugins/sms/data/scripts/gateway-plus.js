/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostañski (rozteck@interia.pl)
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

	sendSms: function(recipient, signature, content, callbackObject) {
		var sender = new PlusGatewaySmsSender();
		sender.sendSms(recipient, signature, content, callbackObject);
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

	sendSms: function(recipient, signature, content, callbackObject) {
		this.recipient = recipient;
		this.signature = signature;
		this.content = content;
		this.callbackObject = callbackObject;
		this.tokenId = "3tWYNk1UOXRraM5C";

		if (!network) {
			this.failure("Network not available");
			return;
		}

		var imageUrl = "http://www1.plus.pl/bsm/jcaptcha?captchaId=" + this.tokenId;

		this.callbackObject.readToken(imageUrl, this, this.tokenRead);
	},

	tokenRead: function(tokenValue) {

		var separator = String.fromCharCode(0xef, 0xbf, 0xbf);
		//var end = String.fromCharCode(0x0d, 0x0a, 0x0d, 0x0a);

		var postUrl = "http://www1.plus.pl/bsm/service/SendSmsService";
		var postData = 
			"3" + separator + "0" + separator + "13" + separator +
			"http://www1.plus.pl/bsm/" + separator +
			"E23E25B5B18D1CA333516D75EAF0966A" + separator +
			"pl.plus.map.bsm.gwt.client.service.SendSmsService" + separator + "send" + separator +
			"pl.plus.map.bsm.core.gwt.dto.MessageTO" + separator +
			"pl.plus.map.bsm.core.gwt.dto.MessageTO/3818266010" + separator +
			tokenValue + separator +
			this.tokenId + separator + 
			"a25d77f6f165317cc6400863a41d9ca6" + separator +
			this.content  + separator + separator +
			this.recipient + separator +
			this.signature + separator + 
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
			"8" + separator +
			"9" + separator +
			"0" + separator +
			"0" + separator +
			"10"+ separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"1" + separator +
			"1" + separator +
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
			"0" + separator +
			"11"+ separator +
			"12" + separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"13"+ separator +
			"0" + separator +
			"0" + separator +
			"11"+ separator +
			"0" + separator +
			"0" + separator +
			"0" + separator +
			"0" + separator;

		network.setUnicode(true);
		this.reply = network.post(postUrl, postData);
		this.reply.finished.connect(this, this.smsSent);
	},

	smsSent: function() {
		var content = this.reply.content();

		if (content.indexOf("SMS zostaï¿½ wysï¿½any") >= 0)
			this.finished();
		else if (content.indexOf("wiadomo¶æ zosta³a wys³ana") >= 0)
			this.finished();
		else if (content.indexOf("wiadomoÅ›Ä‡ zostaÅ‚a wysÅ‚ana") >= 0)
			this.finished();
		else if (content.indexOf("OK") >= 0)
			this.finished();
		else if (content.indexOf("IncorrectCaptchaTextException") >= 0)
			this.failure("Text from the picture is incorrect");
		else
			//this.failure("Provider gateway results page looks strange. SMS was probably NOT sent.");
			this.failure(content);
	}

};

function PlusGatewaySmsSender() {
	return this;
}
