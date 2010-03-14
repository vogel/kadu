GatewayEra.prototype = {
	name: function() {
		return "Era";
	},

	id: function() {
		return "02";
	}
};

function GatewayEra() {
	return this;
}

gatewayManager.addItem(new GatewayEra());
