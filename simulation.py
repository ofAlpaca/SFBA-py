import random
import matplotlib as mpl
mpl.use('Agg')

import matplotlib.pyplot as plt
import networkx as nx

from node import Node

class Network():
    def __init__(self, node_count):
        self.ns = []
        for i in range(node_count):
            self.ns.append(Node(i))
            sliceCnt = random.randint(1,node_count)
            
            for j in range(sliceCnt):
                sliceEnd = random.randint(0,node_count-1)
                sliceStart = random.randint(0, sliceEnd)
                self.ns[i].addSlice([x for x in range(sliceStart,sliceEnd+1)])

    def showNodes(self):
        for n in self.ns:
            print("{}:{}".format(n.nodeId, n.sliceSet))

if __name__ == "__main__":
    nw = Network(10)
    nw.showNodes()
    g = nx.DiGraph()
    for n in nw.ns:
        g.add_node(n.nodeId)
        for s in n.sliceSet:
            g.add_edges_from([(n.nodeId,x) for x in s])
    nx.draw(g,with_labels=True)
    plt.show()
    plt.savefig('topology.png')

