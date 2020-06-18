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


class FBA_Node(Node):
    def __init__(self, _id):
        super().__init__(_id)

    def __str__(self):
        return "{}:\n\tcandidates:{}\n\tslices:{}".format(self.nodeId, self.candidates, self.sliceSet)

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

    # given a set of failed nodes, check if the node is blocked by them
    def isAllSlicesFail(self, _fails):
        # if the node is already failed
        if self.nodeId in _fails:
            self.active = False
            return True

        for s in self.sliceSet:
            if not any(x in s for x in _fails):
                self.active = True
                return False

        self.active = False
        return True



        
