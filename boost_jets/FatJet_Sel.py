import ROOT
import sys


HLT_paths = ["HLT_AK8PFJetFwd15==true", "HLT_AK8PFJet230_SoftDropMass40==true"]
FatJet_variables = [ "pt", "eta" , "phi"  ]

cuts = {
    'forward' : {
        'pt_max' : 1000,
        'eta_min' : 2.4,
        'eta_max' : 4.5, 
   #     'tau21' : 0.45, # only if in non-boosted topologies in the gridpack, wp to be check
    },
    'central' : {
        'pt_min' : 30,
        'eta_max' : 2.4, 
        'mass_range' : [70, 100],
    #    'tau21' : 0.45,  # only if in non-boosted topologies in the gridpack, wp to be check 
    },
}

def fatjet_image(sample):
    sInput = "/eos/user/l/ldellape/VBS/VBS_cards/privateMCproduction/nanov12_samples"
    sOutput = ""
    
    if sample=="ZZ":
        input_nano = sInput + "/ZZ*/*.root"
        sOutput = "ZZ_cleanFatJet.root"
    elif sample=="ssWW":
        input_nano = sInput + "/ssWW*/*.root"
        sOutput= "WW_cleanFatJet.root"
    
    rdf = ROOT.RDataFrame("Events", input_nano)
    fatjet_trigger_selection = " && ".join(HLT_paths)
    
    frw_cuts = (f"FatJet_pt <= {cuts['forward']['pt_max']} && " +
                f"(abs(FatJet_eta) >= {cuts['forward']['eta_min']} && abs(FatJet_eta) <= {cuts['forward']['eta_max']})  " 
            #    f"(FatJet_tau21 < {cuts['forward']['tau21']})" 
                )
    central_cuts = (f"FatJet_pt >= {cuts['central']['pt_min']} && " +
                    f"abs(FatJet_eta) <= {cuts['central']['eta_max']} && " + 
                    f"(FatJet_mass > {cuts['central']['mass_range'][0]} && FatJet_mass < {cuts['central']['mass_range'][1]})  " 
                #   f"(FatJet_tau21 < {cuts['forward']['tau21']})"   
                    )  
    print("***  central cuts *******")  
    print(central_cuts)
    print("*** forward cuts ******* ")
    print(frw_cuts)
    rdf = rdf.Define("FatJet_tau21", "FatJet_tau2/FatJet_tau1")
    
    
    rdf = rdf.Define("central_mask", central_cuts)
    rdf = rdf.Define("frw_mask", frw_cuts)
    columns_to_save = []
    for var in FatJet_variables:
        columns_to_save.append(f"FatJet_frw_{var}")
        columns_to_save.append(f"FatJet_central_{var}")
        rdf = rdf.Define(f"FatJet_central_{var}", f"FatJet_{var}[central_mask]")
        rdf = rdf.Define(f"FatJet_frw_{var}", f"FatJet_{var}[frw_mask]")
    

    rdf.Snapshot("Events", sOutput, columns_to_save)
    
if __name__ == "__main__":
    data_sample = sys.argv[1]
    fatjet_image(data_sample)