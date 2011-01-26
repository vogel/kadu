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

function kadu_appendMessage(html)
{
	var node = document.getElementsByTagName('body')[0];
	var range = document.createRange();
	range.selectNode(node);
	var documentFragment = range.createContextualFragment(html);
	node.appendChild(documentFragment);
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

/* ]]> */
