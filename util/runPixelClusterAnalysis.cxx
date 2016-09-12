

#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "SampleHandler/DiskListEOS.h"

#include "TrackValidation/PixelClusterAnalysis.h"

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  if( argc > 1 ) submitDir = argv[ 1 ];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;
 // SH::scanDir (sh, submitDir);

   // SH::scanDir( sh, "/users/chakanau/public/2014_tutorial_lbnl/data/" );
   SH::readFileList (sh, "sample", "inputdata.txt");
   // cout << "Read= /users/chakanau/public/2014_tutorial_lbnl/data/ "<< endl;

/*
  SH::DiskListEOS list ("/eos/atlas/atlascerngroupdisk/perf-idtracking/DxAOD/data14_cos",
			"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/DxAOD/data14_cos" );
  SH::scanDir(sh, list, "*InDetDxAOD_lb0002.pool.root*", "*data14_cos.00248326.physics_IDCosmic.recon.AAOD.x290_p003*");
*/

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );

  // Add our analysis to the job:
  PixelClusterAnalysis* alg = new PixelClusterAnalysis();
  job.algsAdd( alg );

  // Run the job using the local/direct driver:
  EL::DirectDriver driver;
  driver.submit( job, submitDir );

  return 0;
}
