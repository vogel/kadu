/* <![CDATA[ */

// MessageStatus enum vaules identical with those defined in kadu-core/chat/message/message-common.h
StatusUnknown = 0;
StatusReceived = 1;
StatusSent = 2;
StatusDelivered = 3;
StatusWontDeliver = 4;

// ContactActivity enum vaules identical with those defined in protocols/services/chat-state-service.h
StateActive = 0;
StateComposing = 1;
StateGone = 2;
StateInactive = 3;
StateNone = 4;
StatePaused = 5;

//clear messages
function adium_clearMessages()
{
	var chatElement = document.getElementById('Chat');
	chatElement.innerHTML = "";
}

//removes first message
function adium_removeFirstMessage()
{
	var chatElement = document.getElementById('Chat');
	chatElement.removeChild(chatElement.firstChild);
}

function adium_contactActivityChanged(state, message, name)
{
	if (typeof(contactActivityChanged) != 'undefined')
		contactActivityChanged(state, message, name);
}

function kadu_clearMessages()
{
	var node = document.getElementsByTagName('body')[0];
	node.innerHTML = "";
}

function kadu_appendMessage(html)
{
	var node = document.getElementsByTagName('body')[0];
	var range = document.createRange();
	range.selectNode(node);
	var documentFragment = range.createContextualFragment(html);
	var scriptnodes = kadu_takeScriptNodes(documentFragment);
	node.appendChild(documentFragment);
	var body = document.getElementsByTagName('body')[0];
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
	var node = document.getElementsByTagName('body')[0];
	node.removeChild(node.firstChild);
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

/* ]]> */
