./clean.sh
./bin/example_apps/pagerank file /data/joao/ligra/utils/my_edge_10M               

#LD_LIBRARY_PATH=/data/joao/graphchi-cpp/src/interception  LD_PRELOAD=src/interception/interc.so ltrace ./bin/example_apps/pagerank file /data/joao/ligra/utils/my_edge_1M > >(tee stdout.log) 2> >(tee stderr.log >&2)

