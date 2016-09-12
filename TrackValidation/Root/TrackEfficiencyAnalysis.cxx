#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TrackValidation/TrackEfficiencyAnalysis.h>
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

#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"


using namespace TrackTruthHelpers;

// this is needed to distribute the algorithm to the workers
ClassImp(TrackEfficiencyAnalysis)



TrackEfficiencyAnalysis :: TrackEfficiencyAnalysis ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  m_eventCounter = 0;
}



EL::StatusCode TrackEfficiencyAnalysis :: setupJob (EL::Job& job)
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
  xAOD::Init( "TrackEfficiencyAnalysis" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackEfficiencyAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.


  m_PrimaryTracks    = new TrackEfficiencyHists("ITK/Tracks_Primary/Primary");  
  m_SecondaryTracks  = new TrackEfficiencyHists("ITK/Tracks_Secondary/Secondary");  
  m_PrimaryTruth     = new TrackEfficiencyHists("ITK/Tracks_Truth_Primary/Truth_Primary");     
  m_SecondaryTruth   = new TrackEfficiencyHists("ITK/Tracks_Truth_Secondary/Truth_Secondary"); 
  m_Truth            = new TrackEfficiencyHists("ITK/Tracks_Truth/Truth");   

  m_DuplicatePrimaryTracks   =  new TrackEfficiencyHists("ITK/Tracks_DuplicatePrimary/DuplicatePrimary");
  m_DuplicateSecondaryTracks =  new TrackEfficiencyHists("ITK/Tracks_DuplicateSecondary/DuplicateSecondary");
	
  m_AllTracks        = new TrackEfficiencyHists("ITK/Tracks_All/All");
  m_FakeTracks       = new TrackEfficiencyHists("ITK/Tracks_Fake/Fake");       

  m_QualAllTracks = new TrackQualityHists("ITK/Tracks_Quality_AllTracks/AllTracks");
  m_QualSelTracks = new TrackQualityHists("ITK/Tracks_Quality_SelectedTracks/TruthTracks");
  m_QualSelFakeTracks = new TrackQualityHists("ITK/Tracks_Quality_FakeTracks/FakeTracks");


  m_PrimaryTracks->Book( wk() );
  m_SecondaryTracks->Book( wk() );
  m_PrimaryTruth->Book( wk() );
  m_SecondaryTruth->Book( wk() );
  m_Truth->Book( wk() );

  m_DuplicatePrimaryTracks->Book( wk() );
  m_DuplicateSecondaryTracks->Book( wk() );

  m_AllTracks->Book( wk() );
  m_FakeTracks->Book( wk() );

  m_QualAllTracks->Book( wk() );
  m_QualSelTracks->Book( wk() );
  m_QualSelFakeTracks->Book( wk() );
 
  m_SelectedPrimaryTracks    = new TrackEfficiencyHists("ITK/Tracks_Selected_Primary/Primary");
  m_SelectedSecondaryTracks  = new TrackEfficiencyHists("ITK/Tracks_Selected_Secondary/Secondary");
  m_SelectedPrimaryFakeTracks    = new TrackEfficiencyHists("ITK/Tracks_Selected_Primary_Fake/Fake");
  m_SelectedSecondaryFakeTracks  = new TrackEfficiencyHists("ITK/Tracks_Selected_Secondary_Fake/Fake");
  m_SelectedDuplicatePrimaryTracks   =  new TrackEfficiencyHists("ITK/Tracks_Selected_DuplicatePrimary/DuplicatePrimary");
  m_SelectedDuplicateSecondaryTracks =  new TrackEfficiencyHists("ITK/Tracks_Selected_DuplicateSecondary/DuplicateSecondary");
  m_SelectedAllTracks        = new TrackEfficiencyHists("ITK/Tracks_Selected_All/All");
  m_SelectedFakeTracks       = new TrackEfficiencyHists("ITK/Tracks_Selected_Fake/Fake");

  m_SelectedPrimaryTracks->Book( wk() );
  m_SelectedSecondaryTracks->Book( wk() );
  m_SelectedPrimaryFakeTracks->Book( wk() );
  m_SelectedSecondaryFakeTracks->Book( wk() );
  m_SelectedDuplicatePrimaryTracks->Book( wk() );
  m_SelectedDuplicateSecondaryTracks->Book( wk() );
  m_SelectedAllTracks->Book( wk() );
  m_SelectedFakeTracks->Book( wk() );

  debug_events = new TH1F("ITK/Debug/events", "Events", 10, 0, 10);
  debug_events->GetXaxis()->SetTitle("Passed track cuts"); 
  debug_tracks = new TH1F("ITK/Debug/tracks", "Tracks selected", 10, 0, 10);
  debug_tracks->GetXaxis()->SetTitle("Passed event cuts"); 
  wk()->addOutput (debug_events);
  wk()->addOutput (debug_tracks);


 // pixels
  myXY = new TH2F("ITK/Pixels/Pixels2D", "Pixel global XY", 500, 0, 500, 500, 0, 500);
  myXZ = new TH2F("ITK/Pixels/Pixels2D_XZ", "Pixel XZ", 6000, -3000, 3000,500, 0, 500);
  myXY->GetXaxis()->SetTitle("X global [mm]");
  myXY->GetYaxis()->SetTitle("Y global [mm]");
  myXZ->GetXaxis()->SetTitle("Z global [mm]");
  myXZ->GetYaxis()->SetTitle("X-Y global [mm]");
  //fileout = fopen ("clusters.txt", "w");

  myZ = new TH1F("ITK/Pixels/PixelsZ", "Pixel Z", 3000, -3000, 3000);
  myZ->GetXaxis()->SetTitle("Z global [mm]");
  myZ->GetYaxis()->SetTitle("Entries");

  myX = new TH1F("ITK/Pixels/PixelsX", "Pixel trans", 250, 0, 500);
  myX->GetXaxis()->SetTitle("X-Y global [mm]");
  myX->GetYaxis()->SetTitle("Entries");

  myTOT = new TH1F("ITK/Pixels/TOT", "TOT", 250, 0, 500);
  myTOT->GetXaxis()->SetTitle("TOT");
  myTOT->GetYaxis()->SetTitle("Entries");

  myLayer = new TH1F("ITK/Pixels/Layer", "Layer", 10, 0, 10);
  myLayer->GetXaxis()->SetTitle("Nr layer");
  myLayer->GetYaxis()->SetTitle("Entries");

  wk()->addOutput (myXY);
  wk()->addOutput (myXZ);
  wk()->addOutput (myZ);
  wk()->addOutput (myX);
  wk()->addOutput (myTOT);
  wk()->addOutput (myLayer);


  stXY = new TH2F("ITK/Strips/Strips2D", "Strip global XY", 500, 0, 1000, 600, 0, 1200);
  stXZ = new TH2F("ITK/Strips/Strips2D_XZ", "Strip XZ", 6000, -3000, 3000,600, 0, 1200);
  stXY->GetXaxis()->SetTitle("X global [mm]");
  stXY->GetYaxis()->SetTitle("Y global [mm]");
  stXZ->GetXaxis()->SetTitle("Z global [mm]");
  stXZ->GetYaxis()->SetTitle("X-Y global [mm]");

  stZ = new TH1F("ITK/Strips/StripsZ", "Strips Z", 3000, -3000, 3000);
  stZ->GetXaxis()->SetTitle("Z global [mm]");
  stZ->GetYaxis()->SetTitle("Entries");

  stX = new TH1F("ITK/Strips/StripsX", "Strips trans", 500, 0, 1000);
  stX->GetXaxis()->SetTitle("X-Y global [mm]");
  stX->GetYaxis()->SetTitle("Entries");

  stTOT = new TH1F("ITK/Strips/TOT", "TOT", 250, 0, 500);
  stTOT->GetXaxis()->SetTitle("TOT");
  stTOT->GetYaxis()->SetTitle("Entries");

  stLayer = new TH1F("ITK/Strips/Layer", "Layer", 10, 0, 10);
  stLayer->GetXaxis()->SetTitle("Nr layer");
  stLayer->GetYaxis()->SetTitle("Entries");

  wk()->addOutput (stXY);
  wk()->addOutput (stXZ);
  wk()->addOutput (stZ);
  wk()->addOutput (stX);
  wk()->addOutput (stTOT);
  wk()->addOutput (stLayer);


 // some basic reolution plots for checking
 res1gev = new TH1F("ITK/Resolution/Resolution_1_GeV", "Resolution 0.9-1.0 GeV (primary)", 1000, 0.5,1.5);
 res1gev->GetXaxis()->SetTitle("PTreco/PTtrue");
 res1gev->GetYaxis()->SetTitle("Entries");

 res10gev = new TH1F("ITK/Resolution/Resolution_10_GeV", "Resolution 9-11 GeV (primary)", 1000, 0.5,1.5);
 res10gev->GetXaxis()->SetTitle("PTreco/PTtrue");
 res10gev->GetYaxis()->SetTitle("Entries");

 res100gev = new TH1F("ITK/Resolution/Resolution_100_GeV", "Resolution 90-110 GeV (primary)", 1000, 0.5,1.5);
 res100gev->GetXaxis()->SetTitle("PTreco/PTtrue");
 res100gev->GetYaxis()->SetTitle("Entries");
 
 // for estimation of means
 mean1gev = new TH1F("ITK/Resolution/PT_truth_1_GeV", "pT truth 0.9-1.0 GeV (primary)", 1000, 0.0, 1.5*1000);
 mean1gev->GetXaxis()->SetTitle("PTtrue");
 mean1gev->GetYaxis()->SetTitle("Entries");

 mean10gev = new TH1F("ITK/Resolution/PT_truth_10_GeV", "pT truth  9-11 GeV (primary)", 400, 0.0,15*1000);
 mean10gev->GetXaxis()->SetTitle("PTtrue");
 mean10gev->GetYaxis()->SetTitle("Entries");

 mean100gev = new TH1F("ITK/Resolution/PT_truth_100_GeV", "pT truth 90-110 GeV (primary)", 400, 0.0,150*1000);
 mean100gev->GetXaxis()->SetTitle("PTtrue");
 mean100gev->GetYaxis()->SetTitle("Entries");

 wk()->addOutput (res1gev);
 wk()->addOutput (res10gev);
 wk()->addOutput (res100gev);
 wk()->addOutput (mean1gev);
 wk()->addOutput (mean10gev);
 wk()->addOutput (mean100gev);

//addition plot for duplicate

 barcodePrim = new TH2F("ITK/Tracks_Selected_DuplicatePrimary/DuplicatePrimary_barcode", "barcode of the asso. truth  (primary)", 200000, 0,200000,5,0,5);
 barcodePrim->GetXaxis()->SetTitle("barcode");
 barcodePrim->GetYaxis()->SetTitle("Vertex type");

 barcodeSec = new TH2F("ITK/Tracks_Selected_DuplicateSecondary/DuplicateSecondary_barcode", "barcode of the asso. truth  (secondary)", 1000000, 200000,1200000,5,0,5);
 barcodeSec->GetXaxis()->SetTitle("barcode");
 barcodeSec->GetYaxis()->SetTitle("Vertex type");
 
 wk()->addOutput (barcodePrim);
 wk()->addOutput (barcodeSec);

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode TrackEfficiencyAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackEfficiencyAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackEfficiencyAnalysis :: initialize ()
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



EL::StatusCode TrackEfficiencyAnalysis :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  
  // print every 100 events, so we know where we are:
  if( (m_eventCounter % 100) ==0 ) Info("execute()", "Event number = %i", m_eventCounter );
  m_eventCounter++;
  debug_events->Fill(1.0);

  // Reset barcode set
  m_PrimaryTracks->usedBarcodes.clear();
  m_SecondaryTracks->usedBarcodes.clear();
  m_SelectedPrimaryTracks->usedBarcodes.clear();
  m_SelectedSecondaryTracks->usedBarcodes.clear();
  m_SelectedPrimaryFakeTracks->usedBarcodes.clear();
  m_SelectedSecondaryFakeTracks->usedBarcodes.clear();
  m_SelectedAllTracks->usedBarcodes.clear();

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
  float BSX = eventInfo->beamPosX();
  float BSY = eventInfo->beamPosY();
  float BSZ = eventInfo->beamPosZ();



 // look at pixels
  const xAOD::TrackMeasurementValidationContainer* pixelClusters = 0;
  if ( !m_event->retrieve( pixelClusters, "PixelClusters" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Pixel Clusters. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  for( auto  cluster_itr = pixelClusters->begin(); cluster_itr != pixelClusters->end(); cluster_itr++)
  {
    float charge(0);
    if( (*cluster_itr)->isAvailable< float >( "charge" ) )   // Check is not required if you know it is there
      charge = (*cluster_itr)->auxdataConst< float >("charge");

    int TOT(0);
    if( (*cluster_itr)->isAvailable< int >( "ToT" ) )   // Check is not required if you know it is there
      TOT = (*cluster_itr)->auxdataConst< int >("ToT");

   int layer(0);
   if( (*cluster_itr)->isAvailable< int >( "layer" ) )   // Check is not required if you know it is there
      layer = (*cluster_itr)->auxdataConst< int >("layer");

   double xx= (*cluster_itr)->globalX();
   double yy=(*cluster_itr)->globalY();
   double zz=(*cluster_itr)->globalZ();
   double ppp=sqrt(xx*xx+yy*yy);

  myXY->Fill(xx, yy);
  myXZ->Fill(zz,ppp);
  myZ->Fill(zz);
  myX->Fill(ppp);
  myTOT->Fill((float)TOT);
  myLayer->Fill((float)layer);
}


//If you don't about the track properties at all you can loop over the the sct clusters
   const xAOD::TrackMeasurementValidationContainer* sctClusters = 0;
   if ( !m_event->retrieve( sctClusters, "SCT_Clusters" ).isSuccess() ){ // retrieve arguments: container type, container key
       Error("execute()", "Failed to retrieve SCT_Clusters. Exiting." );
            return EL::StatusCode::FAILURE;
               }

   for( auto  cluster_itr = sctClusters->begin(); cluster_itr != sctClusters->end(); cluster_itr++)
  {

    float charge(0);
    if( (*cluster_itr)->isAvailable< float >( "charge" ) )   // Check is not required if you know it is there
      charge = (*cluster_itr)->auxdataConst< float >("charge");

    int TOT(0);
    if( (*cluster_itr)->isAvailable< int >( "ToT" ) )   // Check is not required if you know it is there
      TOT = (*cluster_itr)->auxdataConst< int >("ToT");

   int layer(0);
   if( (*cluster_itr)->isAvailable< int >( "layer" ) )   // Check is not required if you know it is there
      layer = (*cluster_itr)->auxdataConst< int >("layer");

   double xx= (*cluster_itr)->globalX();
   double yy=(*cluster_itr)->globalY();
   double zz=(*cluster_itr)->globalZ();
   double ppp=sqrt(xx*xx+yy*yy);

   // Info("execute()",  "Pixel Cluster local position (%f, %f) with charge %f",  (*cluster_itr)->localX(),  
   // (*cluster_itr)->localY(), charge );  
      stXY->Fill(xx, yy);
      stXZ->Fill(zz,ppp);
      stZ->Fill(zz);
      stX->Fill(ppp);
      stTOT->Fill((float)TOT);
      stLayer->Fill((float)layer);
   //

   }



  const xAOD::VertexContainer* vertices = 0;

  if ( !m_event->retrieve( vertices, "PrimaryVertices" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
    return EL::StatusCode::FAILURE;
  }
  
  int nvtx = 0;
  const xAOD::Vertex* primaryVertex = 0;
  
  for( auto vertex: *vertices ) {      
    if( vertex->vertexType() == xAOD::VxType::PriVtx ) {
      primaryVertex = vertex;
      ++nvtx;
    } else if( vertex->vertexType() == xAOD::VxType::PileUp ) 
      ++nvtx;
  }


  // get track container of interest
  const xAOD::TrackParticleContainer* recoTracks = 0;
  if ( !m_event->retrieve( recoTracks, "InDetTrackParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
    return EL::StatusCode::FAILURE;
  }


  // loop over the tracks in the container
  for( auto track : *recoTracks ) {
    //Info("execute()", "  original reco track pT = %.2f GeV", (track->pt() * 0.001));

    // no any cuts applied
    m_AllTracks->Fill( track );
    m_QualAllTracks->Fill( track );
    m_QualAllTracks->FillIP( track, primaryVertex);
   


     // *************  apply track quality cuts  ************** 
 
    if( passTrackQualitySelection( track ))  { 
		
      m_SelectedAllTracks->Fill( track );
      if(isMC){
        if( isFake( track ) ){
          m_SelectedFakeTracks->Fill( track );
          m_QualSelFakeTracks->Fill( track );
          m_QualSelFakeTracks->FillIP(track, primaryVertex);
        } 


        const xAOD::TruthParticle* truth = truthParticle( track );  

        // use 0.5 for matching
        if (truth && getMatchingProbability(track)>0.5 ) {

        if( isPrimary( truth ) ) {
          m_QualSelTracks->Fill(track);
          m_QualSelTracks->FillTruth(track, truth);
          m_QualSelTracks->FillIP(track, primaryVertex);


          if ( m_SelectedPrimaryTracks->hasBeenUsed(truth) ){
            m_SelectedDuplicatePrimaryTracks->Fill(truth);
          if(m_eventCounter==1){
            typedef ElementLink< xAOD::VertexContainer > Link_t;
            static const char* NAME = "vertexLink";
            if( ! track->isAvailable< Link_t >( NAME ) ){barcodePrim->Fill(truth->barcode(),4);continue;}
            const Link_t& vtx = track->auxdata< Link_t >( NAME );
            if( ! vtx.isValid() ){barcodePrim->Fill(truth->barcode(),4); continue;}
            barcodePrim->Fill(truth->barcode(),(*vtx)->vertexType());
          }
          } else {

            // look at resolution
            double res=0;
            double pt_t=truth->pt();
            if (truth->pt()>0) res=track->pt() / pt_t; 
            if (pt_t>900 && pt_t<1100)     {res1gev->Fill(res); mean1gev->Fill(pt_t); } 
            if (pt_t>9000 && pt_t<11000)   {res10gev->Fill(res); mean10gev->Fill(pt_t); } 
            if (pt_t>90000 && pt_t<110000) {res100gev->Fill(res); mean100gev->Fill(pt_t); } 
            // look at efficiency
            m_SelectedPrimaryTracks->Fill(truth);
          }
       
       
        } else if( isSecondary( truth ) ){

          if (m_SelectedSecondaryTracks->hasBeenUsed(truth) ){ 
            m_SelectedDuplicateSecondaryTracks->Fill(truth);
          if(m_eventCounter==1){
            typedef ElementLink< xAOD::VertexContainer > Link_t;
            static const char* NAME = "vertexLink";
            if( ! track->isAvailable< Link_t >( NAME ) ){barcodeSec->Fill(truth->barcode(),4);continue;}
            const Link_t& vtx = track->auxdata< Link_t >( NAME );
            if( ! vtx.isValid() ){barcodeSec->Fill(truth->barcode(),4); continue;}
            barcodeSec->Fill(truth->barcode(),(*vtx)->vertexType());
          }
          } else {
            m_SelectedSecondaryTracks->Fill(truth);
          }
        }
        
      } // end matching
     } // end quality cut 
    } // end of truth matching 

 

    // continue with all tracks
    if(isMC){
      if( isFake( track ) ){
        m_FakeTracks->Fill( track );
      } 

      const xAOD::TruthParticle* truth = truthParticle( track );  

      if(truth && getMatchingProbability(track)>0.5) { 

      // carefull, all tracks, matching tracks may be already used!
      if( isPrimary( truth ) ) {
				if ( m_PrimaryTracks->hasBeenUsed(truth) ){
				  m_DuplicatePrimaryTracks->Fill(truth);
				} else {
                                   m_PrimaryTracks->Fill(truth);
				}

      } else if( isSecondary( truth ) ){
				if (m_SecondaryTracks->hasBeenUsed(truth) ){ 
                                    m_DuplicateSecondaryTracks->Fill(truth);
				} else {
					m_SecondaryTracks->Fill(truth);
				}
      }
    }
   } // end matching
  } // end for loop over reconstructed tracks


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

  for( auto truthPart: *truthParticles ) {
    if( isPrimary( truthPart ) ) {
      m_PrimaryTruth->Fill( truthPart );
      m_Truth->Fill(truthPart);
    } else if( isSecondary( truthPart ) ){
      m_SecondaryTruth->Fill( truthPart );
      m_Truth->Fill(truthPart);
    }
  }

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackEfficiencyAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TrackEfficiencyAnalysis :: finalize ()
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



EL::StatusCode TrackEfficiencyAnalysis :: histFinalize ()
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


/// Chekanov 

// efficiency for all tracks without any cuts
  m_PrimaryTracks->BuildAndBookEfficiency( "ITK/Efficiency_PrimaryAll/", m_PrimaryTruth, wk() );
  m_SecondaryTracks->BuildAndBookEfficiency( "ITK/Efficiency_SecondaryAll/", m_SecondaryTruth, wk() );

// efficiency after using selection on reco tracks 
  m_SelectedPrimaryTracks->BuildAndBookEfficiency( "ITK/Efficiency_PrimarySelected/", m_PrimaryTruth, wk() );
  m_SelectedSecondaryTracks->BuildAndBookEfficiency( "ITK/Efficiency_SecondarySelected/", m_SecondaryTruth, wk() );

  m_SelectedPrimaryFakeTracks->BuildAndBookEfficiency( "ITK/Efficiency_Fake_PrimarySelected/", m_PrimaryTracks, wk() );
  m_SelectedSecondaryFakeTracks->BuildAndBookEfficiency( "ITK/Efficiency_Fake_SecondarySelected/", m_SecondaryTracks, wk() );

  m_DuplicatePrimaryTracks->BuildAndBookEfficiency( "ITK/Rate_DuplicatePrimary/", m_PrimaryTracks, wk() );
  m_DuplicateSecondaryTracks->BuildAndBookEfficiency( "ITK/Rate_DuplicateSecondary/", m_SecondaryTracks, wk() );	
  m_FakeTracks->BuildAndBookEfficiency( "ITK/Rate_Fake_AllTracks/", m_AllTracks, wk() );

  return EL::StatusCode::SUCCESS;
}

bool TrackEfficiencyAnalysis::passTrackQualitySelection(const xAOD::TrackParticle* track)
{
  uint8_t numberOfSCTHits= 0;
  uint8_t expectBLayerHit= 0;
  uint8_t numberOfBLayerHits= 0;
  uint8_t numberOfPixelHits= 0;
  uint8_t numberOfPixelHoles=0;
  
  if(  ! track->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits)) 
    Error("passTrackQualitySelection()", "Failed to retrieved summaryValue Pixel Hits" );
  if(  ! track->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits))
    Error("passTrackQualitySelection()", "Failed to retrieved summaryValue SCT hits" );
  if(  ! track->summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits))
    Error("passTrackQualitySelection()", "Failed to retrieved summaryValue Blayer hits" );
  if(  ! track->summaryValue(expectBLayerHit,xAOD::expectBLayerHit)) 
    Error("passTrackQualitySelection()", "Failed to retrieved summaryValue expected blayer" );
  if(  ! track->summaryValue(numberOfPixelHoles,xAOD::numberOfPixelHoles)) 
    Error("passTrackQualitySelection()", "Failed to retrieved xAOD::numberOfPixelHoles" );

  // if( expectBLayerHit >= 1 && numberOfBLayerHits==0 ) return false;
  // if( numberOfPixelHits < 1 ) return false;
  // if( numberOfSCTHits < 6 ) return false;
  debug_tracks->Fill(1.0);

  if( numberOfPixelHoles > 1 )  return false;
  debug_tracks->Fill(2.0);
  
  //Pt and Eta Cuts
  float eta=track->eta();
  int totalpixels=numberOfSCTHits+numberOfPixelHits;
  //if( totalpixels < 5  ||  (fabs(eta) < 2.4 && totalpixels < 9) ) return false;
  if (totalpixels < 9) return false;
  debug_tracks->Fill(3.0);

  if(track->pt() < 900  ) return false;
  debug_tracks->Fill(4.0);


  if( std::fabs(track->d0()) >= 1.0 ) return false;
  debug_tracks->Fill(5.0);


  if( std::fabs(track->z0()) >= 150. ) return false;
  debug_tracks->Fill(6.0);
 
  // if( track->pt() > 10e3  &&  TMath::Prob( track->chiSquared(), track->numberDoF() ) <= 0.01 ) return false;
  
  return true;

}

bool TrackEfficiencyAnalysis::passIP_Selection(const xAOD::TrackParticle* track, const xAOD::Vertex* vertex)
{
  if(!vertex) return false;
  if( std::fabs(track->d0()) >= 1.0 ) return false;
  if( std::fabs(track->z0() + track->vz() - vertex->z() ) * TMath::Sin(track->theta()) >= 1.5 ) return false;
  return true;
}


