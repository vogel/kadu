/* <![CDATA[ */

// MessageStatus enum vaules identical with those defined in kadu-core/chat/message/message-common.h
StatusUnknown = 0;
StatusDelivered = 4;
StatusWontDeliver = 5;

// ContactActivity enum vaules identical with those defined in protocols/services/chat-state-service.h
StateActive = 0;
StateComposing = 1;
StateGone = 2;
StateInactive = 3;
StateNone = 4;
StatePaused = 5;

function adium_clearMessages()
{
	var chatElement = document.getElementById('Chat');
	chatElement.innerHTML = "";
}

function adium_removeFirstMessage()
{
	var chatElement = document.getElementById('Chat');
	if (chatElement && chatElement.firstChild)
		chatElement.removeChild(chatElement.firstChild);
}

function adium_contactActivityChanged(state, message, name)
{
	if (typeof(contactActivityChanged) != 'undefined')
		contactActivityChanged(state, message, name);
}

function adium_chatImageAvailable(name, src)
{
	if (typeof(name) == 'undefined' || typeof(src) == 'undefined')
		return;

	var image = document.getElementsByName(name)[0];
	if (!image)
		return;

	if (image.tagName.toLowerCase() != 'img')
		return;

	image.src = src;
}

function kadu_clearMessages()
{
	// A style can place its own content before any messages with <kadu:top>,
	// so we don't want to delete that content here. Also bear in mind that
	// kadu_takeScriptNodes() usage might result in <script> tags being put as
	// direct <body> children (but surely not preceding the first
	// <span class="kadu_message">).
	var messages = document.getElementsByClassName('kadu_message');
	if (0 == messages.length)
		return;

	var node = messages[0];
	while (node)
	{
		var nextNode = node.nextSibling;
		if (node.parentNode)
			node.parentNode.removeChild(node);
		node = nextNode;
	}
}

function kadu_appendMessage(html)
{
	var body = document.getElementsByTagName('body')[0];
	var range = document.createRange();
	range.selectNode(body);
	var documentFragment = range.createContextualFragment(html);
	var scriptnodes = kadu_takeScriptNodes(documentFragment);
	body.appendChild(documentFragment);
	for (var k in scriptnodes)
		body.appendChild(scriptnodes[k]);
}

function kadu_takeScriptNodes(node)
{
	var scriptnodes = new Array();
	for (var k in node.childNodes)
	{
		var childnode = node.childNodes[k];
		if (childnode.nodeName && childnode.nodeName.toLowerCase() == "script")
		{
			var scriptnode = node.removeChild(childnode);
			scriptnodes.push(scriptnode.cloneNode(true));
		}
		else
			scriptnodes = scriptnodes.concat(kadu_takeScriptNodes(childnode));
	}
	return scriptnodes;
}

function scrollToBottom()
{
//	we have better implementation in C++, so we should ignore it
//	document.body.scrollTop = document.body.offsetHeight;
}

function kadu_removeFirstMessage()
{
	var messages = document.getElementsByClassName('kadu_message');
	for (var k in messages)
	{
		var message = messages[k];
		if (message.parentNode)
		{
			var node = message.nextSibling;
			message.parentNode.removeChild(message);
			// There can be some <script> tags, see kadu_clearMessages() for explanation.
			while (node.className != 'kadu_message')
			{
				var nextNode = node.nextSibling;
				if (node.parentNode)
					node.parentNode.removeChild(node);
				node = nextNode;
			}
			break;
		}
	}
}

function kadu_messageStatusChanged(messageid, status)
{
	if (typeof(messageStatusChanged) != 'undefined')
		messageStatusChanged(messageid, status);
}

function kadu_contactActivityChanged(state, message, name)
{
	if (typeof(contactActivityChanged) != 'undefined')
		contactActivityChanged(state, message, name);
}

function kadu_chatImageAvailable(name, src)
{
	if (typeof(name) == 'undefined' || typeof(src) == 'undefined')
		return;

	var image = document.getElementsByName(name)[0];
	if (!image)
		return;

	if (image.tagName.toLowerCase() != 'img')
		return;

	image.src = src;
}

function toggleImageScaling() {
	var node = event.target;
	if (!node || !node.className)
		return;

	if (node.className.indexOf("scalable") >= 0) {
		if (node.className.indexOf("unscaled") >= 0) {
			node.className = node.className.replace(" unscaled", "");
		} else
			node.className = node.className + " unscaled";
	}
}
document.addEventListener("click", toggleImageScaling, true);
/* ]]> */
