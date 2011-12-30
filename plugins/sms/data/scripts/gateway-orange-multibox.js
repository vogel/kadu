/*
 * %kadu copyright begin%
 * Copyright 2011 Maciej Płaza (plaza.maciej@gmail.com)
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

GatewayOrangeM.prototype = {
	name: function() {
		return "Orange Multibox";
	},

	id: function() {
		/*
		ID 99 because we don't know for what numbers we should use Orange Multibox (it works with Orange, Plus, Play,
		Era and others) so we can't choose this gateway automatically.
		*/
		return "99";
	},

	maxLength: function() {
		return 640;
	},

	signatureRequired: function() {
		return false;
	},

	sendSms: function(recipient, signature, content, callbackObject) {
		var sender = new OMGatewaySmsSender();
		sender.sendSms(recipient, signature, content, callbackObject);
	},
};

function GatewayOrangeM() {
	return this;
}

gatewayManager.addItem(new GatewayOrangeM());

OMGatewaySmsSender.prototype = {

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

		if (!network) {
			this.failure("Network not available");
			return;
		}

		var mainPage = "http://www.orange.pl/portal/map/map/message_box?mbox_edit=new&mbox_view=newsms";
		this.reply = network.get(mainPage);
		this.reply.finished.connect(this, this.logIn);
	},

	logIn: function() {
		var redirect = this.reply.redirect();
		if (redirect.indexOf("zaloguj") >= 0)
		{
			var logInUrl = "http://www.orange.pl/start.phtml?_DARGS=/gear/infoportal/header/user-box.jsp";

			var postData = "";
			postData += "_dyncharset=UTF-8&/ptk/map/infoportal/portlet/header/formhandler/ProxyProfileFormhandler.";
			postData += "userLogin=";
			postData += this.callbackObject.readFromConfiguration("SMS", "OrangeMultibox_User", "");
			postData += "&_D:/ptk/map/infoportal/portlet/header/formhandler/ProxyPr";
			postData += "ofileFormhandler.userLogin=+&/ptk/map/infoportal/portlet/header/formhandler/ProxyProfileF";
			postData += "ormhandler.userPassword=";
			postData += this.callbackObject.readFromConfiguration("SMS", "OrangeMultibox_Password", "");
			postData += "&_D:/ptk/map/infoportal/portlet/header/formhan";
			postData += "dler/ProxyProfileFormhandler.userPassword=+&/ptk/map/infoportal/portlet/header/formhandle";
			postData += "r/ProxyProfileFormhandler.successUrl=/start.phtml&_D:/ptk/map/infoportal/portlet/header";
			postData += "/formhandler/ProxyProfileFormhandler.successUrl=+&/ptk/map/infoportal/portlet/header/fo";
			postData += "rmhandler/ProxyProfileFormhandler.errorUrl=/start.phtml&_D:/ptk/map/infoportal/portlet/";
			postData += "header/formhandler/ProxyProfileFormhandler.errorUrl=+&/ptk/map/infoportal/portlet/header";
			postData += "/formhandler/ProxyProfileFormhandler.login=loguj&_D:/ptk/map/infoportal/portlet/header/";
			postData += "formhandler/ProxyProfileFormhandler.login=+&x=0&y=0&_DARGS=/gear/infoportal/header/user-box.jsp";

			network.setUtf8(false);
			this.reply = network.post(logInUrl, postData);
			this.reply.finished.connect(this, this.getForm);
		}
		else
		{
			this.getForm();
		}
	},

	getForm: function() {
		var getFormUrl = "http://www.orange.pl/portal/map/map/message_box?mbox_edit=new&mbox_view=newsms";

		this.reply = network.get(getFormUrl);
		this.reply.finished.connect(this, this.formReceived);
	},

	formReceived: function() {
		var content = this.reply.content();
		var smsCountPattern = new RegExp("\"value\">([0-9]+)");
		var count = smsCountPattern.exec(content);

		if (content.indexOf("welcome-message") < 0)
		{
			this.failure("You are not logged in!")
		}
		else if (count[1] == "0")
		{
			this.failure("You don't have enough tokens");
		}

		var tokenPattern = new RegExp("</textarea></div><input value=\"([a-zA-Z0-9]+)\"");
		var token = tokenPattern.exec(content);

		var postString = "_dyncharset=UTF-8&/amg/ptk/map/messagebox/formhandlers/MessageFormHandler.type=sms&_D:/";
		postString += "amg/ptk/map/messagebox/formhandlers/MessageFormHandler.type=+&enabled=false&/amg/ptk/ma";
		postString += "p/messagebox/formhandlers/MessageFormHandler.errorURL=/portal/map/map/message_box?mbox_";
		postString += "view=newsms&_D:/amg/ptk/map/messagebox/formhandlers/MessageFormHandler.errorURL=+&/am";
		postString += "g/ptk/map/messagebox/formhandlers/MessageFormHandler.successURL=/portal/map/map/messa";
		postString += "ge_box?mbox_view=messageslist&_D:/amg/ptk/map/messagebox/formhandlers/MessageFormHand";
		postString += "ler.successURL=+&/amg/ptk/map/messagebox/formhandlers/MessageFormHandler.to=";
		postString += this.recipient;
		postString += "&_D:";
		postString += "/amg/ptk/map/messagebox/formhandlers/MessageFormHandler.to=+&_D:/amg/ptk/map/mess";
		postString += "agebox/formhandlers/MessageFormHandler.body=+&/amg/ptk/map/messagebox/formhandlers/Mess";
		postString += "ageFormHandler.body=";
		postString += this.signature + ": " + this.content;
		postString += "&/amg/ptk/map/messagebox/formhandlers/MessageFormHan";
		postString += "dler.token=";
		postString += token[1];
		postString += "&_D:/amg/ptk/map/messagebox/formhandlers/MessageFormHandler.token=+&/";
		postString += "amg/ptk/map/messagebox/formhandlers/MessageFormHandler.create.x=66&/amg/ptk/map/mess";
		postString += "agebox/formhandlers/MessageFormHandler.create.y=14&/amg/ptk/map/messagebox/formhandlers/";
		postString += "MessageFormHandler.create=Wyślij&_D:/amg/ptk/map/messagebox/formhandlers/MessageFor";
		postString += "mHandler.create=+&_DARGS=/gear/mapmessagebox/smsform.jsp";

		var sendSMSUrl = "http://www.orange.pl/portal/map/map/message_box?_DARGS=/gear/mapmessagebox/smsform.jsp";

		network.setUtf8(false);
		this.reply = network.post(sendSMSUrl, postString);
		this.reply.finished.connect(this, this.smsSent);
	},

	smsSent: function() {
		var redirect = this.reply.redirect();
		if (redirect.indexOf("newsms") >= 0)
		{
			this.failure("SMS was probably NOT sent.");
		}

		if (redirect.indexOf("messageslist") >= 0)
		{
			this.finished();
		}

	}
};

function OMGatewaySmsSender() {
	return this;
}
