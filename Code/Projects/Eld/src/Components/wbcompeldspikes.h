#ifndef WBCOMPELDSPIKES_H
#define WBCOMPELDSPIKES_H

#include "wbeldcomponent.h"

class WBCompEldSpikes : public WBEldComponent
{
public:
	WBCompEldSpikes();
	virtual ~WBCompEldSpikes();

	DEFINE_WBCOMP( EldSpikes, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void	HandleOnTouched( WBEntity* const pTouched );
	bool	ShouldSendSpikedEvent( WBEntity* const pTouched );
	void	SendSpikedEvent( WBEntity* const pTouched );

	float	m_SpeedThresholdSq;			// Config
	bool	m_CheckMovingDown;			// Config
	float	m_RecentlyLandedThreshold;	// Config, only applies to CheckMovingDown
};

#endif // WBCOMPELDSPIKES_H
