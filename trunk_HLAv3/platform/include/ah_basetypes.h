#ifndef	INCLUDE_AH_BASETYPES_H
#define INCLUDE_AH_BASETYPES_H

// #############################################################################
// #############################################################################

extern "C" {
#include <library/generic/include/__gen.h>
}

#include <stdexcept>
#include <iostream>

using namespace std;

class String;
class RuntimeException;
class Xml;
class MultiIndexIterator;
class MapStringToInt;
class MapStringToString;
class StringList;
class Value;
class Random;
class Timer;

// #############################################################################
// #############################################################################

class String {
public:
	String();
	~String();
	String( const char *s );
	String( const String& s );
	String( const char *s , int len );

	String& operator +=( const char *v );
	String& operator +=( char v );
	String& operator +=( int v );
	String& operator +=( float v );
	operator const char *() const;
	String& operator =( const char *s );
	String& operator =( String& s );

	bool equals( const char *s ) const;
	void clear();
	String& append( int count , char c );
	String& append( const char *s , int nChars );
	String& assign( const char *s , int nChars );
	void remove( int from , int n );
	void insert( int from , const char *s );

	void resize( int length );
	char *getBuffer();
	int size() const;
	int length() const;
	bool isEmpty() const;

	int find( const char *substring ) const;
	int find( int startfrom , const char *substring ) const;
	int findLastAny( const char *chars ) const;
	int findLast( char c ) const;
	String getMid( int from , int n ) const;
	String getMid( int from ) const;
	char getChar( int index ) const;
	String toUpper() const;
	String toLower() const;
	static String toHex( int value );
	int split( StringList& parts , String delimiter ) const;
	bool startsFrom( const char *substring ) const;

	static String parseStringLiteral( const char *p );

private:
	void createFromString( const char *s );
private:
	char *v;
};

String operator +( const String& s1 , const char *s2 );
String operator +( const String& s1 , int value );
String operator +( const String& s1 , unsigned value );
String operator +( const String& s1 , float value );
String operator +( const String& s1 , double value );
String operator +( const String& s1 , char value );
String operator +( const String& s1 , bool value );

// #############################################################################
// #############################################################################

#define RuntimeError( msg ) RuntimeException( msg , __FILE__ , __LINE__ )

// utility functions & classes
#define ASSERT( x ) if( !(x) ) throw RuntimeError( String( "assertion failed (" ) + (#x) + ")" )
#define ASSERTMSG( x , msg ) if( !(x) ) throw RuntimeError( String( "assertion failed (" ) + (#x) + "): " + (msg) )
#define ASSERTFAILED( msg ) throw RuntimeError( String( "assertion failed: " ) + msg )

class RuntimeException {
public:
	RuntimeException( unsigned int code , int skipLevels , void *p_addr );
	RuntimeException( const char *p_msg , const char *p_file , int p_line );
	String printStackToString();

	bool isSEH();
	const char *getClassName();
	const char *getFunctionName();
	const char *getMsg();
	const char *getFileShort();
	int getLine();
	rfc_threadstack *getStack();
	int getSkipCount();

public:
	bool seh;

	String className;
	String functionName;
	String msg;
	String file;
	String fileShort;
	int line;
	rfc_threadstack *stack;
};

/*#########################################################################*/
/*#########################################################################*/

class Xml {
public:
	Xml();
	void attach( void *doc , void *node );
	bool exists();
	void destroy();

	static Xml parse( const char *src , int& len , bool& p_error );
	static Xml read( const char *data , const char *contentName );
	static Xml create( const char *contentName );
	static Xml load( const char *path );

	// direct data
	String getName();
	String getValue();
	String serialize();
	String getHeading();

	// navigation
	Xml getChildNode( String s );
	Xml getChildNamedNode( String element , String name );
	Xml getFirstChild( String name );
	Xml getNextChild( String name );
	// path items are delimited by '/' chars
	Xml findChildByPath( String path );
	Xml findChildByPathAttr( String path , String attr , String value );

	// attributes
	String getAttribute( String a );
	String getAttribute( String a , String defaultValue );
	bool getBooleanAttribute( String a );
	bool getBooleanAttribute( String a , bool defaultValue );
	int getIntAttribute( String a );
	int getIntAttribute( String a , int defaultValue );
	void setAttribute( String a , String value );
	void setBooleanAttribute( String a , bool value );
	void setIntAttribute( String a , int value );

	// properties
	void getProperties( MapStringToString& map );
	String getProperty( String name );
	String getProperty( String name , String defaultValue );
	bool getBooleanProperty( String name );
	bool getBooleanProperty( String name , bool defaultValue );
	int getIntProperty( String name );
	int getIntProperty( String name , int defaultValue );
	float getFloatProperty( String name );
	float getFloatProperty( String name , float defaultValue );
	void setProperty( String name , String value );
	void setBooleanProperty( String name , bool value );
	void setIntProperty( String name , int value );
	void setFloatProperty( String name , float value );

	// elements
	Xml addTextElement( String name , String value );
	Xml addElement( String name );

private:
	void *doc;
	void *node;
};

// #############################################################################
// #############################################################################

// map String to int
class MapStringToInt {
public:
	MapStringToInt() { mapData = rfc_map_strcreate(); };
	~MapStringToInt() { rfc_map_strdrop( mapData ); };

public:
	void allocate( int count ) { rfc_map_stralloc( mapData , count ); };

	int add( const char *key , int value ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) >= 0 )
			throw RuntimeError( "MapStringToInt::add - key already exists" );

		int index = rfc_map_strsetkey( mapData , key , ( void * )value );
		return( index );
	};

	void add( MapStringToInt& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ )
			rfc_map_stradd( mapData , s -> s_p[ k ].s_x , s -> s_p[ k ].s_y );
	};

	int set( const char *key , int value ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			l_value = -1;

		rfc_map_strsetkey( mapData , key , ( void * )value );
		return( l_value );
	};

	int get( const char *key ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			return( -1 );

		return( l_value );
	};
	// return item from which given string begins
	int getPartial( const char *key ) {
		int n = rfc_map_strcount( mapData );
		if( n == 0 )
			return( -1 );

		int fp = rfc_map_strinsertpos( mapData , key );

		// check returned - it could be equal to required
		if( fp < n ) {
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strcmp( kfp , key ) )
				return( ( int )mapData -> s_p[ fp ].s_y );
		}

		// check previous - it could be partially equal to returned
		if( fp > 0 ) {
			fp--;
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strncmp( kfp , key , strlen( kfp ) ) )
				return( ( int )mapData -> s_p[ fp ].s_y );
		}

		return( -1 );
	};

	int remove( const char *key ) { return( ( int )rfc_map_strremove( mapData , key ) ); };

	int count() { return( rfc_map_strcount( mapData ) ); };

	void destroy() { rfc_map_strclear( mapData ); };

	int getClassByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( ( int )mapData -> s_p[ k ].s_y );
	};
			
	const char *getKeyByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_x );
	};
			
private:
	rfc_strmap *mapData;
};

// #############################################################################
// #############################################################################

// map String to String
class MapStringToString {
public:
	MapStringToString() { mapData = rfc_map_strcreate(); };
	~MapStringToString() {
		destroy();
		rfc_map_strdrop( mapData );
	};

public:
	void allocate( int count ) { rfc_map_stralloc( mapData , count ); };

	int add( const char *key , const char *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) >= 0 )
			throw RuntimeError( "MapStringToInt::add - key already exists" );

		char *setValue = _strdup( value );
		int index = rfc_map_strsetkey( mapData , key , ( void * )setValue );
		return( index );
	};

	void add( MapStringToString& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ ) {
			char *setValue = _strdup( ( char * )s -> s_p[ k ].s_y );
			rfc_map_stradd( mapData , s -> s_p[ k ].s_x , setValue );
		}
	};

	void set( const char *key , const char *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			l_value = NULL;
		else
			free( l_value );

		char *setValue = _strdup( value );
		rfc_map_strsetkey( mapData , key , ( void * )setValue );
	};

	const char *get( const char *key ) {
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			return( NULL );

		return( l_value );
	};
	// return item from which given string begins
	const char *getPartial( const char *key ) {
		int n = rfc_map_strcount( mapData );
		if( n == 0 )
			return( NULL );

		int fp = rfc_map_strinsertpos( mapData , key );

		// check returned - it could be equal to required
		if( fp < n ) {
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strcmp( kfp , key ) )
				return( ( const char * )mapData -> s_p[ fp ].s_y );
		}

		// check previous - it could be partially equal to returned
		if( fp > 0 ) {
			fp--;
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strncmp( kfp , key , strlen( kfp ) ) )
				return( ( const char * )mapData -> s_p[ fp ].s_y );
		}

		return( NULL );
	};

	void remove( const char *key ) {
		char *value = ( char * )rfc_map_strremove( mapData , key );
		free( value );
	};

	int count() { return( rfc_map_strcount( mapData ) ); };

	void destroy() {
		for( int k = 0; k < mapData -> s_n; k++ )
			free( ( char * )mapData -> s_p[ k ].s_y );
		rfc_map_strclear( mapData );
	};

	const char *getClassByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( ( const char * )mapData -> s_p[ k ].s_y );
	};
			
	const char *getKeyByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_x );
	};
			
private:
	rfc_strmap *mapData;
};

// #############################################################################
// #############################################################################

extern "C" static int lstcompareStringListAscend( void *p_userdata , const RFC_TYPE *p_e1 , const RFC_TYPE *p_e2 );
extern "C" static int lstcompareStringListDescend( void *p_userdata , const RFC_TYPE *p_e1 , const RFC_TYPE *p_e2 );

class StringList {
public:
	StringList() {
		data = rfc_lst_create( RFC_EXT_TYPESTRING );
	};
	~StringList() {
		rfc_lst_destroy( data );
	};

	int add( const char *value ) {
		RFC_TYPE v;
		v.u_c = value;
		int index = rfc_lst_add( data , &v );
		return( index );
	}
	int count() {
		return( data -> s_n );
	}
	String get( int index ) {
		ASSERT( index >= 0 && index < data -> s_n );
		return( rfc_lst_get( data , index ) -> u_c );
	}
	void set( int index , const char *value ) {
		ASSERT( index >= 0 && index < data -> s_n );
		RFC_TYPE v;
		v.u_c = value;
		rfc_lst_replace( data , index , &v );
	}
	int find( String file ) {
		for( int k = 0; k < data -> s_n; k++ )
			if( file.equals( data -> s_p[ k ].u_c ) )
				return( k );
		return( -1 );
	}

	void sort() {
		rfc_lst_sort( data , NULL , NULL );
	}

	rfc_list *data;
};

// #############################################################################
// #############################################################################

class Value {
public:
	RFC_TYPE value;
	int type;

public:
	Value( long v ) { value.u_l = v; type = RFC_EXT_TYPELONG; };
	Value( double v ) { value.u_f = v; type = RFC_EXT_TYPEDOUBLE; };
	Value( RFC_INT64 v ) { value.u_m = v; type = RFC_EXT_TYPEMONEY; };
	Value( const char *v ) { if( v == NULL ) v = ""; value.u_s = _strdup( v ); type = RFC_EXT_TYPECHAR; };
	Value( short v ) { value.u_d = v; type = RFC_EXT_TYPESHORT; };
	Value( float v ) { value.u_r = v; type = RFC_EXT_TYPESHORT; };
	~Value() { if( type == RFC_EXT_TYPECHAR ) free( value.u_s ); };

	long getLong() { ASSERTMSG( type == RFC_EXT_TYPELONG , "RFC_EXT_TYPELONG expected" ); return( value.u_l ); };
	double getDouble() { ASSERTMSG( type == RFC_EXT_TYPEDOUBLE , "RFC_EXT_TYPEDOUBLE expected" ); return( value.u_f ); };
	RFC_INT64 getInt64() { ASSERTMSG( type == RFC_EXT_TYPEMONEY , "RFC_EXT_TYPEMONEY expected" ); return( value.u_m ); };
	const char *getString() { ASSERTMSG( type == RFC_EXT_TYPECHAR , "RFC_EXT_TYPECHAR expected" ); return( value.u_s ); };
	short getShort() { ASSERTMSG( type == RFC_EXT_TYPESHORT , "RFC_EXT_TYPESHORT expected" ); return( value.u_d ); };
	float getFloat() { ASSERTMSG( type == RFC_EXT_TYPEFLOAT , "RFC_EXT_TYPEFLOAT expected" ); return( value.u_r ); };

	Value& operator =( const Value& cv ) {
		if( cv.type == RFC_EXT_TYPECHAR )
			value.u_s = _strdup( cv.value.u_s );
		else
			value.u_m = cv.value.u_m;
		type = cv.type;
		return( *this );
	}
};

// #############################################################################
// #############################################################################

class Random {
	typedef enum {
		RND_UNKNOWN = 0 ,
		RND_RANGEFLOAT = 1 ,
		RND_TWORANGEFLOAT = 2 ,
		RND_RANGEINT = 3 ,
		RND_TWORANGEINT = 4
	} RandomType;

public:
	Random();
	~Random();

	void createRangeRandomsFloat( float min , float max );
	void createTwoRangeRandomsFloat( float min1 , float max1 , float min2 , float max2 );
	void createRangeRandomsInt( int min , int max );
	void createTwoRangeRandomsInt( int min1 , int max1 , int min2 , int max2 );

	int getRandomInt();
	float getRandomFloat();
	float getRandomFloat( float min , float max );

	void collectStatisticsInt();
	void collectStatisticsFloat( int count );
	void showStatistics();

	static int getRandomIntStatic( int min , int max );
	static float getRandomFloatStatic( float min , float max );

private:
	static float randFloat();

private:
	static int instanceCount;
	RandomType type;
	RFC_TYPE min1;
	RFC_TYPE max1;
	RFC_TYPE min2;
	RFC_TYPE max2;

	// random stat
	bool collect;
	int *statCounts;
	int statBuckets;
	float bucketSize;
};

// #############################################################################
// #############################################################################

class Timer {
public:
	Timer();
	// do not set interval, set/not set starting timestamp
	void createWithStartingTimestamp();
	// set interval in ms and initial timestamp
	void createRunWindowMs( int runTimeMs );
	// set interval in seconds and initial timestamp
	void createRunWindowSec( int runTimeSec );

	// initial adjustments
	static void startAdjustment();
	static void stopAdjustment();
	// time passed from process start - in ms
	static int timeSinceProcessStartMs();

public:
	// timestamp of timer construction - in ms
	int timeCreatedMs();
	// time passed - in ms
	int timePassedMs();
	// time passed - in clocks
	int timePassedClocks();
	// time passed - in ticks
	int timePassedTicks();
	// time remained - in secs
	int timeRemainedSec();

	// convert clocks to ms
	static int timeClocksToMs( int clocks );
	// convert ms to clocks
	static int timeMsToClocks( int ms );
	// convert ticks to ms
	static int timeTicksToMs( int ticks );
	// convert ms to ticks
	static int timeMsToTicks( int ms );

	// check current time is within run window
	bool go();
	// wait next interval, rounded to seconds
	int waitNextSecs();

private:
	long timeStarted;
	RFC_INT64 timeStartedTicks;
	int waitTimeMs;
	int waitCount;
};

// #############################################################################
// #############################################################################

#endif	// INCLUDE_AH_BASETYPES_H
