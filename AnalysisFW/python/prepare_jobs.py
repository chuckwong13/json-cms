#!/sbin/python

import sys
import subprocess
import math
import os

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'



TAG = bcolors.FAIL + "[JOB_MANAGER]"



def make_job(cwd, info, template, bash_template, queue):

  dirname = cwd + "/job_" + str(info[0])
  bashFile = dirname + "/bash_wrap.sh"
  inputFiles = dirname + "/input.txt"
  confFile = dirname + "/data_cfg.py"
  outputFile = "output_" + str(info[0]) + ".json"
  outputFileROOT = "output_" + str(info[0]) + ".root"

  subprocess.call("mkdir " + dirname, stdout=subprocess.PIPE, shell=True)
  inputFile = open(inputFiles, 'wr')
  for i in info[1]:
      inputFile.write(i)
  inputFile.close()
    
  template1 = template.replace('NAMEOFINPUTFILE', "\"" + inputFiles + "\"") 
  template2 = template1.replace('NAMEOFOUTPUT', "\"" + outputFile + "\"") 
  template3 = template2.replace('NAMEROOTOFOUTPUTROOT', "\"" + outputFileROOT + "\"") 
  confFilef = open(confFile, 'wr')
  confFilef.write(template3)
  confFilef.close()

  bashFilef = open(bashFile, 'wr')
  bash_template1 = bash_template.replace("JOB_DIR", dirname)
  bashFilef.write(bash_template1)
  bashFilef.close()
  subprocess.call("chmod +x " +  bashFile, stdout=subprocess.PIPE, shell=True)

  execute = "bsub -q " + queue + " -J job1 " + bashFile
  return execute


if __name__ == "__main__":

  if not len(sys.argv) == 6:
      print TAG + bcolors.WARNING + " Usage: python prepare_jobs.py name_of_the_template name_of_the_bash_template name_of_the_samplefile number_of_jobs queue" + bcolors.ENDC
      sys.exit()


  name_of_template = sys.argv[1]
  name_of_bashtemplate = sys.argv[2]
  name_of_samplefile = sys.argv[3]
  number_of_jobs = int(sys.argv[4])  
  name_of_queue = sys.argv[5]  
  cwd = os.getcwd() 

  template = open(name_of_template).read()
  bash_template = open(name_of_bashtemplate).read()
  files = open(name_of_samplefile).readlines()
  number_of_files = len(files)
  
  if number_of_jobs > number_of_files:
      print TAG + bcolors.WARNING + " The number of jobs have to be equal or less than the number of files" + bcolors.ENDC
      sys.exit()

  job_info = []
  file_counter = 0
  files_per_job = int(math.floor(number_of_files/number_of_jobs))
  for i in range(0, number_of_jobs):
      list_of_files = []
      for j in range(file_counter, file_counter + files_per_job):
          list_of_files.append(files[j])
      job_info.append([i, list_of_files])
      file_counter = file_counter + files_per_job

  for i in range(0, number_of_files - number_of_jobs * files_per_job):
      job_info[i][1].append(files[file_counter])
      file_counter = file_counter + 1

  execute = []
  for i in range(0, number_of_jobs):
      print TAG + bcolors.OKBLUE + " Job", i, "will process " + bcolors.OKGREEN + str(len(job_info[i][1])) + bcolors.OKBLUE + " files" + bcolors.ENDC
      o = make_job(cwd, job_info[i], template, bash_template, name_of_queue)
      execute.append(o)
      #for j in range(0, len(job_info[i][1])):
      #    print bcolors.HEADER + job_info[i][1][j] + bcolors.ENDC

  run = open("run.sh", 'wr')
  run.write("#!/bin/bash"+'\n')
  for i in execute:
      run.write(i+'\n')
  run.close()

  print TAG
  print TAG
  print TAG
  print TAG + bcolors.OKBLUE + " Type " + bcolors.OKGREEN + "source run.sh " + bcolors.OKBLUE + "to submit the jobs" + bcolors.ENDC
           




