
import json
from ROOT import TFile, TTree
from ROOT import gROOT, AddressOf


with open('prueba.json') as data_file:    
    data = json.load(data_file)
    gROOT.ProcessLine(
    "struct MyStruct {\
    Float_t     felectron_pt[1];\
    Float_t     felectron_phi[1];\
    Char_t    fMyCode[4];\
    Float_t     fmet;\
    Int_t    fevent;\
    };" );
    maxn=2
    n=2
    from ROOT import MyStruct
    d = array( 'f', maxn*[ 0. ] )
    mystruct = MyStruct()
    f = TFile( 'mytree.root', 'RECREATE' )
    tree = TTree( 'JsonTree', 'Just A Tree' )
    tree.Branch( 'electron_phi', AddressOf(mystruct, 'felectron_phi'), 'electron_phi[1]/F' )
    tree.Branch( 'electron_pt', AddressOf(mystruct, 'felectron_pt'), 'electron_pt[1]/F' )
    tree.Branch('met', AddressOf( mystruct, 'fmet' ), 'met/F')
    tree.Branch( 'event', AddressOf( mystruct, 'fevent' ), 'event/I' )
    tree.Branch( 'myval', d, 'myval[2]/F' )
    

    
    for iterevent, eventvalue in data["Event"].iteritems():
        numelec=0
        mystruct.fevent=int(iterevent)
        for key, value in eventvalue.iteritems():
            if key=="run":
                run=value;
            if key=="met":
                mystruct.fmet=value
            if key=="electron":
                for electvar, electvalue in value.iteritems():
                   
                    mystruct.felectron_pt[numelec]= electvalue["pt"]
                    mystruct.felectron_phi[numelec]= electvalue["phi"]
                    numelec=numelec+1
                    break
        d[0] = 0
        d[1] = 1
                            
        tree.Fill()
	
	


f.Write()
f.Close()
