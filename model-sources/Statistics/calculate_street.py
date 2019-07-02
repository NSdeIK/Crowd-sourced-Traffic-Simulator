#!/usr/bin/python

import osmium
import numpy as np
import time
import numpy as np
import graph_tool.all as gt
import sys
import codecs

class WayNodeHandler(osmium.SimpleHandler):

    def __init__(self):
        osmium.SimpleHandler.__init__(self)
        #self.nodesd = dict()
        self.way_dict = dict()
		
    def set_Graph(self,gr):
        self.graph = gr
		
    def result_graph(self):
        return self.graph
		
    def set_nodes(self,n_d):
        self.nodesd = n_d
		
    def result_nodes(self):
        return self.nodesd
		
    def way(self, w):
        #print(w.id)

        e_pi = G.edge_properties["e_pi"]
        idprop = G.vertex_properties[("vertex_name")]

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

                sum = 0.0
                #for way_node in w.nodes:
                start_node_list = gt.find_vertex(G, idprop, w.nodes[0].ref)
                
                if(len(start_node_list) < 1):
                    return

                start_node = start_node_list[0]

                count = 0
                for i in range(1, len(w.nodes)):
                    end_node_list = gt.find_vertex(G, idprop, w.nodes[i].ref)
                        
                    if(len(end_node_list) < 1):
                        return

                    end_node = end_node_list[0]

                    edge = G.edge(start_node,end_node)

                    if(edge == None):
                        return

                    sum += float(e_pi[edge])

                    start_node = end_node

                    count = count + 1

                #print(count+1, len(w.nodes), w.id)
                    
                if 'name' in w.tags:
                    self.way_dict[w.tags['name']] = sum
                else:
                    self.way_dict[str("UNS "+str(w.id))] = sum

if __name__ == '__main__':

    graph_infile = sys.argv[1]
    map_infile = sys.argv[2]

    G = gt.load_graph(graph_infile)

    print(G.edge_properties)

    s = WayNodeHandler()
    s.set_Graph(G)
    s.apply_file(map_infile,locations=True)
    way_dict = s.way_dict
	

    file_str = "["
    for key, value in way_dict.iteritems():
        file_str += " " + key + "=" + str(value) + ","

    file_str = file_str[:-1] + "]"

    f = codecs.open("dict.txt","w", "utf-8")
    f.write( file_str )
    f.close()
	