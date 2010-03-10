GatewayOrange.prototype = {
	name: function() {
		return "orange";
	},

	id: function() {
		return "03";
	},

	sendSms: function(receipient, sender, signature, content, callbackObject) {
		var sender = new GatewaySmsSender();
		sender.sendSms(receipient, sender, signature, content, callbackObject);
	},
};

function GatewayOrange() {
	return this;
}

gatewayManager.addItem(new GatewayOrange());

GatewaySmsSender.prototype = {

	failure: function() {
		this.callbackObject.failure();
	},

	result: function(result) {
		this.callbackObject.result(result);
	},

	sendSms: function(recipient, sender, signature, content, callbackObject) {
		this.recipient = recipient;
		this.sender = sender;
		this.signature = signature;
		this.content = content;
		this.callbackObject = callbackObject;

		if (!network) {
			this.failure();
			return;
		}

		var formUrl = "http://sms.orange.pl/";

		this.reply = network.get(formUrl);
		this.reply.finished.connect(this, this.formReceived);
	},

	formReceived: function() {
		if (!this.reply.ok()) {
			this.failure();
			return;
		}

		var content = this.reply.content();
		var tokenPattern = new RegExp("rotate_token\\.aspx\\?token=([^\"]+)");
		var match = tokenPattern.exec(content);

		if (null == match) {
			this.failure();
			return;
		}

		this.token = match[1];
		var imageUrl = "http://sms.orange.pl/" + match[0];
		tokenReader.readToken(imageUrl, this, this.tokenRead);
	},

	tokenRead: function(tokenValue) {
		var url = "http://sms.orange.pl/sendsms.aspx";
		var postData = "";

		postData += "token=";
		postData += escape(this.token);
		postData += "&SENDER=";
		postData += escape(this.signature);
		postData += "&RECIPIENT=";
		postData += escape(this.recipient);
		postData += "&SHORT_MESSAGE=";
		postData += escape(this.content);
		postData += "&pass=";
		postData += escape(tokenValue);
		postData += "&CHK_RESP=";
		postData += "FALSE";
		postData += "&respInfo=";
		postData += "1";

		network.post(url, postData);
	}

};

function GatewaySmsSender() {
	return this;
}
