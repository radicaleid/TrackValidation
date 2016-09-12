#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TrackValidation/ElectronTrackEfficiencyAnalysis.h>
#include <TrackValidation/TrackTruthHelper.h>


// EDM includes: - if move to header file will not compile?
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODRootAccess/TStore.h"
#include "xAODCore/ShallowCopy.h"


using namespace TrackTruthHelpers;


// this is needed to distribute the algorithm to the workers
ClassImp(ElectronTrackEfficiencyAnalysis)



ElectronTrackEfficiencyAnalysis :: ElectronTrackEfficiencyAnalysis ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  m_eventCounter = 0;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD ();

  // let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init( "ElectronTrackEfficiencyAnalysis" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  double etaRange[3]   = {100,-4.0,4.0}; 
  double ptRange[3]    = {1000,0,500}; 
  double bremRange[3]  = {1000,0,600.01};
  double elossRange[3] = {1000,0,1.005}; 
   


  m_TruevsEloss  = new TH1D("TruevsEloss",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TruevsEloss150  = new TH1D("TruevsEloss150",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TruevsPt     = new TH1D("TruevsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TruevsEta    = new TH1D("TruevsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);
  m_TruevsBremR  = new TH1D("TruevsBremR","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TrackvsEloss  = new TH1D("TrackvsEloss",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackvsEloss150  = new TH1D("TrackvsEloss150",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackvsPt     = new TH1D("TrackvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrackvsEta    = new TH1D("TrackvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);
  m_TrackvsBremR  = new TH1D("TrackvsBremR","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);


  m_DupTrackvsEloss  = new TH1D("DupTrackvsEloss",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_DupTrackvsEloss150  = new TH1D("DupTrackvsEloss150",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_DupTrackvsPt     = new TH1D("DupTrackvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_DupTrackvsEta    = new TH1D("DupTrackvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);
  m_DupTrackvsBremR  = new TH1D("DupTrackvsBremR","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);


  m_TrueRvsEloss  = new TH1D("TrueRvsEloss",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrueRvsEloss150  = new TH1D("TrueRvsEloss150",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrueRvsPt     = new TH1D("TrueRvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrueRvsEta    = new TH1D("TrueRvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);
  m_TrueRvsBremR  = new TH1D("TrueRvsBremR","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TrackRvsEloss  = new TH1D("TrackRvsEloss",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackRvsEloss150  = new TH1D("TrackRvsEloss150",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackRvsPt     = new TH1D("TrackRvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrackRvsEta    = new TH1D("TrackRvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);
  m_TrackRvsBremR  = new TH1D("TrackRvsBremR","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TruevsElossEta  = new TH1D("TruevsElossEta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TruevsEloss150Eta  = new TH1D("TruevsEloss150Eta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TruevsPtEta     = new TH1D("TruevsPtEta","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TruevsBremREta  = new TH1D("TruevsBremREta","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TrackvsElossEta  = new TH1D("TrackvsElossEta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackvsEloss150Eta  = new TH1D("TrackvsEloss150Eta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackvsPtEta     = new TH1D("TrackvsPtEta","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrackvsBremREta  = new TH1D("TrackvsBremREta","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TrueRvsElossEta  = new TH1D("TrueRvsElossEta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrueRvsEloss150Eta  = new TH1D("TrueRvsEloss150Eta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrueRvsPtEta     = new TH1D("TrueRvsPtEta","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrueRvsBremREta  = new TH1D("TrueRvsBremREta","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);

  m_TrackRvsElossEta  = new TH1D("TrackRvsElossEta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackRvsEloss150Eta  = new TH1D("TrackRvsEloss150Eta",";1 - f_{brem}; Electrons;",elossRange[0],elossRange[1],elossRange[2]);
  m_TrackRvsPtEta     = new TH1D("TrackRvsPtEta","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);
  m_TrackRvsBremREta  = new TH1D("TrackRvsBremREta","; Brem Radius [mm]; Electrons;",bremRange[0],bremRange[1],bremRange[2]);



  
  m_SiHitsvsPt  = new TProfile( "SiHitsvsPt", ";p_{T} [GeV]; <Si Hits>;",   ptRange[0],ptRange[1],ptRange[2],  0,  20 );
  m_SiHitsvsEta = new TProfile( "SiHitsvsEta", ";#eta; <Si Hits>;",   etaRange[0],etaRange[1],etaRange[2],  0,  20 );  
  m_PixHitsvsPt = new TProfile( "PixHitsvsPt", ";p_{T} [GeV]; <Pixel Hits>;",   ptRange[0],ptRange[1],ptRange[2],  0,  20 );;  
  m_PixHitsvsEta = new TProfile( "PixHitsvsEta", ";#eta; <Pixel Hits>;",   etaRange[0],etaRange[1],etaRange[2],  0,  20 );  ;   

  m_TRTExtensionsvsPt = new TH1D("TRTExtensionsvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);     
  m_TRTExtensionsvsEta = new TH1D("TRTExtensionsvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);


  
  m_SiHitsRvsPt  = new TProfile( "SiHitsRvsPt", ";p_{T} [GeV]; <Si Hits>;",   ptRange[0],ptRange[1],ptRange[2],  0,  20 );
  m_SiHitsRvsEta = new TProfile( "SiHitsRvsEta", ";#eta; <Si Hits>;",   etaRange[0],etaRange[1],etaRange[2],  0,  20 );  
  m_PixHitsRvsPt = new TProfile( "PixHitsRvsPt", ";p_{T} [GeV]; <Pixel Hits>;",   ptRange[0],ptRange[1],ptRange[2],  0,  20 );;  
  m_PixHitsRvsEta = new TProfile( "PixHitsRvsEta", ";#eta; <Pixel Hits>;",   etaRange[0],etaRange[1],etaRange[2],  0,  20 );  ;   

  m_TRTExtensionsRvsPt = new TH1D("TRTExtensionsRvsPt","; p_{T} [GeV]; Electrons;",ptRange[0],ptRange[1],ptRange[2]);     
  m_TRTExtensionsRvsEta = new TH1D("TRTExtensionsRvsEta","; #eta; Electrons;",etaRange[0],etaRange[1],etaRange[2]);


  h_d0     =  new TH1D( "d0", "; d_{0} (rec-true) [mm]; # Tracks", 500, -2.5, 2.5 );
  h_z0     =  new TH1D( "z0", "; z_{0} (rec-true) [mm]; # Tracks", 500, -5,5 );
  h_phi    =  new TH1D( "phi", "; #phi_{0} (rec-true) [mrad]; # Tracks", 500, -20, 20 );
  h_theta  =  new TH1D( "theta", "; #theta_{0} (rec-true) [mrad]; # Tracks", 500, -10, 10  );
  h_qoverp =  new TH1D( "qoverp", "; q/p  (rec-true)/true; # Tracks", 500, -1, 1 );

  h_Pulld0     =  new TH1D( "Pulld0", "; Pull d_{0}; # Tracks", 500, -10,10 );
  h_Pullz0     =  new TH1D( "Pullz0", "; Pull z_{0}; # Tracks", 500, -10,10 );
  h_Pullphi    =  new TH1D( "Pullphi", "; Pull #phi_{0}; # Tracks", 500, -10, 10 );
  h_Pulltheta  =  new TH1D( "Pulltheta", "; Pull #theta_{0}; # Tracks", 500, -10, 10  );
  h_Pullqoverp =  new TH1D( "Pullqoverp", "; Pull q/p; # Tracks", 500, -10, 10 );

  h_chi2ndf  =  new TH1D( "chi2ndf", "; #chi^{2}/nDoF; # Tracks", 500, 0, 10 );


  wk()->addOutput ( m_TruevsEloss ); 
  wk()->addOutput ( m_TruevsEloss150 ); 
  wk()->addOutput ( m_TruevsPt );    
  wk()->addOutput ( m_TruevsEta );   
  wk()->addOutput ( m_TruevsBremR ); 
 
  wk()->addOutput ( m_TrackvsEloss ); 
  wk()->addOutput ( m_TrackvsEloss150 ); 
  wk()->addOutput ( m_TrackvsPt );    
  wk()->addOutput ( m_TrackvsEta );   
  wk()->addOutput ( m_TrackvsBremR ); 

  wk()->addOutput ( m_DupTrackvsEloss ); 
  wk()->addOutput ( m_DupTrackvsEloss150 ); 
  wk()->addOutput ( m_DupTrackvsPt );    
  wk()->addOutput ( m_DupTrackvsEta );   
  wk()->addOutput ( m_DupTrackvsBremR ); 


  wk()->addOutput ( m_TrueRvsEloss ); 
  wk()->addOutput ( m_TrueRvsEloss150 ); 
  wk()->addOutput ( m_TrueRvsPt );    
  wk()->addOutput ( m_TrueRvsEta );   
  wk()->addOutput ( m_TrueRvsBremR ); 
 
  wk()->addOutput ( m_TrackRvsEloss ); 
  wk()->addOutput ( m_TrackRvsEloss150 ); 
  wk()->addOutput ( m_TrackRvsPt );    
  wk()->addOutput ( m_TrackRvsEta );   
  wk()->addOutput ( m_TrackRvsBremR ); 



  wk()->addOutput ( m_TruevsElossEta ); 
  wk()->addOutput ( m_TruevsEloss150Eta ); 
  wk()->addOutput ( m_TruevsPtEta );    
  wk()->addOutput ( m_TruevsBremREta ); 
 
  wk()->addOutput ( m_TrackvsElossEta ); 
  wk()->addOutput ( m_TrackvsEloss150Eta ); 
  wk()->addOutput ( m_TrackvsPtEta );    
  wk()->addOutput ( m_TrackvsBremREta ); 


  wk()->addOutput ( m_TrueRvsElossEta ); 
  wk()->addOutput ( m_TrueRvsEloss150Eta ); 
  wk()->addOutput ( m_TrueRvsPtEta );    
  wk()->addOutput ( m_TrueRvsBremREta ); 
 
  wk()->addOutput ( m_TrackRvsElossEta ); 
  wk()->addOutput ( m_TrackRvsEloss150Eta ); 
  wk()->addOutput ( m_TrackRvsPtEta );    
  wk()->addOutput ( m_TrackRvsBremREta ); 
  
  wk()->addOutput ( m_SiHitsvsPt );   
  wk()->addOutput ( m_SiHitsvsEta );  
  wk()->addOutput ( m_PixHitsvsPt );  
  wk()->addOutput ( m_PixHitsvsEta );   
  wk()->addOutput ( m_TRTExtensionsvsPt );     
  wk()->addOutput ( m_TRTExtensionsvsEta );    


  wk()->addOutput ( m_SiHitsRvsPt );   
  wk()->addOutput ( m_SiHitsRvsEta );  
  wk()->addOutput ( m_PixHitsRvsPt );  
  wk()->addOutput ( m_PixHitsRvsEta );   
  wk()->addOutput ( m_TRTExtensionsRvsPt );     
  wk()->addOutput ( m_TRTExtensionsRvsEta );    



  wk()->addOutput (h_d0);        //!
  wk()->addOutput (h_z0);        //!
  wk()->addOutput (h_phi);       //!
  wk()->addOutput (h_theta);     //!
  wk()->addOutput (h_qoverp);    //!

  wk()->addOutput (h_Pulld0);        //!
  wk()->addOutput (h_Pullz0);        //!
  wk()->addOutput (h_Pullphi);       //!
  wk()->addOutput (h_Pulltheta);     //!
  wk()->addOutput (h_Pullqoverp);    //!
  
  
  wk()->addOutput (h_chi2ndf); //!

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  
  m_event = wk()->xaodEvent();
  
  // as a check, let's see the number of events in our xAOD
  Info("initialize()", "Number of events = %lli", m_event->getEntries() ); // print long long int



  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  
  // print every 100 events, so we know where we are:
  if( (m_eventCounter % 100) ==0 ) Info("execute()", "Event number = %i", m_eventCounter );
  m_eventCounter++;

  //----------------------------
  // Event information
  //--------------------------- 
  const xAOD::EventInfo* eventInfo = 0;
  if( ! m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error("execute()", "Failed to retrieve event info collection. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  // check if the event is data or MC
  // (many tools are applied either to data or MC)
  bool isMC = false;
  // check if the event is MC
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; // can do something with this later
  }   

  if (!isMC)   
    return EL::StatusCode::SUCCESS;

  //---------------------
  // TRUTH PARTICLES
  //---------------------
  // get track container of interest
  const xAOD::TruthParticleContainer* truthParticles = 0;
  if ( !m_event->retrieve( truthParticles, "TruthParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Truth Particle container. Exiting." );
    return EL::StatusCode::FAILURE;
  }
 
  
  const xAOD::TruthParticle* primaryElectron = 0;
  for(auto truthPart : *truthParticles ){    
    if( isPrimary( truthPart ) ) {
      primaryElectron = truthPart;
      //std::cout << "Primary   " << truthPart->pdgId() << " " << truthPart->barcode() << std::endl;
    } else{ 
      //std::cout << "Secondary " << truthPart->pdgId() << " " << truthPart->barcode() << std::endl;
    }
  }
  
  
  if(!primaryElectron) 
    return EL::StatusCode::SUCCESS;

  double energyLostR600    = 0;
  double energyLostR150    = 0;
  double radius20Loss      = 0;
  bool   isReconstructable = 0;

  getBremInfo( primaryElectron, energyLostR600, energyLostR150, radius20Loss, isReconstructable);
 
  // get track container of interest
  const xAOD::TrackParticleContainer* recoTracks = 0;
  if ( !m_event->retrieve( recoTracks, "InDetTrackParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
    return EL::StatusCode::FAILURE;
  }
    
  double etaRange = 0.6;
  
  m_TruevsEloss->Fill(energyLostR600);
  m_TruevsEloss150->Fill(energyLostR150);
  m_TruevsPt->Fill(primaryElectron->pt()*1e-3);
  m_TruevsEta->Fill(primaryElectron->eta());   
  m_TruevsBremR->Fill(radius20Loss); 
  if(fabs(primaryElectron->eta())<etaRange){
    m_TruevsElossEta->Fill(energyLostR600);
    m_TruevsEloss150Eta->Fill(energyLostR150);
    m_TruevsPtEta->Fill(primaryElectron->pt()*1e-3);
    m_TruevsBremREta->Fill(radius20Loss); 
  }
  
  if(isReconstructable){
    m_TrueRvsEloss->Fill(energyLostR600);
    m_TrueRvsEloss150->Fill(energyLostR150);
    m_TrueRvsPt->Fill(primaryElectron->pt()*1e-3);
    m_TrueRvsEta->Fill(primaryElectron->eta());   
    m_TrueRvsBremR->Fill(radius20Loss);
    if(fabs(primaryElectron->eta())<etaRange){
      m_TrueRvsElossEta->Fill(energyLostR600);
      m_TrueRvsEloss150Eta->Fill(energyLostR150);
      m_TrueRvsPtEta->Fill(primaryElectron->pt()*1e-3);
      m_TrueRvsBremREta->Fill(radius20Loss);
    } 
  }  

  bool alreadyMatched = false;
  
  // loop over the tracks in the container
  for( auto recoTrk : *recoTracks) {
    //Info("execute()", "  original reco track pT = %.2f GeV", ((*recoTrk_itr)->pt()*1e-3 * 0.001));
 
    if( isFake( recoTrk ) ){  
      continue;
    } 
    
    const xAOD::TruthParticle* truth = truthParticle( recoTrk  );  
    if(!truth ||  truth != primaryElectron) {
      continue;
    }
    
    if( isPrimary( truth ) ) {
          
      if(alreadyMatched){ 
        m_DupTrackvsEloss->Fill(energyLostR600);
        m_DupTrackvsEloss150->Fill(energyLostR150);
        m_DupTrackvsPt->Fill(primaryElectron->pt()*1e-3);
        m_DupTrackvsEta->Fill(primaryElectron->eta());   
        m_DupTrackvsBremR->Fill(radius20Loss); 
        continue;      
      } 

   


      if( truth && truth->isAvailable<float>("d0")  ){
        double charge = truth->charge();
        
        h_d0->Fill( (recoTrk->d0() - truth->auxdataConst<float>("d0"))*charge );
        h_z0->Fill( recoTrk->z0()  - truth->auxdataConst<float>("z0") );
        h_phi->Fill( (recoTrk->phi0() - truth->auxdataConst<float>("phi")) *1e3 *charge  );
        h_theta->Fill( (recoTrk->theta() - truth->auxdataConst<float>("theta")) *1e3 );
        h_qoverp->Fill( recoTrk->qOverP() / truth->auxdataConst<float>("qOverP") - 1.  );
        
        
        const xAOD::ParametersCovMatrix_t cov = recoTrk->definingParametersCovMatrix();  
        
        h_Pulld0->Fill( (recoTrk->d0() - truth->auxdataConst<float>("d0"))*charge/sqrt(cov(0,0)) );
        h_Pullz0->Fill( (recoTrk->z0()  - truth->auxdataConst<float>("z0"))/sqrt(cov(1,1)) );
        h_Pullphi->Fill( (recoTrk->phi0() - truth->auxdataConst<float>("phi"))/sqrt(cov(2,2))  );
        h_Pulltheta->Fill( (recoTrk->theta() - truth->auxdataConst<float>("theta"))/sqrt(cov(3,3)) );
        h_Pullqoverp->Fill( (recoTrk->qOverP() - truth->auxdataConst<float>("qOverP") )*charge/sqrt(cov(4,4)) );

        
      }
      
      h_chi2ndf->Fill(recoTrk->chiSquared() / recoTrk->numberDoF());


      uint8_t getIntPixel=0;
      uint8_t getIntSCT=0;
      uint8_t getIntTRT=0;

      recoTrk->summaryValue( getIntPixel, xAOD::numberOfPixelHits );
      recoTrk->summaryValue( getIntSCT, xAOD::numberOfSCTHits ); 
      recoTrk->summaryValue( getIntTRT, xAOD::numberOfTRTHits ); 
       
      m_TrackvsEloss->Fill(energyLostR600);
      m_TrackvsEloss150->Fill(energyLostR150);
      m_TrackvsPt->Fill(primaryElectron->pt()*1e-3);
      m_TrackvsEta->Fill(primaryElectron->eta());   
      m_TrackvsBremR->Fill(radius20Loss); 
      if(fabs(primaryElectron->eta())<etaRange){
        m_TrackvsElossEta->Fill(energyLostR600);
        m_TrackvsEloss150Eta->Fill(energyLostR150);
        m_TrackvsPtEta->Fill(primaryElectron->pt()*1e-3);
        m_TrackvsBremREta->Fill(radius20Loss); 

        if(getIntTRT>0){
          m_TRTExtensionsvsPt->Fill(primaryElectron->pt()*1e-3); 
        }   
      }
      
      m_SiHitsvsPt->Fill( primaryElectron->pt()*1e-3, getIntPixel+getIntSCT  );   
      m_SiHitsvsEta->Fill( primaryElectron->eta(), getIntPixel+getIntSCT  );
      m_PixHitsvsPt->Fill( primaryElectron->pt()*1e-3, getIntPixel  );   
      m_PixHitsvsEta->Fill( primaryElectron->eta(), getIntPixel  );
      
      if(getIntTRT>0){
        m_TRTExtensionsvsEta->Fill(primaryElectron->eta());     
      }
      
      
      if(isReconstructable){
        m_TrackRvsEloss->Fill(energyLostR600);
        m_TrackRvsEloss150->Fill(energyLostR150);
        m_TrackRvsPt->Fill(primaryElectron->pt()*1e-3);
        m_TrackRvsEta->Fill(primaryElectron->eta());   
        m_TrackRvsBremR->Fill(radius20Loss); 
        if(fabs(primaryElectron->eta())<etaRange){
          m_TrackRvsElossEta->Fill(energyLostR600);
          m_TrackRvsEloss150Eta->Fill(energyLostR150);
          m_TrackRvsPtEta->Fill(primaryElectron->pt()*1e-3);
          m_TrackRvsBremREta->Fill(radius20Loss); 

          if(getIntTRT>0){
            m_TRTExtensionsRvsPt->Fill(primaryElectron->pt()*1e-3); 
          }   

        }
  
        m_SiHitsRvsPt->Fill( primaryElectron->pt()*1e-3, getIntPixel+getIntSCT  );   
        m_SiHitsRvsEta->Fill( primaryElectron->eta(), getIntPixel+getIntSCT  );
        m_PixHitsRvsPt->Fill( primaryElectron->pt()*1e-3, getIntPixel  );   
        m_PixHitsRvsEta->Fill( primaryElectron->eta(), getIntPixel  );
        
        if(getIntTRT>0 ){
          m_TRTExtensionsRvsEta->Fill(primaryElectron->eta());     
        }
      }    
      alreadyMatched = true;
    } 
  } // end for loop over reconstructed tracks


  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ElectronTrackEfficiencyAnalysis :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  return EL::StatusCode::SUCCESS;
}



void ElectronTrackEfficiencyAnalysis::getBremInfo( const xAOD::TruthParticle* truth, double& energyLostR600, double& energyLostR150, double& radius20Loss, bool& isReconstructable) const
{

  double energyFraction = 1; 
  double energyFraction600 = 1; 
  double radiusFor50eloss = 600;
    
  double radialCut = 150; //mm
  double radialCutBig = 600; //mm
  double endVertexR = 0;
  double currentVertexR = 0;

  double tempenergyFraction = 1; 
  
  double minPt = truth->pt()*0.01;
  
  isReconstructable =true;
  

  if( truth->barcode() > 200000 || truth->status() != 1 || abs(truth->pdgId()) != 11 ){
    Warning("getBremInfo(...)","Particle passed was not a prompt electron");     
    std::cout <<  truth->barcode()  << ", " <<  truth->status() << ", " << truth->pdgId() << std::endl;
    return;
  }
  
  const xAOD::TruthParticle* currentTruth = truth;
  
  if( currentTruth->hasDecayVtx() ) {
    endVertexR =  currentTruth->decayVtx()->perp();
  } else if( currentTruth->pt() > 500 ) { 
    endVertexR =1000;
  } else {
    endVertexR = 0;
    energyFraction = 0;
    Info("getBremInfo(...)","Truth has no decay vertex and pt is low");
    return;
  }
  
  int bremCount(0);
  
  while( currentTruth->decayVtx() &&  endVertexR < radialCutBig ){ 
     
    int electronIndex(-1);
    int photonIndex(-1);
    for( unsigned int child(0); child < currentTruth->decayVtx()->nOutgoingParticles(); ++child ){
      if( abs(currentTruth->decayVtx()->outgoingParticle(child)->pdgId() ) == 11 ) electronIndex = (int)child;
      if( abs(currentTruth->decayVtx()->outgoingParticle(child)->pdgId() ) == 22 ) photonIndex = (int)child; 
    }
  
    if (electronIndex != -1){
      // Info("getBremInfo(...)","Electron Brems " );
      ++bremCount;
      
      currentTruth = currentTruth->decayVtx()->outgoingParticle(electronIndex);
    
      if( currentTruth->hasDecayVtx() ) {
        endVertexR = currentTruth->decayVtx()->perp();
      } else {
        endVertexR = 0;
      }
      if( currentTruth->hasProdVtx() ){
        currentVertexR = currentTruth->prodVtx()->perp(); 
      }            
      tempenergyFraction = currentTruth->e() / truth->e();

      //std::cout << "  " << currentVertexR << " "<<  tempenergyFraction<<  " " << endVertexR << std::endl;

      if( endVertexR == 0 && currentTruth->pt() >= minPt){
        endVertexR = 1000;
      }
    
      if(currentVertexR<radiusFor50eloss && tempenergyFraction <= 0.80 )
        radiusFor50eloss   = currentVertexR;
      if(currentVertexR <= radialCut )
        energyFraction = tempenergyFraction;
      if(currentVertexR <= radialCutBig )
        energyFraction600 = tempenergyFraction;   

    } else if (photonIndex != -1){
      Info("getBremInfo(...)","Electron Brems -- no more electrons");

      //Electrons energy is too low
      endVertexR = currentTruth->decayVtx()->perp();
      if(endVertexR == 0  && currentTruth->pt() >= minPt )
        endVertexR =1000;       
      break;
    }else{
      Info("getBremInfo(...)","Electron Brems -- no more expected parts");
      //NO futher Children!
      break;
    }
  }// End while loop
 
  //std::cout << " end vertex " <<  endVertexR  << " " << (*mc_gen_eta)[truth]  <<std::endl;                          
  if (  endVertexR < radialCutBig ) isReconstructable  =  false;                  
    
    
  energyLostR600 = energyFraction600;
  energyLostR150 = energyFraction;
  radius20Loss = radiusFor50eloss;
   
} 




