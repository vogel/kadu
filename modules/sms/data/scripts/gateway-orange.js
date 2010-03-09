GatewayOrange.prototype = {
	name: function() {
		return "orange";
	},

	id: function() {
		return "03";
	}
};

function GatewayOrange() {
	return this;
}

gatewayManager.addItem(new GatewayOrange());
