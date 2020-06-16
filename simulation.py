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

    def giveReachable(self):
        for n in self.ns:
            for m in self.ns:
                if (n.nodeId != m.nodeId and abs(n.networkPos - m.networkPos) < self.reachLimit):
                    n.addReachable(m.nodeId)

    def showNodes(self):
        for n in self.ns:
            print(n)

    def failNodes(self, p):
        failNodeCnt = int(p*self.nodeCnt)
        possibleFailes = list(combinations([n.nodeId for n in self.ns], failNodeCnt))
        print(possibleFailes)
        for p in possibleFailes:
            cfNodeCnt = 0
            for n in self.ns:
                if n.detectFailure(p):
                    cfNodeCnt += 1
            print("Failure Rate:{}%".format(cfNodeCnt*100/self.nodeCnt))



if __name__ == "__main__":
    nw = Network(20, 300)
    nw.giveReachable()

    for n in nw.ns:
        n.selectSlice()

    nw.failNodes(0)
    #nw.showNodes()

    g = nx.DiGraph()
    for n in nw.ns:
        g.add_node(n.nodeId)
        for s in n.sliceSet:
            g.add_edges_from([(n.nodeId,x) for x in s])
    nx.draw(g,with_labels=True)
    plt.show()
    plt.savefig('topology.png')

