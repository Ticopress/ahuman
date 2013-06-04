#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetRegionDef::TargetRegionDef( TargetAreaDef *areaInfo ) : MindRegionDef( areaInfo ) {
	circuitInfo = NULL;
}

TargetRegionDef::~TargetRegionDef() {
	if( MindRegionDef::type != NULL )
		delete MindRegionDef::type;
}

void TargetRegionDef::defineSensorRegion( Xml xml ) {
	MindRegionDef::name = xml.getAttribute( "id" );
	Object::setInstance( MindRegionDef::name );
	MindRegionDef::size = xml.getIntAttribute( "size" , 0 );

	// region type for sensor is the same as region itself
	TargetRegionTypeDef *regionTypeInfo = new TargetRegionTypeDef();
	regionTypeInfo -> defineSensorRegionType( xml );
	MindRegionDef::type = regionTypeInfo;
	MindRegionDef::typeName = regionTypeInfo -> getName();
}

