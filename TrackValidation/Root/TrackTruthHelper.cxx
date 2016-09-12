#include <TrackValidation/TrackTruthHelper.h>

using namespace TrackTruthHelpers;


bool TrackTruthHelpers::isPrimary( const xAOD::TruthParticle* tp )
{
  if( tp->status() != 1 )     //Check to see if its a stable particle
    return false;
  
  //Note this const cast will not  be required in the future -- there is bug in the truth edm
  //if( const_cast<xAOD::TruthParticle*>(tp)->isNeutral() )         //Clearly for tracking we don't care about Neutrals 
  //  return false;

   if (tp->isNeutral() == true) return false;

  if( tp->barcode() == 0 ||   //Barcode of zero indicates there was no truth paticle found for this track 
      tp->barcode() >= 200e3) return false; 

 
  if( tp->pt() <= 900.   ||  fabs(tp->eta()) >= 4.0 ) // Check the particle is within acceptance 
    return false;
 
  // InDetPhysValid method 
  bool prod=(!tp->hasProdVtx() || tp->prodVtx()->perp() < 110);
  // make sure particle decays before the last pixel layer
  if( !prod )  return false;

  // original TrackValidation
  //if( !tp->hasProdVtx() || tp->prodVtx()->perp() > 110) return false; 


  return true;
}


bool TrackTruthHelpers::isSecondary( const xAOD::TruthParticle* tp )
{
  if( tp->status() != 1 )     //Check to see if its a stable particle
    return false;

  if (tp->isNeutral() == true) return false;

  if( tp->barcode() < 200e3 )    //larger than this indicates secondary  particles
    return false;
 
  if( tp->pt() < 900.   || fabs(tp->eta()) >  4.0 ) // Check the particle is within acceptance 
    return false;


  // make sure particle decays before the last SCT layer
  if( !tp->hasProdVtx() || tp->prodVtx()->perp() > 400) return false; 

  return true;
}


//get truth/track matching probability
float TrackTruthHelpers::getMatchingProbability(const xAOD::TrackParticle* tp){
  static const char* NAME = "truthMatchProbability";
  if( ! tp->isAvailable< float >( NAME ) ) {
    Warning("isFake(..)", "Track Particle has no MatchProb is this data?" );
    return true;
  }

  float truthProb = tp->auxdata< float >( NAME );
  return truthProb;
}


bool TrackTruthHelpers::isFake( const xAOD::TrackParticle* tp )  
{
  static const char* NAME = "truthMatchProbability";
  if( ! tp->isAvailable< float >( NAME ) ) {
    Warning("isFake(..)", "Track Particle has no MatchProb is this data?" );
    return true;
  }

  const float truthProb = tp->auxdata< float >( NAME );
  if( truthProb < 0.5 ) {
    return true;
  }

  const xAOD::TruthParticle* truth = truthParticle( tp );
  if (truth && truth->barcode()!=0)
    return false;
  
  return true;    
} 

 
bool TrackTruthHelpers::isPrimary( const xAOD::TrackParticle* tp )
{
  const xAOD::TruthParticle* truth =  truthParticle( tp );
  if( !truth  ){
    return false;
  }

  return isPrimary( truth );
}


bool isSecondary( const xAOD::TrackParticle* tp)
{
  const xAOD::TruthParticle* truth =  truthParticle( tp );
  if( !truth  ){
    return false;
  }

  return isSecondary( truth );
}


const xAOD::TruthParticle* TrackTruthHelpers::truthParticle(const xAOD::TrackParticle *trkPart)  
{
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  static const char* NAME = "truthParticleLink";
  if( ! trkPart->isAvailable< Link_t >( NAME ) ) {
    return 0;
  }
  const Link_t& link = trkPart->auxdata< Link_t >( NAME );
  if( ! link.isValid() ) {
    return 0;
  }

  return *link;
} 
