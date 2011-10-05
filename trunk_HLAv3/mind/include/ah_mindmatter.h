#ifndef	INCLUDE_AH_MINDMATTER_H
#define INCLUDE_AH_MINDMATTER_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_mindbase.h"

class NeuroVector;
class NeuroPool;
class NeuroPoolSet;
class NeuroLink;
class NeuroLinkSet;
class ExcitatoryLink;
class InhibitoryLink;
class ModulatoryLink;
class NeuroLinkSource;
class NeuroLinkTarget;

/*#########################################################################*/
/*#########################################################################*/

class NeuroVector : public Object {
public:
	virtual const char *getClass() { return( "NeuroVector" ); };

	NeuroVector( int sizeX , int sizeY );
	NeuroVector( NeuroVector *src );
	~NeuroVector();

public:
	neurovt *getRawData();
	TwoIndexArray<neurovt>& getVectorData();

private:
// utility
	TwoIndexArray<neurovt> data;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroPool : public Object {
public:
	virtual const char *getClass() { return( "NeuroPool" ); };

private:
// own data
	NeuroVector *output;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroPoolSet : public Object {
public:
	virtual const char *getClass() { return( "NeuroPoolSet" ); };

private:
// own data
	ClassList<NeuroPool> list;
};

/*#########################################################################*/
/*#########################################################################*/

class MindRegionLink;
class NeuroLinkInfo;

class NeuroLink : public Object {
public:
	NeuroLink( MindRegionLink *regionLink );
	const char *getClass() { return( "NeuroLink" ); };

	// parameters
	String getId();
	NeuroLinkSource *getSource();
	NeuroLinkTarget *getTarget();

	void setNeuroLinkInfo( NeuroLinkInfo *linkInfo );
	void setTransmitter( String transmitter );
	void create( NeuroLinkSource *p_source , NeuroLinkTarget *p_target );

	virtual void apply( NeuroVector *srcData ) = 0;

public:
// utilities
	String id;
	String transmitter;

// references
	MindRegionLink *regionLink;
	NeuroLinkInfo *linkInfo;
	NeuroLinkSource *source;
	NeuroLinkTarget *target;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkSet : public Object {
public:
	const char *getClass() { return( "NeuroLinkSet" ); };

	void addSetItem( NeuroLink *link );

public:
// own data
	ClassList<NeuroLink> list;
};

/*#########################################################################*/
/*#########################################################################*/

class ExcitatoryLink : public NeuroLink {
public:
	ExcitatoryLink( MindRegionLink *regionLink );
	virtual const char *getClass() { return( "ExcitatoryLink" ); };

public:
	virtual void apply( NeuroVector *srcData );
};

/*#########################################################################*/
/*#########################################################################*/

class InhibitoryLink : public NeuroLink {
public:
	InhibitoryLink( MindRegionLink *regionLink );
	virtual const char *getClass() { return( "InhibitoryLink" ); };

public:
	virtual void apply( NeuroVector *srcData );
};

/*#########################################################################*/
/*#########################################################################*/

class ModulatoryLink : public NeuroLink {
public:
	ModulatoryLink( MindRegionLink *regionLink );
	virtual const char *getClass() { return( "ModulatoryLink" ); };

public:
	virtual void apply( NeuroVector *srcData );
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkSource : public Object {
public:
	NeuroLinkSource();
	virtual const char *getClass() { return( "NeuroLinkSource" ); };

public:
	void addNeuroLink( NeuroLink *link );
	void setSourceVector( NeuroVector *data );
	NeuroVector *getSourceVector();

	void sendMessage( MindRegion *region );

private:
// references
	NeuroVector *data;
	ClassList<NeuroLink> links;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkTarget : public Object {
public:
	NeuroLinkTarget();
	virtual const char *getClass() { return( "NeuroLinkTarget" ); };

public:
	void setHandler( MindRegion *region , MindRegion::NeuroLinkHandler pfn );

public:
	MindRegion *region;
	MindRegion::NeuroLinkHandler pfn;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDMATTER_H
