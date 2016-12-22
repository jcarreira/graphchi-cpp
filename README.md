Fork from original GraphChi project


How to generate input?

For Ligra input can be generated with /data/joao/ligra/utils]% ./rMatGraph 100000000 rMat_100000000.

rmatGraph can be changed to write an edge format, which can be used by GraphChi.

Ramdisks can be created with sudo mount -t tmpfs -o size=40048M tmpfs /tmp/t


Running pagerank:
[joao@f1:/data/joao/ligra/utils]%   sudo ./bin/example_apps/pagerank_functional file /data_nvme/joao/rMat_10000000_edge niters 10

For ligra: [joao@f1:/data/joao/ligra/apps]% ./PageRank -s /data/joao/ligra/utils/rMat_10000000

