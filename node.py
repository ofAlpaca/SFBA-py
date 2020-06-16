import random

NETWORK_SIZE = 1000

class Node():
    def __init__(self, _id):
        self.nodeId = _id
        self.value = None
        self.active = True
        self.networkPos = (random.randint(0,NETWORK_SIZE),random.randint(0,NETWORK_SIZE))
        self.reachables = []
        self.sliceSet = []

    def addSliceAsNodes(self,*_nodes):
        self.sliceSet.append(list(_nodes))

    def addSlice(self, _slice):
        self.sliceSet.append(_slice)
        
        