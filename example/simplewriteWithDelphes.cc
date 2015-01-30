// Data model
#include "datamodel/EventInfo.h"
#include "datamodel/EventInfoCollection.h"
#include "datamodel/MCParticle.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/Particle.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/LorentzVector.h"

// Utility functions
#include "utilities/VectorUtils.h"

// ROOT
#include "TLorentzVector.h"
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"

// STL
#include <iostream>
#include <vector>

#include <stdexcept>
#include <sstream>

#include <signal.h>



// albers specific includes
#include "albers/EventStore.h"
#include "albers/Registry.h"
#include "albers/Writer.h"

// testing tools
#include "utilities/DummyGenerator.h"

// for Delphes
#include "TROOT.h"
#include "TApplication.h"

#include "TObjArray.h"
#include "TStopwatch.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"



#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTask.h"


#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootProgressBar.h"



#include "classes/DelphesModule.h"

#include "classes/SortableObject.h"
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"


#include "classes/DelphesFactory.h"
#include "classes/DelphesHepMCReader.h"




static bool interrupted = false;



using namespace std;



int main(int argc, char* argv[]){
  std::cout<<"start processing"<<std::endl;

  albers::Registry   registry;
  albers::EventStore store(&registry);
  albers::Writer     writer("simpleexample.root", &registry);
  
  
  // delphes stuff here
  //  ExRootTreeWriter *treeWriter = 0; // it will always remain to 0 ??? ....
  //  ExRootTreeBranch *branchEvent = 0;
  ExRootConfReader *confReader = 0;
  Delphes *modularDelphes = 0;
  DelphesFactory *factory = 0;
  TObjArray *stableParticleOutputArray = 0, *allParticleOutputArray = 0, *partonOutputArray = 0;
  DelphesHepMCReader *reader = 0;
  Int_t i, maxEvents, skipEvents;
  Long64_t length, eventCounter;
  FILE *inputFile = 0;
  TFile *outputFile = 0; 
  stringstream message;
  TStopwatch readStopWatch, procStopWatch;
  
  
  if(argc < 2)
    {
      cout << " Usage: "  <<  " config_file" << " input_file " 
	   << endl;
      cout << " input_file(s) - input file(s) in HEPMC format," << endl;
      cout << " config_file - configuration file in Tcl format," << endl;
      //      cout << " output_file - output file in ROOT format," << endl;
      return 1;
    }
  
  // the input files is an HEPMC file
  cout << "** Reading " << argv[2] << endl;
  inputFile = fopen(argv[2], "r");
  
  if(inputFile == NULL)
    {
      message << "can't open " << argv[2];
      throw runtime_error(message.str());
    }
  
  fseek(inputFile, 0L, SEEK_END);     
  
  length = ftello(inputFile);
  fseek(inputFile, 0L, SEEK_SET);
  

  
  cout << "** length of input file " << length << endl;
  
  if(length <= 0)
    {
      fclose(inputFile);
    }
  
  
  // now read delphes card
  
  confReader = new ExRootConfReader;
  confReader->ReadFile(argv[1]);
  
  maxEvents = confReader->GetInt("::MaxEvents", 0);
  skipEvents = confReader->GetInt("::SkipEvents", 0);
  
  if(maxEvents < 0)
    {
      throw runtime_error("MaxEvents must be zero or positive");
    }
  
  if(skipEvents < 0)
    {
      throw runtime_error("SkipEvents must be zero or positive");
    }
  
  
  
  modularDelphes = new Delphes("Delphes");
  modularDelphes->SetConfReader(confReader);
  //  modularDelphes->SetTreeWriter(treeWriter);
  
  

  factory = modularDelphes->GetFactory();
  allParticleOutputArray = modularDelphes->ExportArray("allParticles");
  stableParticleOutputArray = modularDelphes->ExportArray("stableParticles");
  partonOutputArray = modularDelphes->ExportArray("partons");
  
  
  reader = new DelphesHepMCReader;
  
  modularDelphes->InitTask();
  
  
  // now access delphes recontructed particles
  reader->SetInputFile(inputFile);
  
  ExRootProgressBar progressBar(length);
  

  // Loop over all objects                                                                              
  eventCounter = 0;
  modularDelphes->Clear();
  reader->Clear();
  readStopWatch.Start();
  
  
  EventInfoCollection& evinfocoll = store.create<EventInfoCollection>("EventInfo");
  ParticleCollection& pcoll = store.create<ParticleCollection>("Particle");
  
  writer.registerForWrite<EventInfoCollection>("EventInfo");
  
  // collections from the dummy generator
  writer.registerForWrite<ParticleCollection>("Particle");
  
  
  while((maxEvents <= 0 || eventCounter - skipEvents < maxEvents) &&
        reader->ReadBlock(factory, allParticleOutputArray,
			  stableParticleOutputArray, partonOutputArray) && !interrupted)
    {
      if(reader->EventReady())
        {
          ++eventCounter;
	  
          readStopWatch.Stop();
	  
          if(eventCounter > skipEvents)
	    {
	      procStopWatch.Start();
	      modularDelphes->ProcessTask();
	      procStopWatch.Stop();
	      
	      // fill event information
	      
	      EventInfoCollection* evinfocoll = nullptr;
	      
	      store.get("EventInfo", evinfocoll);
	      if(evinfocoll==nullptr) {
		std::cerr<<"collection EventInfo does not exist!"<<std::endl;
		return 1;
	      }
	      EventInfoHandle evinfo = evinfocoll->create();
	      evinfo.mod().Number = eventCounter;

	      
	      Candidate * candidate;
	      for(i = 0; i < stableParticleOutputArray->GetEntriesFast(); ++i)
		{
		  candidate = static_cast<Candidate *>(stableParticleOutputArray->At(i));
		  /* std::cout << "reading a candidate " << std::endl;
		     std::cout << "cand PID " << candidate->PID << std::endl;
		     std::cout << "cand Status " << candidate->Status << std::endl;
		  */
		  // here, asking the store for the collection.
		  // could also just reuse the reference obtained at the time of the creation
		  // of the collection
		  
		  
		  ParticleHandle ptc = pcoll.create();
		  ptc.mod().Core.Type = candidate->PID;
		  ptc.mod().Core.Status = candidate->Status;
		  auto& p4 = ptc.mod().Core.P4;
		  p4.Pt = candidate->Momentum.Pt();
		  p4.Eta = candidate->Momentum.Eta();
		  p4.Phi = candidate->Momentum.Phi();
		  p4.Mass = candidate->Mass;
		  
		  
		  if(eventCounter%1000 == 0) {
		    std::cout<<"writing a stable particle with pT="<<p4.Pt<<std::endl;
		  }
		  
		}
	      
	      writer.writeEvent();
	      store.next();
	      
	    }
	  
          modularDelphes->Clear();
          reader->Clear();
	  
          readStopWatch.Start();
        }
      progressBar.Update(ftello(inputFile), eventCounter);
    }
  
  
  
  
  
  fseek(inputFile, 0L, SEEK_END);
  progressBar.Update(ftello(inputFile), eventCounter, kTRUE);
  progressBar.Finish();
  
  if(inputFile != stdin) fclose(inputFile);
      

  
  modularDelphes->FinishTask();
  
  
  
  cout << "** Exiting..." << endl;



  writer.finish();


  
  
  
  delete reader;
  delete modularDelphes;
  delete confReader;
  delete outputFile;
  
  
  return 0;
}
