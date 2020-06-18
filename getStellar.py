import json
import pprint
pp = pprint.PrettyPrinter(indent=2)
from itertools import combinations

with open('testStellar3.json', 'r') as r:
    jff = json.loads(r.read())

def getCombinationSlices(quorumSet):
    if len(quorumSet) == 0:
        return []
    ls = []
    for i in range(len(quorumSet)):
        innerSet = []
        innerSet = getCombinationSlices(quorumSet[i]['innerQuorumSets'])
        ls += [list(x) for x in combinations(quorumSet[i]['validators']+innerSet, quorumSet[i]['threshold'])]
    return ls



jd = {}
for j in jff:
    jd[j['publicKey']] = getCombinationSlices([j['quorumSet']])
    print(len(jd[j['publicKey']]))
    
    with open('{}.json'.format(j['publicKey']),'w') as fp:
        fp.write(json.dumps(jd[j['publicKey']]))
    break

#pp.pprint(jd)