#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindAreaNet::MindAreaNet( MindNet *p_net ) {
	net = p_net;
}

void MindAreaNet::addRegion( MindRegion *region ) {
	regions.add( region );
}

ClassList<MindRegion>& MindAreaNet::getRegions() {
	return( regions );
}

MindNet *MindAreaNet::getNet() {
	return( net );
}
