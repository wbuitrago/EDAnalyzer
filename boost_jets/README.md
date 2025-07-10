FatJet_Sel.py ---> more or less the same of mkShapesRDF (https://github.com/ldellape/mkShapesRDF/blob/master/mkShapesRDF/processor/modules/FatJetSel.py)

fatjet_image.cpp --> images using the RDataFrame from FatJet_Sel.py


jetConstituents : EDAnalyzer in cmssw, expected miniAOD(AOD) with slimmedJetAK8(ak8JetsPuppi) collection as input. 

~~~~
cmssw-el8
cmsrel CMSSW_13_0_x
cd CMSSW_13_0_x/src
cmsenv
mkdir jetConstituents
mkedanlzr jetConstituents
cp the content of jetConstituents 
cd jetConstituents
scram b
cmsRun ./python/pf_slimmedAK8_cff.py
~~~~
example images with PF objects from fatjets : https://cernbox.cern.ch/s/6keDYWmnkp2JQ54

****************** AOD input ******************** \
"ak8PFJets" and "ak8PFJetsCHS" not yet available in the AOD collections.\
Only "ak8PFJetsPuppi" works.\
Informations that will be stored in the output tree: FatJet kinematics variables + PF variables\
For each event: \
pT of the FatJet\
eta of the FatJet\
phi of the FatJet\
mass of the FatJet\
Idx of the FatJet\
total number of PF inside the FatJet\
total number of electrons inside the FatJet\
total number of muons inside the FatJet\
total number of neutral hadron inside the FatJet\
total number of charged hadrons inside the FatJet\
PF candidate pt (array) \
PF candidate eta (array)\
PF candidate phi (array)\
PF candidate type of particle (array)\
PF candidate cross reference to idx of the FatJet (array)\
************************************************************\
PFCandidate/PFCandidatesPtr class ( https://tinyurl.com/2ph6xcvz )\
PFJet class ( https://tinyurl.com/mwaddn4c )



