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
            n.selectCandidatesOnRand([x.nodeId for x in self.ns if x.nodeId != n.nodeId and abs(n.networkPos-x.networkPos)<self.reachLimit])

    def showNodes(self):
        for n in self.ns:
            print(n)

    # given a percentage of already failed node to check the failure percentage of the network
    def failNodes(self, p):
        failNodeCnt = int(p*self.nodeCnt)
        # the every possible combination of failed nodes
        possibleFailes = list(combinations([n.nodeId for n in self.ns], failNodeCnt))
        lst = 100*[0]
        for pf in possibleFailes:
            cfNodeCnt = 0
            for n in self.ns:
                if n.detectFailure(pf):
                    cfNodeCnt += 1
            r = cfNodeCnt/self.nodeCnt
            lst[int(r*100)-1] += 1
        lst = [x/sum(lst) for x in lst]
        plt.plot(lst)
        plt.xlabel("x% cascading failure")
        plt.ylabel("Probability")
        plt.savefig('graph.png')

def drawGraph(nw):
    g = nx.DiGraph()
    for n in nw.ns:
        g.add_node(n.nodeId)
        for s in n.sliceSet:
            g.add_edges_from([(n.nodeId,x) for x in s])
    nx.draw(g,with_labels=True)
    plt.show()
    plt.savefig('topology.png')

if __name__ == "__main__":
    nw = Network(50, 200)
    nw.giveCandidates()

    for n in nw.ns:
        n.selectSlice()

    nw.failNodes(0.04)
    #nw.showNodes()
    #drawGraph(nw)

