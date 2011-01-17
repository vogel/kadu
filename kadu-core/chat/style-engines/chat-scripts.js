/* <![CDATA[ */

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

function kadu_clearMessages()
{
	var node = document.getElementsByTagName('body')[0];
	node.innerHTML = "";
}

var kadu_timer = null;
var kadu_shouldScroll = false;
function kadu_appendMessage(html)
{
	kadu_shouldScroll = kadu_shouldScroll || document.body.scrollTop >= ( document.body.offsetHeight - window.innerHeight );

	var node = document.getElementsByTagName('body')[0];
	var range = document.createRange();
	range.selectNode(node);
	var documentFragment = range.createContextualFragment(html);
	node.appendChild(documentFragment);

	if (kadu_shouldScroll)
	{
		for(var img in node.getElementsByTagName('img'))
			img.onload = function(){ setTimeout("queueScrollToBottom();", 20); }
		if (kadu_timer != null)
			clearTimeout(kadu_timer);
		kadu_timer = setTimeout("queueScrollToBottom();", 20);
	}
}

function queueScrollToBottom()
{
	setTimeout("scrollToBottom();", 20);
}

function scrollToBottom()
{
	document.body.scrollTop = document.body.offsetHeight;
	kadu_shouldScroll = false;
}

function kadu_removeFirstMessage()
{
	var node = document.getElementsByTagName('body')[0];
	node.removeChild(node.firstChild);
}

/* ]]> */
