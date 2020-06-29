# pip3 install networkx
# sudo apt-get install python3-matplotlib
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import random
import SFBA
import numpy as np

nNodes = 1000
nCommittees = 5
nNodesInCommittee = 98
epoch = 10


def genCommittee():
	totalSelected = []
	commList = []

	for i in range(nCommittees):
		commList.append([])
		while len(commList[i]) < nNodesInCommittee:
			selected = random.randint(0, nNodes-1)
			if selected not in totalSelected:
				commList[i].append(selected)
				totalSelected.append(selected)

	return commList



def genEdge(G, commList):
	for node in range(nNodes):
		# Check if this node is member of committee
		idx = -1
		for i in range(nCommittees):
			if node in commList[i]:
				idx = i

		if idx != -1:
			# This node is in committee, connect to other nodes in same committee
			for comm in commList[idx]:
				if comm != node:
					G.add_edge(node, comm)
		else:
			# This node isn't member of committee, select a committee to connect to
			idx = random.randint(0, nCommittees-1)
			for comm in commList[idx]:
				G.add_edge(node, comm)





nonRepNodes = []
nodeHistory = []
for i in range(epoch):
	G = nx.DiGraph()
	G.add_nodes_from(range(nNodes))

	commList = genCommittee()
	genEdge(G, commList)

	# Parse graph to find strongly connected component (committee)
	network = SFBA.graph(nNodes)
	edges = G.edges()
	for edge in edges:
		network.addEdge(edge[0], edge[1])

	comms = network.findCommittees(nNodesInCommittee)

	print('Member of Committees:')
	numComm = 0
	for comm in comms:
		numComm += 1
		print str(numComm) + '. ',
		comm.printMember()
		print('')
	print('')

	selectedComm = comms[random.randint(0, numComm-1)]
	for j in range(selectedComm.numV):
		node = selectedComm.member[j]
		if node not in nonRepNodes:
			nonRepNodes.append(node)

	nodeHistory.append(float(len(nonRepNodes))*100/nNodes)

	# nx.draw(G, with_labels=True)
	# plt.savefig("SFBA_network_" + str(i) + ".png") # save as png
	# plt.show() # display

fig = plt.figure(figsize=(10,5), dpi=100, linewidth=2)

ax = fig.add_subplot(1,1,1)
ax.plot(range(1, epoch+1), nodeHistory, 's-', color='r', label='SFBA')
ax.plot(range(1, epoch+1), [98.0*100/nNodes]*epoch, 'o-', color='g', label='Stellar')
yticks = mtick.FormatStrFormatter("%.0f%%")
ax.yaxis.set_major_formatter(yticks)

plt.xlabel('Epoch')
plt.ylabel('The percentage of non-repeat quorum core')
plt.legend(loc = "best", fontsize=15)
plt.grid()
plt.show()

