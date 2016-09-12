#ifndef TrackAnalysis_TrackTruthHelper_H
#define TrackAnalysis_TrackTruthHelper_H

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackParticleContainer.h"



namespace TrackTruthHelpers{
  /** Check if the truth particle is a primary charged particle within detector acceptance*/
  bool isPrimary( const xAOD::TruthParticle*   );

  /** Check if the truth particle is a secondary charged particle within detector acceptance*/
  bool isSecondary( const xAOD::TruthParticle* );

  /** Check if the truth particle is a combinatorial fake or has no truth associated to it*/
  bool isFake( const xAOD::TrackParticle*      );

  /** Check if the track particle originated from is a primary charged particle within detector acceptance*/
  bool isPrimary( const xAOD::TrackParticle*   );

  /** Check if the track particle originated from is a secondary charged particle within detector acceptance*/
  bool isSecondary( const xAOD::TrackParticle* );

  /** Get the truth particle from associated to a track particle.  Return 0 if there is no truth particle associated*/
  const xAOD::TruthParticle* truthParticle(const xAOD::TrackParticle *trkPart) ;

  /** Get truth matching probability*/
  float getMatchingProbability( const xAOD::TrackParticle *trkPart   );

}


#endif

