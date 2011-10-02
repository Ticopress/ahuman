#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindMap::~MindMap() {
	mindNets.destroy();
	mindAreas.destroy();
	mindLinks.destroy();
}

MindNetInfo *MindMap::getNetByName( String netName ) {
	MindNetInfo *info = mindNetMap.get( netName );
	ASSERTMSG( info != NULL , "Wrong net name=" + netName );
	return( info );
}

MindAreaInfo *MindMap::getAreaById( String areaId ) {
	MindAreaInfo *info = mindAreaMap.get( areaId );
	ASSERTMSG( info != NULL , "Wrong area id=" + areaId );
	return( info );
}

void MindMap::createFromXml( Xml xml ) {
	createAreaSet( xml.getFirstChild( "MindAreaSet" ) );
	createMindLinkSet( xml.getFirstChild( "MindLinkSet" ) );
	createNetworkTypeSet( xml.getFirstChild( "MindNetworkTypeSet" ) );
	createNetworkSet( xml.getFirstChild( "MindNetworkSet" ) );
}

void MindMap::createAreaSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "MindArea" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "MindArea" ) ) {
		// construct MindArea from attributes
		MindAreaInfo *info = new MindAreaInfo;
		mindAreas.add( info );

		info -> createFromXml( xmlChild );

		// get areaId
		String id = info -> getAreaId();
		ASSERTMSG( !id.isEmpty() , "area is not defined: " + xmlChild.serialize() );
		ASSERTMSG( mindAreaMap.get( id ) == NULL , id + ": area duplicate found for id=" + id );

		// add
		mindAreaMap.add( id , info );
	}
}

void MindMap::createMindLinkSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "MindLink" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "MindLink" ) ) {
		// construct MindArea from attributes
		MindAreaLinkInfo *info = new MindAreaLinkInfo;
		info -> createFromXml( xmlChild );
		mindLinks.add( info );
	}
}

void MindMap::createNetworkTypeSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlNetChild = xml.getFirstChild( "MindNetworkType" ); xmlNetChild.exists(); xmlNetChild = xmlNetChild.getNextChild( "MindNetworkType" ) ) {
		// construct MindArea from attributes
		MindNetworkType *netType = new MindNetworkType;
		netType -> createFromXml( xmlNetChild );
		netTypes.add( netType );

		// get areaId
		String name = netType -> getName();
		ASSERTMSG( !name.isEmpty() , "network type is not defined: " + xmlNetChild.serialize() );
		ASSERTMSG( netTypeMap.get( name ) == NULL , name + ": net type duplicate found for name=" + name );

		// add
		netTypeMap.add( name , netType );
	}
}

void MindMap::createNetworkSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlNetChild = xml.getFirstChild( "MindNet" ); xmlNetChild.exists(); xmlNetChild = xmlNetChild.getNextChild( "MindNet" ) ) {
		// construct MindArea from attributes
		MindNetInfo *info = new MindNetInfo;
		info -> createFromXml( xmlNetChild );
		mindNets.add( info );

		// get areaId
		String name = info -> getName();
		ASSERTMSG( !name.isEmpty() , "network is not defined: " + xmlNetChild.serialize() );
		ASSERTMSG( mindNetMap.get( name ) == NULL , name + ": net duplicate found for name=" + name );

		// set net type
		String netTypeName = info -> getTypeName();
		MindNetworkType *netType = netTypeMap.get( netTypeName );
		ASSERTMSG( netType != NULL , "unknown network type: name=" + netTypeName );
		info -> setNetType( netType );

		// add
		mindNetMap.add( name , info );
	}
}
