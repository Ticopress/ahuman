#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

void MindNetworkType::createFromXml( Xml xml ) {
	// attributes area properties
	name = xml.getAttribute( "name" );
}

String MindNetworkType::getName() {
	return( name );
}

