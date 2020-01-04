# CMS Jet Tuple and JSON file production 2011 

This project is a CMSSW module producing root and json files from 2011A Jet data.

Source code was originally forked from the SMPJ Analysis Framework: 
https://twiki.cern.ch/twiki/bin/viewauth/CMS/SMPJAnalysisFW  
https://github.com/cms-smpj/SMPJ/tree/v1.0/  
https://github.com/laramaktub/json-collisions


The instruction assume that you will work on a VM properly contextualized for CMS, available from http://opendata.cern.ch/VM/CMS.

## Creating the working area

This step is only needed the first time you run this program:
```
mkdir WorkingArea
cd ./WorkingArea
cmsrel CMSSW_5_3_32
cd ./CMSSW_5_3_32/src
cmsenv
git clone https://github.com/chuckwong13/json-collisions
cd ./json-collisions
scram b
```

## Setting up additional files

With `json-collisions/AnalysisFW/python/` as the current folder, run the following commands:

1. Download index files : 
    
    ```
    wget  -P Index_files/ http://opendata.cern.ch/record/1394/files/CMS_MonteCarlo2011_Summer11LegDR_DYJetsToLL_M-50_7TeV-madgraph-pythia6-tauola_AODSIM_PU_S13_START53_LV6-v1_00002_file_index.txt
    wget  -P Index_files/ http://opendata.cern.ch/record/1544/files/CMS_MonteCarlo2011_Summer11LegDR_TTJets_TuneZ2_7TeV-madgraph-tauola_AODSIM_PU_S13_START53_LV6-v1_010001_file_index.txt
    wget  -P Index_files/ http://opendata.cern.ch/record/1633/files/CMS_MonteCarlo2011_Summer11LegDR_WJetsToLNu_TuneZ2_7TeV-madgraph-tauola_AODSIM_PU_S13_START53_LV6-v1_00002_file_index.txt
    wget  -P Index_files/ http://opendata.cern.ch/record/1343/files/CMS_MonteCarlo2011_Summer11LegDR_ZZJetsTo4L_TuneZ2_7TeV-madgraph-tauola_AODSIM_PU_S13_START53_LV6-v1_020000_file_index.txt
    ```
    now we have 4 index files
    
2. Create links to the condition databases:
    ```
    ln -sf /cvmfs/cms-opendata-conddb.cern.ch/START53_LV6A1 START53_LV6A1
    ln -sf /cvmfs/cms-opendata-conddb.cern.ch/START53_LV6A1.db START53_LV6A1.db
    ```
    
    Make sure the cms-opendata-conddb.cern.ch directory has actually expanded in your VM. One way of doing this is executing:
    ```
    ls -l
    ls -l /cvmfs/
    ```
    
## Run the program:
 
This command creates json files we need from Monte Carlo simulations:

```
    python runthisone.py 
```
 
After running this command, we can gain 4 different json files located in '/outputjsons', corresponding to 4 different index files we've downloaded above.
