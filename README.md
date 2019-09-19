# ARL
    'Introduction of how to use it.'
    
    1. execute the file ./src/make.sh
generate the tool:configure-gen and the executable file ARLTool

    2. execute the file ./api/make.sh
    
  generate the library arltoolapi.a 
  
  then take ./api/hello.c as an example to modify the source code of the target program
  
    3. use configure-gen tool to generate the XML template of configureation
./configure-gen 

get configure.xml

    4. according the requirement of the user to modify this configuration file
label Switch default value is 1, represent enable the dual-checker mechanism

every job tuple correspond to an independent job

label job-ID is an interger value, it is unnique and assigned by the user for make a
distinction between different jobs.

parameter-N refers to the number of computer nodes

parameter-n refers to the number of processes

limit-time is a threshold preset by the user, when the actual execution time beyond this value

ARL would consider current execution occurred failure (program stall)

job-launchID, job-state here remained for the future.

    5.lastly, use src/autoReLaunch.sh to submit the job of ARL
    
yhbatch -N 1 -n 1 -p paratera ./autoReLaunch.sh
