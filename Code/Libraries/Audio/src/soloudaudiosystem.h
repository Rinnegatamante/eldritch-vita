#ifndef SOLOUDAUDIOSYSTEM_H
#define SOLOUDAUDIOSYSTEM_H

#ifdef __vita__
#include <vitasdk.h>
#endif

#include "audiosystemcommon.h"
#include "map.h"
#include "set.h"
#include "list.h"
#include "hashedstring.h"
#include "array.h"
#include "interpolator.h"
#include "simplestring.h"

#include "soloud.h"
#include "soloud_echofilter.h"

struct SoLoudAudioCommand
{
	SimpleString	m_DefinitionName;
	Vector		  	m_Location;
	float		   	m_VolumeOverride;
};

struct SoLoudAudioResult
{
	ISoundInstance* m_Instance;
	Vector		  	m_Location;
	float		   	m_VolumeOverride;
};

struct SoLoudPropCache
{
	SoLoudPropCache()
	:	m_Channels( 0 )
	,	m_SampleRate( 0.0f )
	,	m_SampleCount( 0 )
	{
	}

	uint	m_Channels;
	float	m_SampleRate;
	uint	m_SampleCount;
};

class SoLoudAudioSystem : public AudioSystemCommon
{
public:
	SoLoudAudioSystem();
	virtual ~SoLoudAudioSystem();

	virtual void			Tick( const float DeltaTime, bool GamePaused );
	virtual ISound*			CreateSound( const SSoundInit& SoundInit );
	virtual ISoundInstance*	Play( const SimpleString& DefinitionName, const Vector& Location );
	virtual void			SetReverbParams( const SimpleString& DefinitionName ) const;
	virtual void			ConditionalApplyReverb( ISoundInstance* const pSoundInstance ) const { Unused( pSoundInstance ); /*not needed for SoLoud, all done with buses*/ }
#if BUILD_DEV
	virtual void			ReverbTest_Toggle();
	virtual void			ReverbTest_Update();
	virtual bool			ReverbTest_IsActive() const;
	virtual void			ReverbTest_Print() const;
	virtual void			ReverbTest_Export() const;
	virtual void			ReverbTest_PrevSetting();
	virtual void			ReverbTest_NextSetting();
	virtual void			ReverbTest_IncrementSetting( const float Scalar );
	virtual void			ReverbTest_DecrementSetting( const float Scalar );
#endif

	SoLoud::Soloud*			GetSoLoudEngine() const		{ return m_SoLoudEngine; }
	SoLoud::Bus*			GetSoLoudEchoBus() const	{ return m_SoLoudEchoBus; }

	bool					IsReverbCategory( const HashedString& Category ) const	{ return m_ReverbCategories.Find( Category ); }

	const SoLoudPropCache*	GetPropCache( const HashedString& Filename ) const;
	void					SetPropCache( const HashedString& Filename, const SoLoudPropCache& PropCache );
	void					EnqueueSound( const SimpleString& Def, const Vector& Location, float Volume );
	void					FlushReadyInstances();
	void					AudioWorkerRun();
	
	static int	  			AudioWorkerEntry( SceSize args, void* argp );
	
private:
	void					ApplyReverb( const float EchoTime, const float DecayTime, const float LowPassFilter, const float WetDryMix ) const;

	SoLoud::Soloud*			m_SoLoudEngine;
	SoLoud::Bus*			m_SoLoudEchoBus;
	SoLoud::EchoFilter*		m_SoLoudEchoFilter;
	SoLoud::handle			m_SoLoudEchoBusHandle;

	SimpleString		m_DefaultReverb;

	Array<HashedString>	m_ReverbCategories;	// Categories that has teh reverbs (probably generally the same as pause)

	// Optimization for SoLoud to bypass stb_vorbis_stream_length_in_samples each time a file is reopened
	Map<HashedString, SoLoudPropCache>	m_PropCache;

#if BUILD_DEV
	bool				m_ReverbTest_IsActive;
	uint				m_ReverbTest_SettingIndex;
#endif

#ifdef __vita__
	SceUID		  		m_WorkerThreadId;
	SceUID		  		m_CommandSema;
	SceUID		  		m_CommandMutex;
	SceUID		  		m_ResultMutex;
#endif

	bool						m_WorkerRunning;
	Array<SoLoudAudioCommand>	m_CommandQueue;
	Array<SoLoudAudioResult>	m_ResultQueue;
};

#endif // SOLOUDAUDIOSYSTEM_H