import os
#os.system('cmsRun /home/cms-opendata/WorkingArea/CMSSW_5_3_32/src/json-cms/AnalysisFW/python/OpenDataTreeProducerOptimized_mcPAT_2011_cfg.py')

cwd = os.getcwd()
index_path = cwd + '/Index_files'

INPUTINDEX = []
for root, dirs, files in os.walk(index_path):  
	INPUTINDEX = files
	
#print(INPUTINDEX)
def findSubStr(substr, str):
        index0 = str.find(substr)   
        index1=str.find(substr,index0+1)  
        index2=str.find(substr,index1+1)  
        index3=str.find(substr,index2+1)
        return index2, index3
        
ionput = []
for files in INPUTINDEX:
	index2, index3 = findSubStr('_', files)
	#print(files[index2+1:index3])
	ionput.append(('Index_files/' + files, files[index2+1:index3]+'.json'))
#print(ionput)

for i, val in enumerate(ionput):
		INPUTINDEX = str(val[0])
		print(INPUTINDEX)
		OUTPUTJSON = str(val[1])
		os.system('cmsRun /home/cms-opendata/WorkingArea/CMSSW_5_3_32/src/json-cms/AnalysisFW/python/OpenDataTreeProducerOptimized_mcPAT_2011_cfg.py %s ' % INPUTINDEX+OUTPUTJSON)



