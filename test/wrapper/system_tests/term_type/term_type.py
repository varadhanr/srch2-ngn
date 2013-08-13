#This case tests  whether the last character is a whitespace (which is transformed from "+")
#For example, for a query "q=trus+", we will take "trus" as a complete term.

#using: python term_type.py queriesAndResults.txt

import sys, urllib2, json, time, subprocess, os, commands, signal

port = '8081'

#make sure that start the engine up
def pingServer():
	info = 'curl -s http://localhost:' + port + '/search?q=Garden | grep -q results'
	while os.system(info) != 0:
		time.sleep(1)
		info = 'curl -s http://localhost:' + port + '/search?q=Garden | grep -q results'

#Function of checking the results
def checkResult(query, responseJson,resultValue):
    isPass=1
    if  len(responseJson) == len(resultValue):
        for i in range(0, len(resultValue)):
                #print response_json['results'][i]['record']['id']
            if responseJson[i]['record']['id'] !=  resultValue[i]:
                isPass=0
                print query+' test failed'
                print 'query results||given results'
                print 'number of results:'+str(len(responseJson))+'||'+str(len(resultValue))
                for i in range(0, len(responseJson)):
                    print responseJson[i]['record']['id']+'||'+resultValue[i]
                break
    else:
        isPass=0
        print query+' test failed'
        print 'query results||given results'
        print 'number of results:'+str(len(responseJson))+'||'+str(len(resultValue))
        maxLen = max(len(responseJson),len(resultValue))
        for i in range(0, maxLen):
            if i >= len(resultValue):
                 print responseJson[i]['record']['id']+'||'
            elif i >= len(responseJson):
                 print '  '+'||'+resultValue[i]
            else:
                 print responseJson[i]['record']['id']+'||'+resultValue[i]

    if isPass == 1:
        print  query+' test pass'

def testTermType(queriesAndResultsPath, conf, binary_path):
	#Start the engine server
	binary= binary_path + '/srch2-search-server'
	binary=binary+' --config-file='+ conf +' &'
	os.popen(binary)

	pingServer()

	#construct the query
	f_in = open(queriesAndResultsPath, 'r')
	for line in f_in:
	    #get the query keyword and results
	    value=line.split('||')
	    queryValue=value[0].split()
	    resultValue=(value[1]).split()
	    #construct the query
	    query='http://localhost:' + port + '/search?q='
	    for i in range(0, len(queryValue)):
		if i == (len(queryValue)-1) :
                    if queryValue[i] != '+': #if the last keyword is '+', we take it as space
		        query=query+queryValue[i]
		else:
		    query=query+queryValue[i]+'+'
	    
	    #do the query
	    response = urllib2.urlopen(query).read()
	    response_json = json.loads(response)

	    #check the result
	    checkResult(query, response_json['results'], resultValue )

	#get pid of srch2-search-server and kill the process
	s = commands.getoutput('ps aux | grep srch2-search-server')
	stat = s.split() 
	os.kill(int(stat[1]), signal.SIGUSR1)
	print '=============================='

if __name__ == '__main__':      
    #Path of the query file
    #each line like "trust||01c90b4effb2353742080000" ---- query||record_ids(results)
    binary_path = sys.argv[1]    
    queriesAndResultsPath = sys.argv[2]  
  
    testTermType(queriesAndResultsPath, './term_type/conf.ini', binary_path)
    print '--------Term type test  for attribute_based_search--------------'  
    testTermType(queriesAndResultsPath, './term_type/conf_for_attribute_based_search.ini', binary_path)
