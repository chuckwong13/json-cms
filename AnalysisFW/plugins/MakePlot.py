import ROOT
f = ROOT.TFile.Open("mytree.root")
for event in f.JsonTree :
      print event.electron_pt[0]
