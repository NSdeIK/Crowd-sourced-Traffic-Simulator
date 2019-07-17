import copy
from scipy.stats import chisquare
import matplotlib.pyplot as plt
import sys

simulation = []
frequencies = {}
stationary = {}
results = []

print sys.float_info

with open("dict.txt", "rt") as stfile:
	for stline in stfile:
		stline = stline.replace("[ ", "").replace("]","") #just cleaning the line
		kvalues = stline.split(", ")
		for stdata in kvalues:
			stsplitted = stdata.split("=")
			stationary[stsplitted[0]] = float(stsplitted[1])

stationary_minvalue = min(stationary.values())

for key in stationary:
	stationary[key] += abs(stationary_minvalue) + 0.000000000000001

print ("Num of streets: " + str(len(stationary)))
print ("Smallest element: " + str(min(stationary.values())))
print ("Largest element: " + str(max(stationary.values())))
print ("Sum stationary: " + str(sum(stationary.values())))

with open(sys.argv[1], "rt") as distfile:
	for line in distfile:
		frequencies = copy.deepcopy(stationary)
		for key in frequencies:
			frequencies[key] = 0.
		line = line.replace("[ ", "").replace("]\n","") #just cleaning the line
		keyvalues = line.split(",  ")
		for data in keyvalues:
			splitted = data.split("=")
			if frequencies.get(splitted[0]) != None:
				frequencies[splitted[0]] = float(splitted[1])
		sum_cars = sum(frequencies.values())
		for key in frequencies:
			frequencies[key] = float(frequencies[key]) / float (sum_cars)

		simulation.append(frequencies)
		frequencies = {}

i = 0
for step in simulation:
	chisq, p = chisquare(step.values(), f_exp=stationary.values())
	if chisq < 1000:
		results.append(chisq)
	i += 1

results_filtered = results[:]

plt.plot(results_filtered)
plt.title('Simulation test statistic\n(' + sys.argv[1].split(".")[0] + ')')
plt.ylabel('Chi-squared test statistic')
plt.xlabel('Minutes (m)')
outfile = sys.argv[1].split(".")[0]+".pdf"
print "plt saved: " + outfile
plt.savefig(outfile, dpi=300, format='pdf')
plt.show()