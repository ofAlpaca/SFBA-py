import random

NETWORK_SIZE = 1000

class Node():
    def __init__(self, _id):
        self.nodeId = _id
        self.value = None
        self.active = True
        self.networkPos = random.randint(0,NETWORK_SIZE)
        self.candidates = []
        self.sliceSet = []
        self.blockSet = []

    # add a node to the candidate
    def addCandidate(self, _node):
        self.candidates.append(_node)

    # given a sliceDight ex. "1010" which represents the first and the third candidate are selected as failed nodes.
    def isAllSlicesFail(self, sliceDigit):
        contain_ls = []
        for n in range(len(sliceDigit)):
            if sliceDigit[n] == '1':
                for s in range(len(self.sliceSet)):
                    if self.candidates[int(n)] in self.sliceSet[s] and s not in contain_ls:
                        contain_ls.append(s)
        return len(contain_ls) == len(self.sliceSet)

    # with given slices, find out the corresponding blocksing sets
    def findBlockSet(self):
        for b in range(1, 1<<len(self.candidates)):
            digit = bin(b)[2:].zfill(len(self.candidates))
            if self.isAllSlicesFail(digit):
                self.blockSet.append([self.candidates[x] for x in range(len(digit)) if digit[x] is '1'])

    # given a set of failed nodes, check if the node is blocked by them
    def isAllBlocksFail(self, fails):
        # if the node is already failed
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
        return "{}:\n\tcandidates:{}\n\tslices:{}\n\tblocks:{}".format(self.nodeId, self.candidates, self.sliceSet,self.blockSet)

    def selectCandidatesOnRand(self,_availables):
        csize = random.randint(1,len(_availables))
        self.candidates = random.sample(_availables,csize)

    # each FBA has its slice selection method
    def selectSlice(self):
        node_count = len(self.candidates)
        sliceCnt = random.randint(1,node_count)
        for j in range(sliceCnt):
            sliceSize = random.randint(1,node_count)
            self.sliceSet.append(random.sample(self.candidates, sliceSize))
        print("Node {} select slice({}) done".format(self.nodeId,len(self.sliceSet)))
        self.findBlockSet()
        print("Node {} find blocking set({}) done".format(self.nodeId, len(self.blockSet)))

    # given a set of failed node, check if the node is blocked by them
    def detectFailure(self, _fails):
        return super().isAllBlocksFail(_fails)





        
