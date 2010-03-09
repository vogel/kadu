GatewayPlus.prototype = {
	name: function() {
		return "plus";
	},

	id: function() {
		return "01";
	}
};

function GatewayPlus() {
	return this;
}

gatewayManager.addItem(new GatewayPlus());
