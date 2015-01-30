delphesfcc
============

C++ example analysis package based on the FCC event datamodel which integrated a delphes reader module (in particular DelphesHEPMCreader).

For documentation about delphes please have a look at 

 http://cp3.irmp.ucl.ac.be/projects/delphes

example:
- example executable with a simple writer of HEPMC + delphes simulation but the FCC-EDM data formats! It will store the "stable" particles produced by delphes simulation (so with status=1)


Prerequisites: before doing anything,
- compile albers and source its init script to set your environment for albers.
- compile fcc-edm and source its init script to set your environment for fcc-edm
- download delphes and compile it 

download and compilation of delphes:
- cd $FCC
- wget http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.2.0.tar.gz
- tar -xvf Delphes-3.2.0.tar.gz 
- cd Delphes-3.2.0
- make

run for example as 
- ./DelphesHepMC cards/delphes_card_FCC_basic.tcl test.root /afs/cern.ch/user/s/selvaggi/public/delphes_tuto/MinBias_100TeV_1.hepmc
(we will compare the content of test.root with what we get later....)

N.B: not important for now, follow https://cp3.irmp.ucl.ac.be/projects/delphes/ticket/375 if you wnat to compile delphes with cmake...


now you can back in the delphesfcc directory

tested so far only on lxplus,
Before compiling or using on lxplus:

    source init.sh


N.B. make sure you have the var DELPHES_DIR correctly set in init.sh;
also set correctly the variable DelphesExternals_INCLUDE_DIR in the CMakeList.txt;
have a look at the cmake/Finddelphes.cmake!


Compilation:

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../install ..
    make -j 4 install
    cd ..

 
Test of the executable

- ./install/bin/delphesfcc-simpleWrite  delphes_card_FCC_basic_noTreeWriter.tcl /afs/cern.ch/user/s/selvaggi/public/delphes_tuto/MinBias_100TeV_1.hepmc

N.B the (delphes)treeWriter from Delphes card is removed, 'cause we are using our own fcc-edm writer!


now you compare the content of the output obtained before with the standalone delphes run

- root -l ../Delphes-3.2.0/test.root 
- Delphes->Draw("Particle.PT", "Particle.Status==1 && abs(Particle.PID)==13")
(Long64_t)88

- root -l simpleexample.root
- events->Draw("Particle.Core.P4.Eta", "abs(Particle.Core.Type)==13")
Info in <TCanvas::MakeDefCanvas>:  created default TCanvas with name c1
(Long64_t)88


    
What to do further:

-    try to store other particles type produced by delphes, and in general build a converter from delphes to fcc-edm datamodel;
-    besides HEPMC, delphes has readers that can handle other MC formats, such as Pythia and HEP, we could easily integrate these other cases;
-   fully integration in GAUDI
    


