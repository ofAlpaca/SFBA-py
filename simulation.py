import random
import matplotlib as mpl
import matplotlib.pyplot as plt
import networkx as nx

from itertools import combinations
from node import Node, FBA_Node

class Network():
    def __init__(self, _nodeCount, _reachLimit):
        self.nodeCnt = _nodeCount
        self.reachLimit = _reachLimit
        self.ns = []
        for i in range(self.nodeCnt):
            self.ns.append(FBA_Node(i))

    # given a node a set of candidate to communicate with
    def giveCandidates(self):
        for n in self.ns:
            n.selectCandidatesOnRand([x.nodeId for x in self.ns if x.nodeId != n.nodeId])

    def showNodes(self):
        for n in self.ns:
            print(n)

    # given a percentage of already failed node to check the failure percentage of the network
    def failNodes(self, p):
        failNodeCnt = int(p*self.nodeCnt)
        possibleFailes = list(combinations([n.nodeId for n in self.ns], failNodeCnt))
        # the every possible combination of failed nodes
        #print(possibleFailes)
        lst = 70*[0]
        for pf in possibleFailes:
            cfNodeCnt = 0
            for n in self.ns:
                if n.detectFailure(pf):
                    cfNodeCnt += 1
            r = cfNodeCnt/self.nodeCnt
            #print(r)
            lst[int(r*100)] += 1
        plt.plot(lst)
        plt.xlabel("{} crashes".format(p))
        plt.ylabel("Failure nodes")
        plt.savefig('graph.png')



if __name__ == "__main__":
    nw = Network(15, 300)
    nw.giveCandidates()

    for n in nw.ns:
        n.selectSlice()

    nw.failNodes(0.2)
    
    #nw.showNodes()
    '''
    g = nx.DiGraph()
    for n in nw.ns:
        g.add_node(n.nodeId)
        for s in n.sliceSet:
            g.add_edges_from([(n.nodeId,x) for x in s])
    nx.draw(g,with_labels=True)
    plt.show()
    plt.savefig('topology.png')
    '''

