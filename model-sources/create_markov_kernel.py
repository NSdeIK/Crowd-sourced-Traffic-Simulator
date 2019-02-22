#!/usr/bin/python

import csv
import osmium
import scipy as sp
from scipy.spatial import distance
import networkx as nx
import numpy as np
from networkx.drawing.nx_pydot import write_dot
import threading
import multiprocessing
from multiprocessing import Process,Manager
import time
import numpy as np
import pandas as pd
			
porto_dict = dict()
G = nx.DiGraph()
manager = Manager()
trajectories = manager.list()

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
					self.graph.add_node(str(way_node.ref),lon=way_node.location.lon,lat=way_node.location.lat)
				for x in range(0, len(w.nodes)):
					if(x != len(w.nodes)-1):
						dist = pow((w.nodes[x].location.lon-w.nodes[x+1].location.lon),2)+pow((w.nodes[x].location.lat-w.nodes[x+1].location.lat),2)
						self.graph.add_edge(str(w.nodes[x].ref),str(w.nodes[x+1].ref),weight=dist,sums=0,prob=0)
						self.graph.add_edge(str(w.nodes[x+1].ref),str(w.nodes[x].ref),weight=dist,sums=0,prob=0)


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
	
	print "Processing OSM"
	s = WayNodeHandler()
	s.set_nodes(porto_dict)
	s.set_Graph(G)
	s.apply_file("porto.osm",locations=True)
	porto_dict = s.result_nodes()
	G = s.result_graph()
	
	print "Processing OSM DONE"
	
	count = 0
	
	print "Processing trajectories"
	
	start_time = time.time()
	with open('pkdd15-subset-bbox-train.csv') as csvfile:
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
	time.sleep(5)			
	trajectories = np.asarray(trajectories)
	print trajectories.shape
	
	c = 0
	c2 = 0
	for list_ in trajectories:
		for n in range(0, len(list_)-1):
			if list_[n] == list_[n+1]:
				c = c + 1
				continue
			c2 = c2 + 1
			G[(list_[n])][(list_[n+1])]['sums']=G[(list_[n])][(list_[n+1])]['sums']+1
	
	print c, c2
	for n, nbrdict in G.adjacency():
		summa = 0
		for key in nbrdict:
			summa += G[n][key]['sums']
			
		if summa != 0:
			for key in nbrdict:
				edgesum = G[n][key]['sums']
				G[n][key]['prob'] = edgesum/float(summa)

	write_dot(G,'porto_prob.dot')
	nx.write_adjlist(G, 'porto_adj.adjlist')
	nx.write_edgelist(G, 'porto_adj.edgelist')
	nx.write_graphml(G, 'porto_graphml.xsd')
