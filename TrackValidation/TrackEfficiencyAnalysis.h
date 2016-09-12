#ifndef TrackValidation_TrackEfficiencyAnalysis_H
#define TrackValidation_TrackEfficiencyAnalysis_H

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <EventLoop/Algorithm.h>

#include "TrackValidation/TrackEfficiencyHists.h"
#include "TrackValidation/TrackQualityHists.h"

#ifndef __MAKECINT__
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticle.h"
#endif // not __MAKECINT__

class TrackEfficiencyAnalysis : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;



  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!

   // pixels
      TH2 *myXY; //!
      TH2 *myXZ; //!
      TH1 *myZ; //!
      TH1 *myX; //!
      TH1 *myC; //!
      TH1 *myTOT; //!
      TH1 *myLayer; //!
                        
      // strips
       TH2 *stXY; //!
       TH2 *stXZ; //!
       TH1 *stZ; //!
       TH1 *stX; //!
       TH1 *stC; //!
       TH1 *stTOT; //!
       TH1 *stLayer; //!
      
       TH1 *debug_events; //!
       TH1 *debug_tracks; //!


       TH1 *res1gev; //!
       TH1 *res10gev; //!
       TH1 *res100gev; //!
                
       TH1 *mean1gev; //!
       TH1 *mean10gev; //!
       TH1 *mean100gev; //!

       TH2 *barcodePrim; //!
       TH2 *barcodeSec; //!

  // this is a standard constructor
  TrackEfficiencyAnalysis ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

// Everything in the header file  that refers to the xAOD edm needs 
#ifndef __MAKECINT__
  bool passTrackQualitySelection(const xAOD::TrackParticle* );
  bool passIP_Selection(const xAOD::TrackParticle*, const xAOD::Vertex*);
#endif // not __MAKECINT__

  xAOD::TEvent *m_event;                   //!
  unsigned int  m_eventCounter;            //!
 
  
  TrackEfficiencyHists* m_PrimaryTracks;   //!
  TrackEfficiencyHists* m_SecondaryTracks; //!
  TrackEfficiencyHists* m_PrimaryTruth;    //!
  TrackEfficiencyHists* m_SecondaryTruth;  //!
  TrackEfficiencyHists* m_Truth;  //!

  TrackEfficiencyHists* m_DuplicatePrimaryTracks;   //!
  TrackEfficiencyHists* m_DuplicateSecondaryTracks; //!


  TrackEfficiencyHists* m_SelectedPrimaryTracks;   //!
  TrackEfficiencyHists* m_SelectedSecondaryTracks; //!
  TrackEfficiencyHists* m_SelectedPrimaryFakeTracks;   //!
  TrackEfficiencyHists* m_SelectedSecondaryFakeTracks; //!
  TrackEfficiencyHists* m_SelectedDuplicatePrimaryTracks;   //!
  TrackEfficiencyHists* m_SelectedDuplicateSecondaryTracks; //!
  TrackEfficiencyHists* m_SelectedAllTracks;       //!
  TrackEfficiencyHists* m_SelectedFakeTracks;      //!


  TrackEfficiencyHists* m_AllTracks;       //!
  TrackEfficiencyHists* m_FakeTracks;      //!

  TrackQualityHists*    m_QualAllTracks;    //!
  TrackQualityHists*    m_QualSelTracks;    //!
  TrackQualityHists*    m_QualSelFakeTracks;//!

  // this is needed to distribute the algorithm to the workers
  ClassDef(TrackEfficiencyAnalysis, 1);
};

#endif
