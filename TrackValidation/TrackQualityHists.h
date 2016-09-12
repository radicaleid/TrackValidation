#ifndef TrackQualityHists_h
#define TrackQualityHists_h

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TString.h"

#ifndef __MAKECINT__
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#endif // not __MAKECINT__

#include <EventLoop/Worker.h>
#include <vector>
#include <set>

struct TrackQualityHists
{

  TString name;      //!

  //Truth based plots

  TH1D*  h_d0;        //!
  TH1D*  h_z0sinT;    //!
  TH1D*  h_phi;       //!
  TH1D*  h_theta;     //!
  TH1D*  h_qoverp;    //!
  TH2D*  h_ptVsPt;    //!

  //Reco Based Plots

  TH1D*  h_d0PV;      //!
  TH1D*  h_z0sinTPV;  //!
  TH2D*  h_d0vsPt;    //!
  TH2D*  h_z0sinTvsPt;//!
  TH2D*  h_d0vsEta;   //!
  TH2D*  h_z0sinTvsEta;//!

  TH1D*  h_Chi2DoF;   //!
  TH1D*  h_Chi2Prob;  //!


  TH1D*  h_nPixel;   		    //!  //Inner Tracker Hits plots
  TH1D*  h_nBL;      		    //!
  TH1D*  h_nSCT;     		    //!
  TH1D*  h_nTRT;     		    //!


  TProfile*  p_SiHitsvsPt;   //!
  TProfile*  p_SiHitsvsEta;  //!
  TProfile*  p_PixHitsvsPt;  //!
  TProfile*  p_PixHitsvsEta; //!  
  TProfile*  p_SCTHitsvsPt;  //!
  TProfile*  p_SCTHitsvsEta; //!  
  TProfile*  p_TRTHitsvsPt;  //!   
  TProfile*  p_TRTHitsvsEta; //!   


  TProfile2D* p_nPixelvsEtaPhi; //!
  TProfile2D* p_nPixelvsEtaPt;  //!
  TProfile2D* p_nSCTvsEtaPhi;   //!
  TProfile2D* p_nSCTvsEtaPt;    //!
  TProfile2D* p_nBLvsEtaPhi;    //! 
  TProfile2D* p_nPixelHolesvsEtaPhi; //! 
  TProfile2D* p_nPixelDeadvsEtaPhi;  //! 
  TProfile2D* p_nSCTHolesvsEtaPhi; //! 
  TProfile2D* p_nSCTDeadvsEtaPhi;  //! 


  std::vector< TH1* > allHists;
  std::vector< TH1* > allProfiles;

  TrackQualityHists(const TString& n)
  {
 
    name = n;


    const int nbinsPt=100;
    Double_t ptbins[nbinsPt+1];
    double dx = 3./nbinsPt;
    double l10 = TMath::Log(10);
    for (int i=0;i<=nbinsPt;i++) {      
      ptbins[i] = TMath::Exp(l10*(i*dx -1.) );
    }
  
    const int nbinsEta=50;
    dx  = 7. / nbinsEta;
    Double_t etabins[nbinsEta+1];
    for (int i=0;i<=nbinsEta;i++) {
      etabins[i] = i*dx - 3.5;
    }

    const int nbinsPhi=50;
    dx  = TMath::Pi() * 2 / nbinsPhi;
    Double_t phibins[nbinsPhi+1];
    for (int i=0;i<=nbinsPhi;i++) {
      phibins[i] = i*dx - TMath::Pi();
    }

    const int nbinsD0=100;
    dx  = 5. / nbinsD0;
    Double_t d0bins[nbinsD0+1];
    for (int i=0;i<=nbinsD0;i++) {
      d0bins[i] = i*dx - 2.5;
    }

    const int nbinsZ0=100;
    dx  = 10. / nbinsZ0;
    Double_t z0bins[nbinsZ0+1];
    for (int i=0;i<=nbinsZ0;i++) {  
      z0bins[i] = i*dx - 5; 
    }
  

    h_d0     =  new TH1D( name + "d0", "; d_{0} (rec-true) [mm]; # Tracks", 1000, -3.5, 3.5 );
    h_z0sinT =  new TH1D( name + "z0sinT", "; z_{0} #times sin#theta (rec-true) [mm]; # Tracks", 1000, -5,5 );
    h_phi    =  new TH1D( name + "phi", "; #phi_{0} (rec-true) [mrad]; # Tracks", 1000, -20, 20 );
    h_theta  =  new TH1D( name + "theta", "; #theta_{0} (rec-true) [mrad]; # Tracks", 1000, -10, 10  );
    h_qoverp =  new TH1D( name + "qoverp", "; q/p  (rec-true)/true; # Tracks", 1000, -1, 1 );

    h_ptVsPt =  new TH2D( name + "ptvsPt", "; p_{T}^{rec}; p_{T}^{true}; # Tracks", nbinsPt, ptbins, nbinsPt, ptbins );
 

    h_Chi2DoF  =  new TH1D( name + "Chi2DoF", "; #chi^{2}/DoF; # Tracks", 1000, 0, 25 );
    h_Chi2Prob =  new TH1D( name + "Chi2Prob", "; #chi^{2} Prob; # Tracks", 1000, 0, 1.000001 );


    h_d0PV     =  new TH1D( name + "d0PV", "; d_{0} (rec-pv) [mm]; # Tracks", 1000, -3.5, 3.5 );
    h_z0sinTPV =  new TH1D( name + "z0sinTPV", "; z_{0} #times #sin#theta (rec-pv) [mm]; # Tracks", 1000, -5,5 );

 
    h_d0vsPt =  new TH2D( name + "d0vsPt", ";  d_{0} (rec-pv) [mm]; p_{T} [GeV]; #Tracks", nbinsD0, d0bins, nbinsPt, ptbins ); 
    h_z0sinTvsPt =  new TH2D( name+ "z0sinTvsPt", ";  z_{0}#times sin#theta (rec-pv) [mm]; p_{T} [GeV]; #Tracks", nbinsZ0, z0bins, nbinsPt, ptbins ); 

    h_d0vsEta =  new TH2D( name + "d0vsEta", ";  d_{0} (rec-pv) [mm]; #eta; #Tracks", 100, -3.5, 3.5, 50, -4.0, 4.0 ); 
    h_z0sinTvsEta =  new TH2D( name + "z0sinTvsEta", ";  z_{0}#times sin#theta (rec-pv) [mm]; #eta; #Tracks", 100, -5, 5, 50, -4.0, 4.0 ); 
 
 
    h_nPixel=  new TH1D( name + "nPixel", "; Pixel Hits; # Tracks", 10, -0.5, 9.5 );
    h_nBL=  new TH1D( name + "nBL", "; Innermost Layer Hits; # Tracks", 10, -0.5, 9.5 );
    h_nSCT=  new TH1D( name + "nSCT", "; SCT Hits; # Tracks", 20, -0.5, 19.5 );
    h_nTRT =  new TH1D( name + "nTRT", "; TRT Hits; # Tracks", 60, -0.5, 59.5 );

 

    p_SiHitsvsPt  =  new TProfile( name + "SiHitsvsPt",";p_{T} [GeV]; <Si Hits>;", nbinsPt, ptbins);    //!
    p_SiHitsvsEta =  new TProfile( name + "SiHitsvsEta","; #eta; <Si Hits>;", nbinsEta, etabins);   //!
    p_PixHitsvsPt =  new TProfile( name + "PixHitsvsPt",";p_{T} [GeV]; <Pixel Hits>;", nbinsPt, ptbins);  //!
    p_PixHitsvsEta =  new TProfile( name + "PixHitsvsEta","; #eta; <Pixel Hits>;", nbinsEta, etabins);  //!  
    p_SCTHitsvsPt  =  new TProfile( name + "SCTHitsvsPt",";p_{T} [GeV]; <SCT Hits>;", nbinsPt, ptbins);  //!   
    p_SCTHitsvsEta =  new TProfile( name + "SCTHitsvsEta","; #eta; <SCT Hits>;", nbinsEta, etabins);  //!   
    p_TRTHitsvsPt  =  new TProfile( name + "TRTHitsvsPt",";p_{T} [GeV]; <TRT Hits>;", nbinsPt, ptbins);  //!   
    p_TRTHitsvsEta =  new TProfile( name + "TRTHitsvsEta","; #eta; <TRT Hits>;", nbinsEta, etabins);  //!   

    p_nPixelvsEtaPhi  =  new TProfile2D( name + "PixelHitsvsEtaPhi",";#eta;#phi [rad]; <Pixel Hits>;", nbinsEta, etabins, nbinsPhi, phibins);  //!   //!
    p_nPixelvsEtaPt   =  new TProfile2D( name + "PixelHitsvsEtaPt",";#eta;p_{T} [GeV]; <Pixel Hits>;", nbinsEta, etabins, nbinsPt, ptbins);  //!    //!
    p_nSCTvsEtaPhi  =  new TProfile2D( name + "SCTHitsvsEtaPhi",";#eta;#phi [rad]; <SCT Hits>;", nbinsEta, etabins, nbinsPhi, phibins);  //!   //!
    p_nSCTvsEtaPt   =  new TProfile2D( name + "SCTHitsvsEtaPt",";#eta;p_{T} [GeV]; <SCT Hits>;", nbinsEta, etabins, nbinsPt, ptbins);  //!    //!

    p_nBLvsEtaPhi  =  new TProfile2D( name + "BLHitsvsEtaPhi",";#eta;#phi [rad]; <Innermost Hits>;", nbinsEta, etabins, nbinsPhi, phibins);  //!   //!
    p_nPixelHolesvsEtaPhi  =  new TProfile2D( name + "PixelHolesHitsvsEtaPhi",";#eta;#phi [rad]; <Pixel Holes>;", nbinsEta, etabins, nbinsPhi, phibins);  //!   //!
    p_nPixelDeadvsEtaPhi  =  new TProfile2D( name + "PixelDeadvsEtaPhi",";#eta;#phi [rad]; <Pixel Dead Modules>;", nbinsEta, etabins, nbinsPhi, phibins);  //!
    p_nSCTHolesvsEtaPhi  =  new TProfile2D( name + "SCTHolesHitsvsEtaPhi",";#eta;#phi [rad]; <SCT Holes>;", nbinsEta, etabins, nbinsPhi, phibins);  //!   //!
    p_nSCTDeadvsEtaPhi  =  new TProfile2D( name + "SCTDeadvsEtaPhi",";#eta;#phi [rad]; <SCT Dead Modules>;", nbinsEta, etabins, nbinsPhi, phibins);  //!

    //Truth based plots

    allHists.push_back(h_d0);        
    allHists.push_back(h_z0sinT);    
    allHists.push_back(h_phi);       
    allHists.push_back(h_theta);     
    allHists.push_back(h_qoverp);    
  
    allHists.push_back(h_ptVsPt);    
  
    //Reco Based Plots

    allHists.push_back( h_Chi2DoF  ); 
    allHists.push_back( h_Chi2Prob ); 

    allHists.push_back(h_d0PV);    
    allHists.push_back(h_z0sinTPV);        


    allHists.push_back(h_d0vsPt);    
    allHists.push_back(h_z0sinTvsPt);
    allHists.push_back(h_d0vsEta);   
    allHists.push_back(h_z0sinTvsEta);


    allHists.push_back(h_nPixel);   		      //Inner Tracker Hits plots
    allHists.push_back(h_nBL);      		    
    allHists.push_back(h_nSCT);     		    
    allHists.push_back(h_nTRT);     		    


    allProfiles.push_back( p_SiHitsvsPt);   
    allProfiles.push_back( p_SiHitsvsEta);  
    allProfiles.push_back( p_PixHitsvsPt);  
    allProfiles.push_back( p_PixHitsvsEta);   
    allProfiles.push_back( p_SCTHitsvsPt);  
    allProfiles.push_back( p_SCTHitsvsEta);   
    allProfiles.push_back( p_TRTHitsvsPt);     
    allProfiles.push_back( p_TRTHitsvsEta);    


    allProfiles.push_back(p_nPixelvsEtaPhi); 
    allProfiles.push_back(p_nPixelvsEtaPt);  
    allProfiles.push_back(p_nSCTvsEtaPhi);   
    allProfiles.push_back(p_nSCTvsEtaPt);    
    allProfiles.push_back(p_nBLvsEtaPhi);     
    allProfiles.push_back(p_nPixelHolesvsEtaPhi);  
    allProfiles.push_back(p_nPixelDeadvsEtaPhi);   
    allProfiles.push_back(p_nSCTHolesvsEtaPhi);  
    allProfiles.push_back(p_nSCTDeadvsEtaPhi);   

    for( auto hist: allHists){		  
		  hist->Sumw2();
    }
	
	}
  
  ~TrackQualityHists()
  {
    for( auto hist: allHists){
      delete hist;
    }
    for( auto hist: allProfiles){
      delete hist;
    }
    
  }

  /** Fill the histograms using the reconstructed information*/ 
  void FillTruth( const xAOD::TrackParticle * track, const xAOD::TruthParticle * truth )
  {
   
    if(truth)
      h_ptVsPt->Fill(track->pt()*1e-3, truth->pt()*1e-3); 
   
    if( truth && truth->isAvailable<float>("d0")  ){
      h_d0->Fill( track->d0() - truth->auxdataConst<float>("d0") );
      h_z0sinT->Fill( track->z0() * sin( track->theta() ) - truth->auxdataConst<float>("z0st") );
      h_phi->Fill( (track->phi0() - truth->auxdataConst<float>("phi")) *1e3 );
      h_theta->Fill( (track->theta() - truth->auxdataConst<float>("theta")) *1e3 );
      h_qoverp->Fill( track->qOverP() / truth->auxdataConst<float>("qOverP") - 1. );
    } else {
      h_d0->Fill( track->d0() );
      h_z0sinT->Fill( track->z0() );
      h_phi->Fill( track->phi0() );
      h_theta->Fill( track->theta() );
      h_qoverp->Fill( track->qOverP() );
    }
    
  }

  /** Fill the histograms using the reconstructed information*/ 
  void Fill( const xAOD::TrackParticle * track )
  {
  
    h_Chi2DoF->Fill(track->chiSquared() / track->numberDoF());
    h_Chi2Prob->Fill(TMath::Prob(track->chiSquared(),track->numberDoF())); 
   
    double phi = track->phi();
    double eta = track->eta();
    double pt =  track->pt() *1e-3;

    uint8_t nPixel=0;
    uint8_t nPixelHoles=0;
    uint8_t nPixelDead=0;
    uint8_t nBL=0;
    uint8_t nSCT=0;
    uint8_t nSCTHoles=0;
    uint8_t nSCTDead=0;
    uint8_t nTRT=0;

    track->summaryValue( nPixel, xAOD::numberOfPixelHits );
    track->summaryValue( nPixelHoles, xAOD::numberOfPixelHoles );
    track->summaryValue( nPixelDead, xAOD::numberOfPixelDeadSensors  );
    track->summaryValue( nBL, xAOD::numberOfBLayerHits ); 
    track->summaryValue( nSCT, xAOD::numberOfSCTHits ); 
    track->summaryValue( nSCTHoles, xAOD::numberOfSCTHoles );
    track->summaryValue( nSCTDead, xAOD::numberOfSCTDeadSensors  );
    track->summaryValue( nTRT, xAOD::numberOfTRTHits ); 
 
    h_nPixel->Fill(nPixel);   		     
    h_nBL->Fill(nBL);      		    
    h_nSCT->Fill(nSCT);     		    
    h_nTRT->Fill(nTRT);     		    


    p_SiHitsvsPt->Fill(pt, nPixel +nSCT);   
    p_SiHitsvsEta->Fill(eta, nPixel +nSCT);  
    p_PixHitsvsPt->Fill(pt, nPixel);  
    p_PixHitsvsEta->Fill(eta, nPixel );   
    p_SCTHitsvsPt->Fill(pt, nSCT);  
    p_SCTHitsvsEta->Fill(eta, nSCT);   
    p_TRTHitsvsPt->Fill(pt, nTRT);     
    p_TRTHitsvsEta->Fill(eta, nTRT);    


    p_nPixelvsEtaPhi->Fill(eta,phi,nPixel); 
    p_nPixelvsEtaPt->Fill(eta,pt,nPixel);  
    p_nSCTvsEtaPhi->Fill(eta,phi,nSCT);   
    p_nSCTvsEtaPt->Fill(eta,pt,nSCT);    
    p_nBLvsEtaPhi->Fill(eta,phi,nBL);     
    p_nPixelHolesvsEtaPhi->Fill(eta,phi,nPixelHoles);
    p_nPixelDeadvsEtaPhi->Fill(eta,phi,nPixelDead);   
    p_nSCTHolesvsEtaPhi->Fill(eta,phi,nSCTHoles);
    p_nSCTDeadvsEtaPhi->Fill(eta,phi,nSCTDead);    
    
  }

  /** Fill the histograms using the reconstructed information*/ 
  void FillIP( const xAOD::TrackParticle * track, const xAOD::Vertex * vx )
  {
    double eta = track->eta();
    double pt =  track->pt() *1e-3;
 
 
    double d0 = track->d0();
    double z0 = track->z0();
    if(vx){
      z0 += track->vz() - vx->z(); 
      z0 *= sin(track->theta() );
    } 
    
    //Reco Based Plots

    h_d0PV->Fill(d0);
    h_z0sinTPV->Fill(z0);
    h_d0vsPt->Fill(d0, pt);
    h_z0sinTvsPt->Fill(z0, pt);
    h_d0vsEta->Fill(d0, eta);
    h_z0sinTvsEta->Fill(z0,eta);    
  }


  /** Add histograms to the output file */
  void Book( EL::Worker* wk )
  {
    for( auto hist : allHists){
      wk->addOutput( hist );
    }
    for( auto hist : allProfiles){
      wk->addOutput( hist );
    }
  }


};

#endif 
