#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

InhibitoryLink::InhibitoryLink( MindRegionLink *p_regionLink ) : NeuroLink( p_regionLink ) {
	attachLogger();
}

void InhibitoryLink::apply( NeuroVector *srcData ) {
}

