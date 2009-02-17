/* <![CDATA[ */
// Some parts of the code below are borrowed from psi project (http://psi-im.org/)

//append html fragment what to where node
function kadu_appendChilds(where, what)
{
	while(what.hasChildNodes())
	{
		'app'+where.appendChild(what.firstChild);
	}
}

//append non consecutive message
function kadu_appendNextMessage(messageBody)
{
	chatElement = document.getElementById('Chat');
	insertDiv = document.getElementById('insert');

	if(insertDiv != null)
	{
		insertDiv.parentNode.removeChild(insertDiv);
        }

	newNode = document.createElement('div');
	newNode.innerHTML=messageBody;
	kadu_appendChilds(chatElement, newNode);
}

//append consecutive message
function kadu_appendConsecutiveMessage(messageBody)
{
	insertDiv = document.getElementById('insert');
	parentNode = insertDiv.parentNode;
	newNode = document.createElement('span'); 
	newNode.innerHTML=messageBody;
	parentNode.replaceChild(newNode, insertDiv);
}


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
