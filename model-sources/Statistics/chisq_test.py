import copy
from scipy.stats import chisquare
import matplotlib.pyplot as plt
import sys

simulation = []
frequencies = {}
stationary = {}
results = []

with open("dict.txt", "rt") as stfile:
	for stline in stfile:
		stline = stline.replace("[ ", "").replace("]","") #just cleaning the line
		kvalues = stline.split(", ")
		for stdata in kvalues:
			stsplitted = stdata.split("=")
			if stsplitted[1] < 0 or stsplitted[1] > 1:
				stsplitted[1] = 0.1
			stationary[stsplitted[0]] = float(stsplitted[1])

with open(sys.argv[1], "rt") as distfile:
	for line in distfile:
		frequencies = copy.deepcopy(stationary)
		for key in frequencies:
			frequencies[key] = 0
		line = line.replace("[ ", "").replace("]\n","") #just cleaning the line
		keyvalues = line.split(",  ")
		for data in keyvalues:
			splitted = data.split("=")
			if frequencies.get(splitted[0]) != None:
				frequencies[splitted[0]] = float(splitted[1])/int(sys.argv[2])
		simulation.append(frequencies)
		frequencies = {}
		
for step in simulation:
	chisq, p = chisquare(step.values(), f_exp=stationary.values())
	results.append(chisq)

plt.plot(results)
plt.title('Simulation test statistic\n(' + sys.argv[1].split(".")[0] + ')')
plt.ylabel('Chi-squared test statistic')
plt.xlabel('Minutes (m)')
outfile = sys.argv[1].split(".")[0]+".pdf"
print "plt saved: " + outfile
plt.savefig(outfile, dpi=300, format='pdf')
plt.show()