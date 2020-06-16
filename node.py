import random

NETWORK_SIZE = 1000

class Node():
    def __init__(self, _id):
        self.nodeId = _id
        self.value = None
        self.active = True
        self.networkPos = random.randint(0,NETWORK_SIZE)
        self.reachables = []
        self.sliceSet = []
        self.blockSet = []

    def addSliceAsNodes(self,*_nodes):
        self.sliceSet.append(list(_nodes))

    def addSlice(self, _slice):
        self.sliceSet.append(_slice)

    def addReachable(self, _node):
        self.reachables.append(_node)
    
    def addReachables(self, _reach_ls):
        self.reachables = self.reachables + _reach_ls

    def isAllSlicesFail(self, sliceDigit):
        contain_ls = []
        for n in range(len(sliceDigit)):
            if sliceDigit[n] == '1':
                for s in range(len(self.sliceSet)):
                    if self.reachables[int(n)] in self.sliceSet[s] and s not in contain_ls:
                        contain_ls.append(s)
        return len(contain_ls) == len(self.sliceSet)

    def findBlockSet(self):
        for b in range(1, 1<<len(self.reachables)):
            digit = bin(b)[2:].zfill(len(self.reachables))
            if self.isAllSlicesFail(digit):
                self.blockSet.append([self.reachables[x] for x in range(len(digit)) if digit[x] is '1'])

    def isAllBlocksFail(self, fails):
        if self.nodeId in fails:
            self.active = False
            return True

        blockingSets = self.blockSet.copy()
        bl = len(blockingSets)
        for b in range(bl-1,-1,-1):
            if any(x in blockingSets[b] for x in fails):
                blockingSets.pop()
        
        if len(blockingSets) == 0:
            self.active = False
            return True
        else:
            self.active = True
            return False



class FBA_Node(Node):
    def __init__(self, _id):
        super().__init__(_id)

    def __str__(self):
        return "{}({}):\n\tslices:{}\n\tblocks:{}".format(self.nodeId, self.active, self.sliceSet,self.blockSet)

    def selectSlice(self):
        node_count = len(self.reachables)
        sliceCnt = random.randint(1,node_count)
        for j in range(sliceCnt):
            sliceSize = random.randint(1,node_count)
            self.sliceSet.append(random.sample(self.reachables, sliceSize))

        self.findBlockSet()

    def detectFailure(self, fails):
        return super().isAllBlocksFail(fails)





        