#include <ah_mind.h>
#include <ah_mind_impl.h>

static int sequence_NL = 0;

/*#########################################################################*/
/*#########################################################################*/

NeuroLink::NeuroLink( MindRegionLink *p_regionLink ) {
	regionLink = p_regionLink;

	region = NULL;
	linkInfo = NULL;
	source = NULL;
	target = NULL;
	sizeX = 0;
	sizeY = 0;
}

NeuroLinkSource *NeuroLink::getSource() {
	return( source );
}

NeuroLinkTarget *NeuroLink::getTarget() {
	return( target );
}

void NeuroLink::setMindCircuitLinkDef( MindCircuitLinkDef *p_linkInfo ) {
	linkInfo = p_linkInfo;
}

void NeuroLink::setTransmitter( String p_transmitter ) {
	transmitter = p_transmitter;
}

void NeuroLink::createInternal( MindRegion *p_region ) {
	id = String( "NL" ) + ( ++sequence_NL );
	region = p_region;
}

void NeuroLink::create( NeuroLinkSource *p_source , NeuroLinkTarget *p_target ) {
	id = String( "NL" ) + ( ++sequence_NL );

	source = p_source;
	target = p_target;

	// set sizes if not defined by MindCircuitLinkDef
	sizeX = source -> getSizeX();
	sizeY = source -> getSizeY();
}

String NeuroLink::getId() {
	return( id );
}

int NeuroLink::getSizeX() {
	return( sizeX );
}

int NeuroLink::getSizeY() {
	return( sizeY );
}

int NeuroLink::getSize() {
	return( sizeX * sizeY );
}
