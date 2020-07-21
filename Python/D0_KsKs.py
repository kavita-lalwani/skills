import basf2
from basf2 import *
from modularAnalysis import *
from stdCharged import *
import stdCharged as stdc
from stdV0s import *
from stdPi0s import stdPi0s
import vertex as vx
from stdPhotons import stdPhotons
from variables import variables
import math
import sys
import variables.collections as vc
import variables.utils as vu

def main(name1,name2):

    my_path= basf2.create_path()
    
    fileList = name1
    
    inputMdstList('default', fileList, path=my_path)
    
    output_file= '/home/belle2/sanjeeda/D0_Ks_Ks/reconstructed/reco'+name2
    
    #load the lists for pion and kaon
    stdc.stdPi(listtype='all', path=my_path)
    stdc.stdK(listtype='all', path=my_path)
    
    #list of soft pion with cuts
    soft = '[thetaInCDCAcceptance] and [dr<0.5] and [abs(dz)<2] and [nCDCHits>0]'
    fillParticleList('pi+:soft', soft, path=my_path)  
    
    #create merged K_S0 list
    stdKshorts(prioritiseV0=True,fitter='TreeFit',path=my_path) 
    reconstructDecay('K_S0:merged -> pi-:all pi+:all', cut='0.3 < M < 0.7', path=my_path)
    variablesToExtraInfo(particleList= 'K_S0:merged', variables={'M': 'M_preFit'}, path=my_path)
 
    reconstructDecay('D0:sig -> K_S0:merged K_S0:merged', cut='1.7 < M < 2.1', path=my_path)
    
    reconstructDecay('D*+ -> D0:sig pi+:soft', cut='useCMSFrame(p)> 2 and massDifference(0) < 0.16', path=my_path)
    
    matchMCTruth(list_name='D*+',path=my_path)
    
    vx.treeFit(list_name='D*+', conf_level=0.001, ipConstraint=True, updateAllDaughters=True, massConstraint=[310],path=my_path)
    
    vertex_variables = vc.flight_info + ['M','x','y','z','mcX','mcY','mcZ',
                                         'mcProdVertexX','mcProdVertexY','mcProdVertexZ','mcFlightTime','mcFlightDistance']
    d0_vars= vu.create_aliases_for_selected(
        list_of_variables = vc.inv_mass + vc.mc_truth + vertex_variables + vc.kinematics +['isSignalAcceptMissingGamma'],
        decay_string='D*+ -> [^D0:sig -> [K_S0:merged -> pi+:all pi-:all]  [K_S0:merged -> pi+:all pi-:all]] pi+:soft', prefix='Dz')
    
    kshort_vars = vu.create_aliases_for_selected(
        list_of_variables = vc.inv_mass + vc.mc_truth +  vertex_variables + vc.kinematics +  ['extraInfo(M_preFit)'],
        decay_string= 'D*+ -> [D0:sig -> [^K_S0:merged -> pi+:all pi-:all]  [^K_S0:merged -> pi+:all pi-:all]] pi+:soft',prefix=['Ks1','Ks2'])
    
    
    pi_vars = vu.create_aliases_for_selected(
        list_of_variables = vc.kinematics + vc.track + vc.mc_truth + vc.pid,
        decay_string='D*+ -> [D0:sig -> [K_S0:merged -> ^pi+:all ^pi-:all] [K_S0:merged -> ^pi+:all ^pi-:all]] ^pi+:soft',prefix=['pi1','pi2','pi3','pi4','pisoft'])
    
    dstar_vars= vu.create_aliases_for_selected(
        list_of_variables = vc.inv_mass + vc.mc_truth + vc.kinematics+['useCMSFrame(p)','massDifference(0)','isSignalAcceptMissingGamma','Q'] ,
        decay_string='^D*+ -> [D0:sig -> [K_S0:merged -> pi+:all pi-:all] [K_S0:merged -> pi+:all pi-:all]] pi+:soft',prefix ='Dst')
    
    variablesToNtuple('D*+', d0_vars + kshort_vars + pi_vars + dstar_vars,
                      filename=output_file, treename='D0KsKs', path=my_path)
    
 


    process(my_path)
    print(statistics)

                                                                                                                                  
if __name__ == "__main__":
                                                                                                                                 
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('name1', help='input file name')
    parser.add_argument('name2', help='outputfile name')
    
    args = parser.parse_args()
    
    main(args.name1, args.name2)


