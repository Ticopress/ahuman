#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

class ExcitatoryLink : public NeuroLink {
public:
	ExcitatoryLink( MindRegionLink *regionLink );
	virtual const char *getClass() { return( "ExcitatoryLink" ); };

public:
	virtual NeuroSignal *apply( NeuroSignal *srcData , NeuroPool *dstPool );

private:
	void recalculateActionPotential( NeuroSignal *srcData , NeuroPool *dstPool );
	void activateMembranePotential( NeuroSignal *srcData , NeuroPool *dstPool );

private:
	int opid;
};

/*#########################################################################*/
/*#########################################################################*/

NeuroLink *MindService::createExcitatoryLink( MindRegionLink *link ) { return( new ExcitatoryLink( link ) ); };

/*#########################################################################*/
/*#########################################################################*/

ExcitatoryLink::ExcitatoryLink( MindRegionLink *p_regionLink ) : NeuroLink( p_regionLink ) {
	attachLogger();
	opid = 0;
}

NeuroSignal *ExcitatoryLink::apply( NeuroSignal *srcData , NeuroPool *dstPool ) {
	opid++;

	// log 
	LOGDEBUG( String( "apply: project NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + "..." );

	// 1. recalculate accumulated pre-synaptic action potential
	recalculateActionPotential( srcData , dstPool );

	// 2. recalculate membrane potential
	activateMembranePotential( srcData , dstPool );

	// 3. fire
	NeuroSignal *ffSignal = dstPool -> fire( srcData );

	// log 
	int signalSize = srcData -> getDataSize();
	int affectedCount = ffSignal -> getDataSize();
	int projectionRate = ( affectedCount * 100 ) / signalSize;
	logger.logInfo( String( "apply: projected NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + ", signalCount=" + signalSize + ", affectedCount=" + affectedCount + ", projectionRate=" + projectionRate );

	StatService *ss = StatService::getService();
	ss -> addMetricValue( getId() + ".link.projection.srcsize" , signalSize );
	ss -> addMetricValue( getId() + ".link.projection.dstsize" , affectedCount );

	// check signal is required
	if( ffSignal -> getDataSize() == 0 ) {
		delete ffSignal;
		return( NULL );
	}

	return( ffSignal );
}

void ExcitatoryLink::recalculateActionPotential( NeuroSignal *srcData , NeuroPool *dstPool ) {
	// current timestamp
	RFC_INT64 msNow = Timer::getCurrentTimeMillis();

	// map source surface to target surface - as linear arrays
	int dtotal = dstPool -> getNeuronDataSize();
	TwoIndexArray<NEURON_DATA>& dstDataNeurons = dstPool -> getNeuronData();
	NEURON_DATA *dvdata = dstDataNeurons.getData();

	// read activated indexes
	int sn = srcData -> getDataSize();
	int stotal = srcData -> getMaxSize();
	int *sv = srcData -> getIndexRawData();

	for( int k = 0; k < sn; k++ ) {
		// get value and project
		int sk = *sv++;
		int dk = ( int )( ( sk * (RFC_INT64)dtotal ) / stotal );

		// get current potential and handle timestamps
		NEURON_DATA *dv = dvdata + dk;
		neurovt_state lastCharge = dv -> synaptic_potential;
		RFC_INT64 msPassed = msNow - dv -> updated_mp;
		dv -> updated_mp = msNow;

		// calculate current value of action potential
		neurovt_state currentCharge = lastCharge;
			
		if( msPassed < NEURON_FULL_RELAX_ms ) {
			if( currentCharge >= 0 ) {
				currentCharge -= ( ( neurovt_state )msPassed ) * NEURON_POTENTIAL_DISCHARGE_RATE_pQ_per_ms;
				if( currentCharge < 0 )
					currentCharge = 0;
			}
			else {
				currentCharge += ( ( neurovt_state )msPassed ) * NEURON_POTENTIAL_DISCHARGE_RATE_pQ_per_ms;
				if( currentCharge > 0 )
					currentCharge = 0;
			}
		}
		else
			currentCharge = 0;

		// add action potential
		currentCharge += NEURON_ACTION_POTENTIAL_BY_SIGNAL_pQ;

		// save new value
		dv -> synaptic_potential = currentCharge;

		LOGDEBUG( String( "recalculateActionPotential: projected NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + ", spos=" + sk + ", dpos=" + dk + ", lastCharge=" + lastCharge + ", newCharge=" + currentCharge + ", msPassed=" + ( int )msPassed );
	}
}

void ExcitatoryLink::activateMembranePotential( NeuroSignal *srcData , NeuroPool *dstPool ) {
	int sn = srcData -> getDataSize();
	int stotal = srcData -> getMaxSize();
	int *sv = srcData -> getIndexRawData();
	int dtotal = dstPool -> getNeuronDataSize();
	TwoIndexArray<NEURON_DATA>& dstDataNeurons = dstPool -> getNeuronData();
	NEURON_DATA *dvdata = dstDataNeurons.getData();

	RFC_INT64 msNow = Timer::getCurrentTimeMillis();

	for( int k = 0; k < sn; k++ ) {
		// get value and project
		int sk = *sv++;
		int dk = ( int )( ( sk * (RFC_INT64)dtotal ) / stotal );
		NEURON_DATA *dv = dvdata + dk;

		RFC_INT64 msSilentTill = dv -> silent_till;

		// check need to generate membrane potential
		if( dv -> synaptic_potential >= dv -> synaptic_threshold ) {
			// cannot increase membrane potential in silent time
			if( msNow < msSilentTill ) {
				// use energy to improve connectivity
				dv -> synaptic_potential = 0;
				dv -> synaptic_threshold = ( dv -> synaptic_threshold * NEURON_CONNECTIVITY_UPDATE_FACTOR ) / 100;
				if( dv -> synaptic_threshold < NEURON_SYNAPTIC_THRESHOLD_MIN_pQ )
					dv -> synaptic_threshold = NEURON_SYNAPTIC_THRESHOLD_MIN_pQ;
				LOGDEBUG( String( "activateMembranePotential: do not fire index=" ) + dk + ", msRemained=" + ( int )( msSilentTill - msNow ) + ", new synaptic_threshold=" + dv -> synaptic_threshold );
				continue;
			}

			// affect membrane potential
			dv -> membrane_potential += NEURON_MEMBRANE_POTENTIAL_BY_ACTION_POTENTIAL_pQ;
			dv -> synaptic_potential = 0;

			LOGDEBUG( String( "activateMembranePotential: NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + ", spos=" + sk + ", dpos=" + dk + ", membrane_potential=" + dv -> membrane_potential + ", threshold=" + dv -> membrane_threshold );
		}
	}
}

