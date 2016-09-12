#ifndef TrackAnalysis_ElectronTrackEfficiencyAnalysis_H
#define TrackAnalysis_ElectronTrackEfficiencyAnalysis_H

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <EventLoop/Algorithm.h>

#include "TH1D.h"
#include "TProfile.h"

#ifndef __MAKECINT__
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#endif // not __MAKECINT__

class ElectronTrackEfficiencyAnalysis : public EL::Algorithm
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



  // this is a standard constructor
  ElectronTrackEfficiencyAnalysis ();

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

  void getBremInfo( const xAOD::TruthParticle* truth, double& energyLostR600, double& energyLostR150, double& radius20Loss, bool& isReconstructable) const;

#endif // not __MAKECINT__

  xAOD::TEvent *m_event;                   //!
  unsigned int  m_eventCounter;            //!


  TH1D*      m_TruevsEloss; //!
  TH1D*      m_TruevsEloss150; //!
  TH1D*      m_TruevsPt;    //!
  TH1D*      m_TruevsEta;   //!
  TH1D*      m_TruevsBremR; //!
 
  TH1D*      m_TrackvsEloss; //!
  TH1D*      m_TrackvsEloss150; //!
  TH1D*      m_TrackvsPt;    //!
  TH1D*      m_TrackvsEta;   //!
  TH1D*      m_TrackvsBremR; //!

  TH1D*      m_DupTrackvsEloss; //!
  TH1D*      m_DupTrackvsEloss150; //!
  TH1D*      m_DupTrackvsPt;    //!
  TH1D*      m_DupTrackvsEta;   //!
  TH1D*      m_DupTrackvsBremR; //!

  TH1D*      m_TrueRvsEloss; //!
  TH1D*      m_TrueRvsEloss150; //!
  TH1D*      m_TrueRvsPt;    //!
  TH1D*      m_TrueRvsEta;   //!
  TH1D*      m_TrueRvsBremR; //!
 
  TH1D*      m_TrackRvsEloss; //!
  TH1D*      m_TrackRvsEloss150; //!
  TH1D*      m_TrackRvsPt;    //!
  TH1D*      m_TrackRvsEta;   //!
  TH1D*      m_TrackRvsBremR; //!


  TH1D*      m_TruevsElossEta; //!
  TH1D*      m_TruevsEloss150Eta; //!
  TH1D*      m_TruevsPtEta;    //!
  TH1D*      m_TruevsBremREta; //!
 
  TH1D*      m_TrackvsElossEta; //!
  TH1D*      m_TrackvsEloss150Eta; //!
  TH1D*      m_TrackvsPtEta;    //!
  TH1D*      m_TrackvsBremREta; //!

  TH1D*      m_TrueRvsElossEta; //!
  TH1D*      m_TrueRvsEloss150Eta; //!
  TH1D*      m_TrueRvsPtEta;    //!
  TH1D*      m_TrueRvsBremREta; //!
 
  TH1D*      m_TrackRvsElossEta; //!
  TH1D*      m_TrackRvsEloss150Eta; //!
  TH1D*      m_TrackRvsPtEta;    //!
  TH1D*      m_TrackRvsBremREta; //!

  
  TProfile*  m_SiHitsvsPt;   //!
  TProfile*  m_SiHitsvsEta;  //!
  TProfile*  m_PixHitsvsPt;  //!
  TProfile*  m_PixHitsvsEta; //!  
  TH1D*      m_TRTExtensionsvsPt;  //!   
  TH1D*      m_TRTExtensionsvsEta; //!   

  TProfile*  m_SiHitsRvsPt;   //!
  TProfile*  m_SiHitsRvsEta;  //!
  TProfile*  m_PixHitsRvsPt;  //!
  TProfile*  m_PixHitsRvsEta; //!  
  TH1D*      m_TRTExtensionsRvsPt;  //!   
  TH1D*      m_TRTExtensionsRvsEta; //!   

  TH1D*  h_chi2ndf;   //!

  TH1D*  h_d0;        //!
  TH1D*  h_z0;        //!
  TH1D*  h_phi;       //!
  TH1D*  h_theta;     //!
  TH1D*  h_qoverp;    //!

  TH1D*  h_Pulld0;        //!
  TH1D*  h_Pullz0;        //!
  TH1D*  h_Pullphi;       //!
  TH1D*  h_Pulltheta;     //!
  TH1D*  h_Pullqoverp;    //!



  // this is needed to distribute the algorithm to the workers
  ClassDef(ElectronTrackEfficiencyAnalysis, 1);
};

#endif
