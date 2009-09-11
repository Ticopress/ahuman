#include "aiio_impl.h"

/*#########################################################################*/
/*#########################################################################*/

XmlCall::XmlCall( Channel *p_channelIn , Channel *p_channelOut , const char *txt )
:	XmlMessage( txt )
{
	responseSent = false;
	channelIn = p_channelIn;
	channelOut = p_channelOut;
}

XmlCall::~XmlCall()
{
	if( xmlResponse.exists() && responseSent == false )
		AIEngine::getInstance().destroyXmlDoc( xmlResponse );
}

void XmlCall::postExecute()
{
	if( responseSent == false )
		sendUnknownResponse();
}

void XmlCall::setXmlFromMessage()
{
	XmlMessage::setXmlFromMessage( "xmlcall" );
	
	Xml xml = XmlMessage::getXml();
	requestId = xml.getAttribute( "requestId" );
	functionName = xml.getAttribute( "name" );
	params = xml.getChildNode( "parameters" );
}

// request
String XmlCall::getFunctionName()
{
	return( functionName );
}

String XmlCall::getParam( String paramName )
{
	return( params.getProperty( paramName ) );
}

String XmlCall::getParam( String paramName , String defaultValue )
{
	return( params.getProperty( paramName , defaultValue ) );
}

int XmlCall::getIntParam( String paramName )
{
	return( params.getIntProperty( paramName ) );
}

int XmlCall::getIntParam( String paramName , int defaultValue )
{
	return( params.getIntProperty( paramName , defaultValue ) );
}

bool XmlCall::getBooleanParam( String paramName )
{
	return( params.getBooleanProperty( paramName ) );
}

float XmlCall::getFloatParam( String paramName )
{
	return( params.getFloatProperty( paramName ) );
}

float XmlCall::getFloatParam( String paramName , float defaultValue )
{
	return( params.getFloatProperty( paramName , defaultValue ) );
}

// response 
Xml XmlCall::createResponse()
{
	ASSERT( !xmlResponse.exists() );
	
	xmlResponse = AIEngine::getInstance().createXml( "xmlcallresult" );
	xmlResponse.setAttribute( "requestId" , requestId );
	xmlResponse.setAttribute( "name" , functionName );

	return( xmlResponse );
}

Xml XmlCall::getResponse()
{
	return( xmlResponse );
}

String XmlCall::sendResponse( Publisher *pub )
{
	ASSERT( xmlResponse.exists() );
	XmlMessage *msg = new XmlMessage( xmlResponse );
	responseSent = true;
	return( pub -> publish( msg ) );
}

String XmlCall::sendResponseException( Publisher *pub , RuntimeException& e )
{
	String error = e.printStack();
	Xml res = createResponse();
	res.setAttribute( "status" , "Exception" );
	res.addTextElement( "exception" , error );
	String msg = sendResponse( pub );

	return( msg );
}

String XmlCall::sendResponseUnknownException( Publisher *pub )
{
	Xml res = createResponse();
	res.setAttribute( "status" , "Exception" );
	res.addTextElement( "exception" , "Unknown Error" );
	String msg = sendResponse( pub );

	return( msg );
}

String XmlCall::sendUnknownResponse()
{
	// send default response
	Xml res = createResponse();
	res.setAttribute( "status" , "Unknown" );

	XmlMessage *msg = new XmlMessage( xmlResponse );
	Publisher *pubUnknown = channelOut -> getDefaultPublisher();
	String msgId = sendResponse( pubUnknown );

	return( msgId );
}
