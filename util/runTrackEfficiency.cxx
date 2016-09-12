#include <unistd.h>

#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoopGrid/PrunDriver.h"

#include "TrackValidation/TrackEfficiencyAnalysis.h"

#include <string>
#include <sys/stat.h>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
void usage() {
  std::cout << "Usage: runTrackEfficiency filename driver(local/grid) outputtag\n"
           << std::endl;
                       
}

int main( int argc, char* argv[] ) {


// Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  std::string samplePath="inputdata.txt";
  std::string driverName = "local";
  std::string outputtag = "";
  if( argc > 1 ) samplePath = argv[ 1 ];
  if( argc > 2 ) driverName = argv[ 2 ];
  if( argc > 3 ) outputtag = argv[ 3 ];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

 // SH::scanDir (sh, submitDir);

   // SH::scanDir( sh, "/users/chakanau/public/2014_tutorial_lbnl/data/" );
//   SH::readFileList (sh, "sample", "inputdata.txt");
   // cout << "Read= /users/chakanau/public/2014_tutorial_lbnl/data/ "<< endl;

/*
  SH::DiskListEOS list ("/eos/atlas/atlascerngroupdisk/perf-idtracking/DxAOD/data14_cos",
                        "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/DxAOD/data14_cos" );
  SH::scanDir(sh, list, "*InDetDxAOD_lb0002.pool.root*", "*data14_cos.00248326.physics_IDCosmic.recon.AAOD.x290_p003*");
*/
  std::string containerName;
  std::string userName = getlogin();
  std::vector< std::string > outputContainerNames; //for grid only
  ///////// Set output container name //////////////
  std::string outputName;

  //Check if input is a directory or a file
  if( driverName=="grid"){ //It is samples for the grid
    std::ifstream inFile( samplePath );
	  struct stat st;
    stat("gridoutput/gridjobs/",&st);
    if(!S_ISDIR(st.st_mode)) system("mkdir -p \"gridoutput/gridjobs/\" ");
    while(std::getline(inFile, containerName) ){
      if (containerName.size() > 1 && containerName.find("#") != 0 ){
        // Construct the samples to run on:
        SH::SampleHandler sh;
        std::cout << "Adding container " << containerName << std::endl;
        SH::scanDQ2( sh, containerName);
        //Add output container name to file of containers
        //follows grid format: "user."+userName+".%in:name[1]%.%in:name[2]%.%in:name[3]%"+outputTag
        int namePosition = 0;
        //int startPosition = containerName.find_first_of(".", namePosition)+1;
        //namePosition = containerName.find_first_of(".", namePosition)+1;

        namePosition = containerName.find_first_of(".", namePosition)+1;
        namePosition = containerName.find_first_of(".", namePosition)+1;
        namePosition = containerName.find_first_of(".", namePosition)+1;
        //outputContainerNames.push_back( ("user."+userName+"."+containerName.substr(startPosition, namePosition)+outputTag+"/") );
        outputContainerNames.push_back( ("user."+userName+"."+containerName.substr(0, namePosition)+"_"+outputtag+"/") );


      // Set the name of the input TTree. It's always "CollectionTree"
      // for xAOD files.
      sh.setMetaString( "nc_tree", "CollectionTree" );
    //-----------------------------------new---------------------------------//
      sh.setMetaString("nc_grid_filter", "*");  //Data files on grid to not end in .root
      sh.print();
    //-----------------------------------new---------------------------------//
      // Print what we found:
      sh.print();

      // Create an EventLoop job:
      EL::Job job;
      job.sampleHandler( sh );

      // Add our analysis to the job:
      TrackEfficiencyAnalysis* alg = new TrackEfficiencyAnalysis();
      job.algsAdd( alg );

      // Run the job using the local/direct driver:
    //  EL::DirectDriver driver;
    //  driver.submit( job, submitDir );
        submitDir = "gridoutput/gridjobs/submitDir_user."+userName+"."+containerName.substr(0, namePosition)+"_"+outputtag;
        outputName = "user."+userName+"."+containerName.substr(0, namePosition)+"_"+outputtag;
        EL::PrunDriver driver;
        driver.options()->setString("nc_outputSampleName", outputName);

        driver.options()->setDouble(EL::Job::optGridNFilesPerJob, 1);
        //driver.options()->setString(EL::Job::optRootVer, "5.34.25");
        //driver.options()->setString(EL::Job::optCmtConfig, "x86_64-slc6-gcc48-opt");
        //driver.options()->setDouble("nc_nGBPerJob", 1);
        //driver.options()->setString("nc_excludeSite", ???);
        driver.options()->setString(EL::Job::optGridMergeOutput, "true");
        //driver.options()->setDouble(EL::Job::optGridMemory,10240); // 10 GB
    //    driver.options()->setString("nc_skipScout", "true");

        //driver.submit(job, submitDir); // with monitoring
        driver.submitOnly(job, submitDir); //without monitoring    
      }
    }
  }
  else{    
    // Construct the samples to run on:
    SH::SampleHandler sh;
    SH::readFileList (sh, "sample", samplePath);
    // Set the name of the input TTree. It's always "CollectionTree"
    // for xAOD files.
    sh.setMetaString( "nc_tree", "CollectionTree" );
  //-----------------------------------new---------------------------------//
    sh.setMetaString("nc_grid_filter", "*.root*");  //Data files on grid to not end in .root
    sh.print();
  //-----------------------------------new---------------------------------//
    // Print what we found:
    sh.print();

    // Create an EventLoop job:
    EL::Job job;
    job.sampleHandler( sh );

    // Add our analysis to the job:
    TrackEfficiencyAnalysis* alg = new TrackEfficiencyAnalysis();
    job.algsAdd( alg );
    EL::DirectDriver driver;
    driver.options()->setString("nc_outputSampleName", outputName);
    driver.submit( job, submitDir );
  }

//-----------------------------------new---------------------------------//
  if( driverName=="grid"){    
    for( unsigned int iCont=0; iCont < outputContainerNames.size(); ++iCont){
    ///// For grid, save list of ouput containers to the submission directory /////
      std::ofstream fileList(("gridoutput/gridjobs/outputContainers.txt"), std::ios_base::out);
      for( unsigned int iCont=0; iCont < outputContainerNames.size(); ++iCont){
        fileList << outputContainerNames.at(iCont)+"\n";
      }
      fileList.close();
    }
  }

//-----------------------------------new---------------------------------//

 
  return 0;
}
