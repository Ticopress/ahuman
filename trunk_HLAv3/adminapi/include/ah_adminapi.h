#ifndef	INCLUDE_AH_ADMINAPI_IMPL_H
#define INCLUDE_AH_ADMINAPI_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <platform\include\ah_platform.h>

class AdminApi;

/*#########################################################################*/
/*#########################################################################*/

class AdminApiSocket;

class AdminApi {
public:
	void initThread();
	void exitThread();

	void connect( String url );
	void disconnect();
	bool isConnected();

	Xml execute();

	Xml createXmlRequest();
	Xml loadXmlRequest( String fileName );
	Xml readXmlRequest( const char *data );

	void setFunctionName( String name );
	void setParam( String name , String value );
	void setBooleanParam( String name , bool value );
	void setIntParam( String name , int value );
	void setFloatParam( String name , float value );

public:
	AdminApi();
	~AdminApi();

private:
	AdminApiSocket *socket;
	Xml lastRequest;
	Xml lastResponse;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_ADMINAPI_IMPL_H
