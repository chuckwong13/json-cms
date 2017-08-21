
import json
from ROOT import TFile, TTree
from ROOT import gROOT, AddressOf
from array import array


with open('short.json') as data_file:    
    data = json.load(data_file)
    #We define the trees    
    f = TFile( 'mytree.root', 'RECREATE' )
    tree = TTree( 'JsonTree', 'Just A Tree' )
    #maximum number of electrons that we expect to have per event
    maxn=10
    
    myelectron_pt = array( 'f', maxn*[ 0. ] )
    nel = array( 'i', [ 0 ] )
    myrun = array( 'i', [ 0 ] )
    mymet= array ('f', [0])
        
    for iterevent, eventvalue in data["Event"].iteritems():
        #Extract number of electrons from the Json file
        numel=int(data["Event"][iterevent]["numel"])
        #Create the tree branches
        tree.Branch( 'nelec', nel, 'nelec/I' )
        tree.Branch( 'run', myrun, 'run/I' )
        tree.Branch( 'met', mymet, 'met/F' )
        tree.Branch( 'electron_pt', myelectron_pt, 'electron_pt[nelec]/F' )

        nel[0]=numel
        counterelec=0
        for key, value in eventvalue.iteritems():
            if key=="run":
                myrun[0]=value
            if key=="met":
                mymet[0]=value
            if key=="electron":
                for electvar, electvalue in value.iteritems():
                    myelectron_pt[counterelec]=float(electvalue["pt"])
                    counterelec=counterelec+1

        tree.Fill()
	
	


f.Write()
f.Close()
