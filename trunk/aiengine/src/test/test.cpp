#include "test_impl.h"

/*#########################################################################*/
/*#########################################################################*/

AITestPool::AITestPool()
{ 
	thisPtr = AITestPoolImpl::getServiceImpl(); 
}

AITestPoolImpl *AITestPoolImpl::getServiceImpl()
{
	return( static_cast<AITestPoolImpl *>( AIEngine::getInstance().getService( "TestPool" ) ) );
}

/* static */ Service *AITestPool::newService()
{
	Service *svc = new AITestPoolImpl();
	return( svc );
}

AITestPoolImpl::AITestPoolImpl()
:	engine( AIEngine::getInstance() )
{
	callPub = NULL;
	callSub = NULL;
}

void AITestPoolImpl::createService( Xml config )
{
	// create units
	addTestUnits();
	
	// log available commands
	logger.logInfo( "createService: AVAILABLE TEST METHODS:" );
	for( int k = 0; k < units.count(); k++ ) {
		TestUnit *tu = units.getClassByIndex( k );
		
		MapStringToClass<TestMethod>& methods = tu -> methods;
		for( int m = 0; m < methods.count(); m++ ) {
			logger.logInfo( String( "createService: method=" ) + methods.getKeyByIndex( m ) );
		}
	}

	Xml xml = config.getFirstChild( "channels" );
	channelIn = xml.getProperty( "request" );
	channelOut = xml.getProperty( "response" );
}

void AITestPoolImpl::initService()
{
	// subscribe
	AIIO io;
	callSub = io.subscribe( NULL , channelIn , "testpool" , this );
	callPub = io.createPublisher( NULL , channelOut , "testpool" , "test" );
}

void AITestPoolImpl::runService()
{
}

void AITestPoolImpl::exitService()
{
	AIIO io;

	// ensure no more requests
	if( callSub != NULL )
		io.unsubscribe( callSub );

	// exit all test units
	for( int k = 0; k < units.count(); k++ ) {
		TestUnit *tu = units.getClassByIndex( k );
		tu -> exit();
	}
}

void AITestPoolImpl::destroyService()
{
	units.destroy();
	delete this;
}

void AITestPoolImpl::addTestUnit( TestUnit *p_unit )
{
	units.add( p_unit -> getName() , p_unit );
	p_unit -> init();
}

/*#########################################################################*/
/*#########################################################################*/

void AITestPoolImpl::onXmlCall( XmlCall *msg )
{
	XmlCall& call = *msg;
	String cn = call.getClassName();
	String fn = call.getFunctionName();
	String fnfull = cn + "::" + fn;
	logger.logInfo( "onXmlCall: test method name=" + fnfull + " called in session=" + call.getSession() -> getSessionId() );

	// find unit
	TestUnit *unit = units.get( cn );
	ASSERTMSG( unit != NULL , "Unknown test unit=" + cn );
		
	// find method
	TestMethod *method = unit -> getMethod( fnfull );
	ASSERTMSG( method != NULL , "Unknown test unit method=" + fn );

	try {
		unit -> logger.attach( fnfull );
		unit -> callMethod( method , call );
		call.sendStatusResponse( "OK" );
	}
	catch ( RuntimeException& e ) {
		e.printStack( logger );
		call.sendResponseException( callPub , e );
	}
	catch( ... ) {
		logger.logError( "onXmlCall: unknown exception:" );
		logger.printStack();
		call.sendResponseUnknownException( callPub );
	}
}

