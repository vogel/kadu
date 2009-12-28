/* <![CDATA[ */
// Some parts of the code below are borrowed from psi project (http://psi-im.org/)

//clear messages
function kadu_clearMessages()
{
	chatElement = document.getElementById('Chat');
	chatElement.innerHTML = "";
}

//removes first message
function kadu_removeFirstMessage()
{
	chatElement = document.getElementById('Chat');
	chatElement.removeChild(chatElement.firstChild);
}


/* ]]> */
