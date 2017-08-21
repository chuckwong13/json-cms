# Lxplus Batch Job Script
#alias eosmount='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select -b fuse mount'
#eosmount /afs/cern.ch/user/l/lara/work/eos 
cd /afs/cern.ch/user/l/lara/work/WorkingArea/CMSSW_5_3_32/src/2011-jet-inclusivecrosssection-ntupleproduction-optimized/AnalysisFW/python
eval `scramv1 runtime -sh`
cd JOB_DIR
cmsRun data_cfg.py
