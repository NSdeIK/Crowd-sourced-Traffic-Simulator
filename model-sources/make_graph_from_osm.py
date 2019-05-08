#!/usr/bin/python

import csv
import osmium
import scipy as sp
from scipy.spatial import distance
import networkx as nx
import numpy as np
from networkx.drawing.nx_pydot import write_dot
#from networkx.drawing.nx_pydot import write_adjlist
#from networkx.drawing.nx_pydot import write_graphml
#from scipy import io
import threading
import multiprocessing
from multiprocessing import Process,Manager
import time
import numpy as np
import pandas as pd
import sys
			
porto_dict = dict()
G = nx.DiGraph()
manager = Manager()
trajectories = manager.list()
#trajectories = list()
f = open("prob.txt","w")

class WayNodeHandler(osmium.SimpleHandler):

	def __init__(self):
		osmium.SimpleHandler.__init__(self)
		self.nodesd = dict()
		self.graph = nx.Graph
		
	def set_Graph(self,gr):
		self.graph = gr
		
	def result_graph(self):
		return self.graph
		
	def set_nodes(self,n_d):
		self.nodesd = n_d
		
	def result_nodes(self):
		return self.nodesd
		
	def way(self, w):
		if 'highway' in w.tags:
			if( w.tags['highway'] != 'pedestrian'
			and w.tags['highway'] != 'track'
			and w.tags['highway'] != 'bus_guideway'
			and w.tags['highway'] != 'raceway'
			and w.tags['highway'] != 'footway'
			and w.tags['highway'] != 'bridleway'
			and w.tags['highway'] != 'steps'
			and w.tags['highway'] != 'path'
			and w.tags['highway'] != 'cycleway'
			and w.tags['highway'] != 'construction'
			and w.tags['highway'] != 'proposed'):
				for way_node in w.nodes:
					self.nodesd[str(way_node.ref)] = way_node.location
					self.graph.add_node(str(way_node.ref),lon=way_node.location.lon,lat=way_node.location.lat,start_sum=0.0,end_sum=0.0)
				for x in range(0, len(w.nodes)):
					if(x != len(w.nodes)-1):
						dist = pow((w.nodes[x].location.lon-w.nodes[x+1].location.lon),2)+pow((w.nodes[x].location.lat-w.nodes[x+1].location.lat),2)
						self.graph.add_edge(str(w.nodes[x].ref),str(w.nodes[x+1].ref),weight=dist,sums=0,prob=0.0)
						self.graph.add_edge(str(w.nodes[x+1].ref),str(w.nodes[x].ref),weight=dist,sums=0,prob=0.0)


def interpolation(G,nodelist):
	resultlist = []
	partlist = []
	for x in range(0, len(nodelist)-1):
		try:
			path = nx.dijkstra_path(G,nodelist[x],nodelist[x+1])
			partlist.extend(path)
		except Exception:
			if(len(partlist) != 0):
				resultlist.append(partlist)
			partlist = list()
	resultlist.append(partlist)
	return resultlist
	
def matching_thread(coordinates):
	nodes = list()
	nodelist = list()
	for x in range(0, len(coordinates),2):
		lat = float(coordinates[x+1])
		lon = float(coordinates[x])
		distt = 100000000
		closest_node = str(0)
		

		for key, value in porto_dict.iteritems():

			dist1 = (pow((value.lon - lon),2)+pow((value.lat - lat),2))
			if(dist1 < distt):
				distt = dist1
				closest_node = str(key)
			nodes.append(closest_node)
				

		nodelist.append((nodes[0]))
		for x in range(1, len(nodes)):
			if nodes[x] != nodes[x-1]:
				nodelist.append(nodes[x])
		

		resultlist = interpolation(G, nodelist)
		

		for list_ in resultlist:
			trajectories.append(list_)
		
		return

lock = multiprocessing.Lock()

def sum_and_write(row, index):
	r = row[row > 0]
	s = r.div(r.sum())
	sr = str(index+":{")
	for i,v in s.items():
		sr+=str("["+str(i)+":"+str(v)+"]")
	sr += "}\n"
	with lock:
		f.write(sr)
	return

if __name__ == '__main__':
	
	mapfile = sys.argv[1]
	trainfile = sys.argv[2]
	print "Processing OSM"
	s = WayNodeHandler()
	s.set_nodes(porto_dict)
	s.set_Graph(G)
	s.apply_file(mapfile,locations=True)
	porto_dict = s.result_nodes()
	G = s.result_graph()
	
	print "Processing OSM DONE"

	trajectories2osmnodes = []
	count = 0
	
	print "Processing trajectories"
	
	start_time = time.time()
	with open(trainfile) as csvfile:
		reader = csv.DictReader(csvfile)
		for row in reader:
			line = row['polyline'].translate(None,'[]')
			coordinates = line.split(',')
			
			threadd = multiprocessing.Process(target=matching_thread,args=(coordinates,))
			threadd.daemon = True
			threadd.start()
			
			count = count + 1
			if(count%6 == 0):
				threadd.join()
			if(count%100 == 0):
				elapsed_time = time.time() - start_time
				print count," ",elapsed_time
	
	print "DONE. Now wait 100 sec for all to stop."
	time.sleep(2)			
	trajectories = np.asarray(trajectories)
	print trajectories.shape
	

	Gr=nx.OrderedDiGraph()
	

	start_time = time.time()
	def threadjob(list_):
		ll = len(list_)

		if not Gr.has_node(str(list_[0])):
			Gr.add_node(str(list_[0]),start_sum=0.0,end_sum=0.0)

		print ll
		for n in range(0, ll-1):
			if list_[n] == list_[n+1]:
				continue

			if not Gr.has_node(str(list_[n+1])):
				Gr.add_node(str(list_[n+1]),start_sum=0.0,end_sum=0.0)

			if not Gr.has_edge(str(list_[n]),str(list_[n+1])):
				Gr.add_edge(str(list_[n]),str(list_[n+1]),sums=0.0,prob=0.0)
			
			Gr[(list_[n])][(list_[n+1])]['sums']+=1.0

		if ll > 1:
			Gr.nodes[(list_[0])]['start_sum']+=1
			Gr.nodes[(list_[ll-1])]['end_sum']+=1
		return

	for list_ in trajectories:
		ll = len(list_)

		if ll < 1:
			continue

		#threadd = multiprocessing.Process(target=threadjob,args=(list_,))
		threadd = threading.Thread(target=threadjob,args=(list_,))
		threadd.daemon = True
		threadd.start()
			
		count = count + 1
		if(count%8 == 0):
			threadd.join()
		if(count%10000 == 0):
			elapsed_time = time.time() - start_time
			print count," ",elapsed_time

	for n, nbrdict in Gr.adjacency():
		summa = 0
		for key in nbrdict:
			summa += Gr[n][key]['sums']
			
		#print summa
		if summa != 0:
			for key in nbrdict:
				edgesum = Gr[n][key]['sums']
				Gr[n][key]['prob'] = edgesum/float(summa)
	
	####atmenet
	write_dot(Gr,'porto_prob_v2_connected.dot')