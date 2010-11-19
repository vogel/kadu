/* <![CDATA[ */

//clear messages
function adium_clearMessages()
{
	chatElement = document.getElementById('Chat');
	chatElement.innerHTML = "";
}

//removes first message
function adium_removeFirstMessage()
{
	chatElement = document.getElementById('Chat');
	chatElement.removeChild(chatElement.firstChild);
}

function kadu_clearMessages()
{
	node = document.getElementsByTagName('body')[0];
	node.innerHTML = "";
}

function kadu_appendMessage(html)
{
	shouldScroll = document.body.scrollTop >= (document.body.offsetHeight - (window.innerHeight * 1.2));

	node = document.getElementsByTagName('body')[0];
	range = document.createRange();
	range.selectNode(node);
	documentFragment = range.createContextualFragment(html);
	node.appendChild(documentFragment);

	if (shouldScroll)
		setTimeout("scrollToBottom()", 0);
}

function scrollToBottom()
{
	document.body.scrollTop = document.body.offsetHeight;
}

function kadu_removeFirstMessage()
{
	node = document.getElementsByTagName('body')[0];
	node.removeChild(node.firstChild);
}

/* ]]> */
