#include "engine_impl.h"

/*#########################################################################*/
/*#########################################################################*/

EngineThreadHelper::EngineThreadHelper()
{ 
	oldAIUnhandledExceptionTranslator = NULL;
	remains = false; 
}

EngineThreadHelper::~EngineThreadHelper()
{
}

void EngineThreadHelper::addThreadObject()
{
	AIEngine& engine = AIEngine::getInstance();
	engine.addThreadObject( "engineThreadHelper" , this );
}

EngineThreadHelper *EngineThreadHelper::getThreadObject()
{
	AIEngine& engine = AIEngine::getInstance();
	return( ( EngineThreadHelper * )engine.getThreadObject( "engineThreadHelper" ) );
}
