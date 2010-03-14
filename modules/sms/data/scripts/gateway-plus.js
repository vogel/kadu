GatewayPlus.prototype = {
	name: function() {
		return "Plus";
	},

	id: function() {
		return "01";
	}
};

function GatewayPlus() {
	return this;
}

gatewayManager.addItem(new GatewayPlus());
