#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TrackValidation/PixelClusterAnalysis.h>


// EDM includes: - if move to header file will not compile?
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODRootAccess/TStore.h"
#include "xAODCore/ShallowCopy.h"


// this is needed to distribute the algorithm to the workers
ClassImp(PixelClusterAnalysis)


//____________________________________________________________________________________________________
// Copy of enum MeasurementType from Athena: Tracking/TrkEvent/TrkEventPrimitives/TrkEventPrimitives/TrackStateDefs.h
namespace TrackState {
  enum MeasurementType {
    unidentified = 0,
    Pixel      = 1,
    SCT        = 2,
    TRT        = 3,
    MDT        = 4,
    CSC        = 5,
    RPC        = 6,
    TGC        = 7,
    Pseudo     = 8,
    Vertex     = 9,
    Segment    = 10,
    SpacePoint = 11,
    LArCal     = 12,
    TileCal    = 13,
    STGC       = 14,
    MM         = 15,
    NumberOfMeasurementTypes=16
  };
}

//____________________________________________________________________________________________________
// Copy of enum TrackStateOnSurfaceType from Athena: Tracking/TrkEvent/TrkTrack/TrkTrack/TrackStateOnSurface.h
namespace TrackStateOnSurface {
  enum TrackStateOnSurfaceType {
    /** This is a measurement, and will at least contain a Trk::MeasurementBase*/
    Measurement=0,
 
    /** This represents inert material, and so will contain MaterialEffectsBase */
    InertMaterial=1, 
            
    /** This represents a brem point on the track,
     * and so will contain TrackParameters and MaterialEffectsBase */
    BremPoint=2, 
         
    /** This represents a scattering point on the track,
     * and so will contain TrackParameters and MaterialEffectsBase */
    Scatterer=3, 
          
    /** This represents a perigee, and so will contain a Perigee
     * object only*/
    Perigee=4, 
         
    /** This TSoS contains an outlier, that is, it contains a
     * MeasurementBase/RIO_OnTrack which was not used in the track
     * fit*/
    Outlier=5,
          
    /** A hole on the track - this is defined in the following way.
     * A hole is a missing measurement BETWEEN the first and last
     * actual measurements. i.e. if your track starts in the SCT,
     * you should not consider a missing b-layer hit as a hole.*/
    Hole=6,
    /** For some reason this does not fall into any of the other categories
     * PLEASE DO NOT USE THIS - DEPRECATED!*/
    Unknown=7,
           
    /** This TSOS contains a CaloEnergy object*/
    CaloDeposit=8,
              
    /**
     * This TSOS contains a Trk::MeasurementBase
     */
    Parameter=9,
            
    /**
     * This TSOS contains a Trk::FitQualityOnSurface
     */
    FitQuality=10,
    NumberOfTrackStateOnSurfaceTypes=11
  };
}


PixelClusterAnalysis :: PixelClusterAnalysis ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode PixelClusterAnalysis :: setupJob (EL::Job& job)
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
  xAOD::Init( "PixelClusterAnalysis" ).ignore(); // call before opening first file
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  //TDirectory *cdtof = top->mkdir("Pixels");
  //cdtof->cd();


  // track quality
  m_QualAllTracks = new TrackQualityHists("AllTracks");
  m_QualAllTracks->Book( wk() );

  // pixels
  myXY = new TH2F("ITK/Pixel/Pixels2D", "Pixel global XY", 500, 0, 500, 500, 0, 500);
  myXZ = new TH2F("ITK/Pixel/Pixels2D_XZ", "Pixel XZ", 6000, -3000, 3000,500, 0, 500);
  myXY->GetXaxis()->SetTitle("X global [mm]");
  myXY->GetYaxis()->SetTitle("Y global [mm]");
  myXZ->GetXaxis()->SetTitle("Z global [mm]");
  myXZ->GetYaxis()->SetTitle("X-Y global [mm]");
  //fileout = fopen ("clusters.txt", "w");

  myZ = new TH1F("ITK/Pixel/PixelsZ", "Pixel Z", 3000, -3000, 3000);
  myZ->GetXaxis()->SetTitle("Z global [mm]");
  myZ->GetYaxis()->SetTitle("Entries");

  myX = new TH1F("ITK/Pixel/PixelsX", "Pixel trans", 250, 0, 500);
  myX->GetXaxis()->SetTitle("X-Y global [mm]");
  myX->GetYaxis()->SetTitle("Entries");

  myTOT = new TH1F("ITK/Pixel/TOT", "TOT", 250, 0, 500);
  myTOT->GetXaxis()->SetTitle("TOT");
  myTOT->GetYaxis()->SetTitle("Entries");

  myLayer = new TH1F("ITK/Pixel/Layer", "Layer", 10, 0, 10);
  myLayer->GetXaxis()->SetTitle("Nr layer");
  myLayer->GetYaxis()->SetTitle("Entries");

  wk()->addOutput (myXY);
  wk()->addOutput (myXZ);
  wk()->addOutput (myZ);
  wk()->addOutput (myX);
  wk()->addOutput (myTOT);
  wk()->addOutput (myLayer);


  stXY = new TH2F("ITK/Strips/Strips2D", "Strip global XY", 500, 0, 1000, 500, 0, 1000);
  stXZ = new TH2F("ITK/Strips/Strips2D_XZ", "Strip XZ", 6000, -3000, 3000,500, 0, 1000);
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


  myPt = new TH1F("ITK/Tracks/pt", "Track pT [GeV]", 100, 0, 2000);
  myPt->GetXaxis()->SetTitle("pT [GeV]");
  myPt->GetYaxis()->SetTitle("Entries");

  myEta = new TH1F("ITK/Tracks/eta", "Track Eta", 20, -4.0, 4.0);
  myEta->GetXaxis()->SetTitle("Eta");
  myEta->GetYaxis()->SetTitle("Entries");

  myPhi = new TH1F("ITK/Tracks/phi", "Track Phi [rad]", 20, -3.5, 3.5);
  myPhi->GetXaxis()->SetTitle("Phi");
  myPhi->GetYaxis()->SetTitle("Entries");

  wk()->addOutput (myPt);
  wk()->addOutput (myEta);
  wk()->addOutput (myPhi);



  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: initialize ()
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



EL::StatusCode PixelClusterAnalysis :: execute ()
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


  //---------------------
  //  // TRUTH PARTICLES
  //  //---------------------
  //  // get track container of interest
  const xAOD::TruthParticleContainer* truthParticles = 0;
  if ( !m_event->retrieve( truthParticles, "TruthParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
       Error("execute()", "Failed to retrieve Truth Particle container. Exiting." );
       return EL::StatusCode::FAILURE;
    }
  
  for( auto truthPart: *truthParticles ) {
/*
    if( isPrimary( truthPart ) ) {
       m_PrimaryTruth->Fill( truthPart );
       } else if( isSecondary( truthPart ) ){
      m_SecondaryTruth->Fill( truthPart );
              }
*/
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
             m_QualAllTracks->Fill( track );
  }


  // loop over the tracks in the container
  // auto type: xAOD::TrackParticleContainer::const_iterator
  for( auto recoTrk_itr = recoTracks->begin(); recoTrk_itr != recoTracks->end(); recoTrk_itr++) {

    // Make your track selection
    //if ( (*recoTrk_itr)->pt() < 1000 )
    //  continue;

    // Info("execute()",  "Track  (pT, eta, phi)  = (%f, %f, %f)", (*recoTrk_itr)->pt(), (*recoTrk_itr)->eta(), (*recoTrk_itr)->phi() );
    myPt->Fill( (*recoTrk_itr)->pt() / 1000.);
    myEta->Fill( (*recoTrk_itr)->eta() );
    myPhi->Fill( (*recoTrk_itr)->phi() );


    typedef std::vector<ElementLink< xAOD::TrackStateValidationContainer > > MeasurementsOnTrack;
    //typedef std::vector<ElementLink< xAOD::TrackStateValidationContainer > >::const_iterator MeasurementsOnTrackIter;
    
    static const char* measurementNames = "IDDET1_msosLink";   //Note the prefix could change

    // Check if there are MSOS attached
    if( ! (*recoTrk_itr)->isAvailable< MeasurementsOnTrack >( measurementNames ) ) {
      continue;
    }
    // Get the MSOS's
    const MeasurementsOnTrack& measurementsOnTrack = (*recoTrk_itr)->auxdata< MeasurementsOnTrack >( measurementNames );

    // Loop over track TrackStateValidation's
    
    
    // auto type: MeasurementsOnTrackIter ( == std::vector<ElementLink< xAOD::TrackStateValidationContainer > >::const_iterator )
    for( auto msos_iter = measurementsOnTrack.begin(); msos_iter != measurementsOnTrack.end(); ++msos_iter)
    {  
      //Check if the element link is valid
      if( ! (*msos_iter).isValid() ) {
        continue;
      }
      
      const xAOD::TrackStateValidation* msos = *(*msos_iter); 
  


     // ---- everything but pixels
     if( msos->detType() != TrackState::Pixel  &&  msos->type() == TrackStateOnSurface::Measurement ) {

      //Get pixel cluster
      if(  msos->trackMeasurementValidationLink().isValid() && *(msos->trackMeasurementValidationLink()) ){
           const xAOD::TrackMeasurementValidation* cluster_itr =  *(msos->trackMeasurementValidationLink());

/*
    float charge = (cluster_itr)->auxdataConst< float >("charge");
    float TOT = (cluster_itr)->auxdataConst< int >("ToT");
    int layer = (cluster_itr)->auxdataConst< int >("layer");

   double xx= (cluster_itr)->globalX();
   double yy=(cluster_itr)->globalY();
   double zz=(cluster_itr)->globalZ();
   double ppp=sqrt(xx*xx+yy*yy);

  stXY->Fill(xx, yy);
  stXZ->Fill(zz,ppp);
  stZ->Fill(zz);
  stX->Fill(ppp);
  stTOT->Fill((float)TOT);
  stLayer->Fill((float)layer);
*/
    }
    } // end run over positions 





      // -------------------- pixel staff

   
      // The detector type enum is defined in TrkEventPrimitives/TrackStateDefs.h
      // The mesurement state on surface type is defined in the header file TrkTrack/TrackStateOnSurface.h
      // We will port the enums to xAOD world in the future
      if( msos->detType() != TrackState::Pixel  ||  msos->type() != TrackStateOnSurface::Measurement )  //its   pixel && and a measurement on track
        continue;
      
      // Info("execute()",  "->MSOS is a pixel with the following ID %li", msos->detElementId()); 
      
      //Get pixel cluster
      if(  msos->trackMeasurementValidationLink().isValid() && *(msos->trackMeasurementValidationLink()) ){
        const xAOD::TrackMeasurementValidation* pixelCluster =  *(msos->trackMeasurementValidationLink());        
        // Access a dressed member    
        float charge(0);
        if( pixelCluster->isAvailable< float >( "charge" ) )   // Check is not required if you know it is there
          charge = pixelCluster->auxdata< float >("charge"); 

        // Info("execute()",  "-->Pixel Cluster local position (%f, %f) has the charge %f", pixelCluster->localX(), pixelCluster->localY(), charge );
 
        if(isMC){
          std::vector<int> truth_barcode;
          if( pixelCluster->isAvailable< std::vector<int> >( "truth_barcode" ) ) 
            truth_barcode = pixelCluster->auxdata< std::vector<int> >("truth_barcode"); 
	  // auto type: std::vector<int>::const_iterator
          for( auto barcode = truth_barcode.begin(); barcode !=truth_barcode.end();  ++barcode){
            Info("execute()",  "--->Pixel Cluster orginates from particle with barcode %i", *barcode);
          } 
        } //End check on MC           
      } // End Check on Element link     
    } // End for loop over MSOS's
  } // end for loop over reconstructed tracks


  // If you don't care about the global properties of the track you can loop over the MSOS directly 
  // Get the measurement state on surface
  const xAOD::TrackStateValidationContainer* pixelMSOSs = 0;
  if ( !m_event->retrieve( pixelMSOSs, "PixelMSOSs" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Pixel TrackStateValidationContainer. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  // loop over the tracks in the container

  // Info("execute()",  "Looping over the TrackStateValidation (PixelMSOSs) container");
  // auto type: xAOD::TrackStateValidationContainer::const_iterator
  for( auto msos_itr = pixelMSOSs->begin(); msos_itr != pixelMSOSs->end(); msos_itr++) 
  {  
  
 //  if( (*msos_itr)->detType() == TrackState::Pixel  &&  (*msos_itr)->type() == TrackStateOnSurface::Measurement ) //its  a pixel && and a a measurement on track    
    //Info("execute()",  "MSOS is a pixel with the following ID %li", (*msos_itr)->detElementId());
  }  
  
  
  //If you don't about the track properties at all you can loop over the the pixel clusters
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

   // Info("execute()",  "Pixel Cluster local position (%f, %f) with charge %f",  (*cluster_itr)->localX(),  (*cluster_itr)->localY(), charge );  
  myXY->Fill(xx, yy);
  myXZ->Fill(zz,ppp);
  myZ->Fill(zz);
  myX->Fill(ppp);
  myTOT->Fill((float)TOT);
  myLayer->Fill((float)layer);

     

   //cout << "pixels: " << xx << " " << yy << " " << zz << endl;

 
    // not supported for ITK 
    // Info("execute()",  "Size of the NN charge matrix %li", NN_matrixOfCharge( **cluster_itr ).size() ); 
    
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

   // Info("execute()",  "Pixel Cluster local position (%f, %f) with charge %f",  (*cluster_itr)->localX(),  (*cluster_itr)->localY(), charge );  
      stXY->Fill(xx, yy);
      stXZ->Fill(zz,ppp);
      stZ->Fill(zz);
      stX->Fill(ppp);
      stTOT->Fill((float)TOT);
      stLayer->Fill((float)layer);
   //
 
   }



 
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode PixelClusterAnalysis :: finalize ()
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



EL::StatusCode PixelClusterAnalysis :: histFinalize ()
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

  // fclose(fileout);

  return EL::StatusCode::SUCCESS;
}
