
from ROOT import *
import os
from urlparse import urlparse
from AtlasStyle import *
import sys


myinput="interactive"
if (len(sys.argv) ==2):
   myinput = sys.argv[1]

#Parses URL

URL = raw_input('Input histogram URL: ')
parsedURL = urlparse(URL)
pathSplit = parsedURL.path.rsplit('/',3)
histogramSplit = parsedURL.path.split('/',5)
histogram = histogramSplit[5].rsplit('.',1)
histogramSplit2 = parsedURL.path.split('/',7)
histogram2 = histogramSplit2[7].rsplit('.',1)
layout = pathSplit[0].rsplit('_',1)

#Can specify the kind of root file you want:
print "Specify file with ref = reference for HAN tool, new = test for HAN tool, aft = after HAN tool"
File = raw_input('Input file type (ref, new, aft): ')
if File == 'ref':
    fileKind = 'output_ref_' + layout[1]    # reference histograms for HAN tool
elif File == 'new':
    fileKind = 'output_new_' + layout[1]   # test histograms for HAN tool
elif File == 'aft':
    fileKind = 'output_' + layout[1] + '_han'   # histogram after HAN tool

rootDownload = parsedURL.scheme + '://' + parsedURL.netloc + pathSplit[0]+ '/' + fileKind + '.root'
#os.system('wget '+ rootDownload) #Downloads root file
#print rootDownload
file  = TFile.Open(rootDownload) #load input file
SetAtlasStyle() #runs AtlasStyle
c = TCanvas('c', 'Canvas title', 0, 0, 800, 600) #create canvas
h = file.Get(histogram[0]) #grabs single histogram
histType = str(type(h))
#print "histtype: ", histType
#print "type: ", type(histType)
if histType == "<class '__main__.TH1D'>":
    h.Draw() 
else:
    h.Draw("colz")
#print "Object Info: ", type(h)
#h.Draw() #draws histogram
c.Print(histogram2[0]+".png") #names save file after histogram
c.Print(histogram2[0]+".eps") #names save file after histogram
#os.system('rm '+fileKind + '.root') #removes root file

if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"):
                         c.Close(); sys.exit(1);

