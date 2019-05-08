#python make_corrected_graph.py <in.dot> <outname>

import numpy as np;
import graph_tool.all as gt
import scipy as SP
import matplotlib.pyplot as plt
import sys
import threading
import multiprocessing
from multiprocessing import Process,Manager
import time
import subprocess

infile = sys.argv[1]
output_name = sys.argv[2]

print("Creating graph-tool graph...")

G = gt.load_graph(infile)

print("Graph creating done!")

vlen = len(G.get_vertices())

N = np.zeros(shape=(vlen,vlen))
diff = np.zeros((vlen,1))

ss = G.vertex_properties[("start_sum")]
es = G.vertex_properties[("end_sum")]
for vertex in G.vertices():
	i = int(vertex)
	diff[i] = int(float(ss[i]))-int(float(es[i]))

p = G.edge_properties[("sums")]
for edge in G.edges():
	s = int(edge.source())
	e = int(edge.target())
	N[s,e] = int(float(p[edge]))

A = gt.adjacency(G)

with open('log.txt', 'w') as file_w:
	file_w.write("A= (4)\n")
	file_w.write(str(A.todense().transpose()))
	file_w.write("\n")


print("Laplacian...")

L = gt.laplacian(G, deg='total', normalized=False, weight=None, index=None)
L = L-A.transpose()

print("Laplacian done!")


print("Lstsq...")

lamb = np.linalg.lstsq(L.todense(), diff)
lamb = lamb[0]

print("Lstsq done!")

one = np.ones((vlen,1))

print("R...")

R = np.multiply(np.dot(one,np.transpose(lamb))-np.dot(lamb,np.transpose(one)),A.todense().transpose());

print("R done!")

print("Q, Qdist, pi, P...")

Q = N+R;  # 2-dimensional stationary frequencies

Qdist = Q/(sum(Q).sum());  # 2-dimensional stationary distribution

pi = sum(Qdist);  # stationary distribution

P = np.transpose(np.divide(np.transpose(Qdist),pi));

print("Saving everything to nxgraph....")

#probcorr = G.new_edge_property("double")
probcorr = G.new_edge_property("string")
G.edge_properties["prob_corr"] = probcorr

def threadjob(i,j):
	vs = G.vertex(i)
	vt = G.vertex(j)
	if G.edge(vs,vt) == None:
		return
		
	probcorr[G.edge(vs,vt)] = '%f' % P[i,j]
	
	return

start_time = time.time()
count = 0
for i in range(0,vlen):
	for j in range(0,vlen):
		if P[i,j] != 0:
			threadd = threading.Thread(target=threadjob,args=(i,j,))
			threadd.daemon = True
			threadd.start()
				
			count = count + 1
			if(count%8 == 0):
				threadd.join()
			if(count%10000 == 0):
				elapsed_time = time.time() - start_time
				print count," ",elapsed_time

time.sleep(1)

print("Saving to files....")

fname = output_name + str(".graphml")
G.save(fname)

print G.edge_properties
idprop = G.vertex_properties[("vertex_name")]
probcorrprop = G.edge_properties[("prob_corr")]
probprop = G.edge_properties[("prob")]
sumprop = G.edge_properties[("sums")]

edge_name = output_name + str(".edgelist")
F = open(edge_name,"w") 
for e in G.edges():
    source = idprop[e.source()]
    target = idprop[e.target()]
    prob = probprop[e]
    probcorr = probcorrprop[e]
    sums = sumprop[e]
    F.write(str(source) + ' ' + str(target) + " {\'sums\': " + str(sums) + ", \'prob\': " + str(prob) + ", \'prob_corr\': " + str(probcorr) + "}\n")
F.close()

print("Everything done!")

