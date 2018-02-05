/*
 * Def.h
 *
 *  Created on: Jul 13, 2008
 *      Author: nmmkchow
 */

#ifndef DEF_H_
#define DEF_H_

//value '1' will activate each type of debug messages
#define DEBUG 0						//messages used for original debug
#define AGG_DEBUG 0					//messages used for aggregator
#define FLT_DEBUG 0					//messages used for VN request filtering
#define MIP_DEBUG 0					//messages used for Multiple InfP related implementation
#define HAGG_DEBUG 0

//#define MYCPP

#define MY_INFINITY         (INT_MAX / 2)
#define EPSILON             1E-6

#define SUCCESS             0
#define NODE_MAP_SUCCESS    0
#define EDGE_MAP_SUCCESS    0
#define BOTH_MAP_SUCCESS    0

#define COULD_NOT_OPEN_FILE -1

#define NOT_MAPPED_YET      -1

#define BOTH_MAP_FAILED     -1
#define NODE_MAP_FAILED     -2
#define EDGE_MAP_FAILED     -3

// node mapping methods
#define INVALID_NM_METHOD   -1
#define NM_GREEDY_BEST_FIT  0
#define NM_GREEDY_WORST_FIT 1
#define NM_RANDOM           2
#define NM_RANDOM_P2_BEST   3
#define NM_RANDOM_P2_WORST  4
#define NM_DETERMINISTIC    5
#define NM_RANDOMIZED       6

// VNode ordering
#define VNODE_ORDER_ASC   0
#define VNODE_ORDER_DESC  1
#define VNODE_ORDER_RAND  2

// edge mapping methods
#define INVALID_EM_METHOD   -1
#define EM_GREEDY_BEST_FIT  0
#define EM_GREEDY_WORST_FIT 1
#define EM_RANDOM           2
#define EM_RANDOM_P2_BEST   3
#define EM_RANDOM_P2_WORST  4
#define EM_MCF              5

// VNode ordering
#define VEDGE_ORDER_ASC   0
#define VEDGE_ORDER_DESC  1
#define VEDGE_ORDER_RAND  2

#define MAX_PATHS_TO_CONSIDER 1

#define LEN_FILENAME 128

#define MAX_CPU 100				//original 100
#define MAX_BW  100				//original 100

#define META_EDGE_BW  1000000

#define MAX_NODES_IN_A_SUBSTRATE  1000
#define MAX_NODES_IN_A_REQUEST    100

// topology types
#define TOPO_GENERAL  0
#define TOPO_STAR     1
#define TOPO_TREE     2
#define TOPO_HUBS     3

#define LINK_SPLITTABLE   1
#define LINK_UNSPLITTABLE 0

#define KSP_INPUT_FILENAME  "ksp_input.txt"
#define KSP_INPUT_FILENAME_HOP  "ksp_input_hop.txt"					//by Abdel for best of MIP selection

//value 1 activates pre-matching to filter VN requests and put substrates in ordered queue based on path bandwidth
//value 0 will make the system ignore pre-matching criteria and substrates will be taken without any priority order
#define PRE_MATCHING 0		//ADVEN resource discovery; value 1 will enable
#define V_AGG 0			//Histogramic aggregation; value 1 will enable

#define VECT_SIZE 5		//vector size

//Select Sorting criteria
//value 1 select; value 0 deselect
#define PATH_BW_SORT 0		//	Sorting InPs based on path bandwidths
#define AVE_CPUBW_SORT 1	//	Sorting based on (Ave_CPU * Ave_BW)


//value 1 for following constants activate monitoring
#define TIME_TREE 0			//time to construct minimum branching tree for the network graph
#define TIME_AGG 1			//time to calculate aggregation values
#define TIME_BEST 0			//time to select best substrate network for matching or rejecting request
#define COUNT_MESSAGE 0		//number of total messages passed in the network
#define TIME_MAP 0			//time to map requests to substrates. This measurements are to be taken with and without pre-matching

//Output Data Files to store monitoring results
#define T_READ_TREE_FILE "t_readings_tree.txt"			//time readings for tree construction
#define T_READ_AGG_FILE "t_readings_aggregator.txt"
#define COUNT_AGG_MSG_FILE "count_agg_msg.txt"
#define COUNT_CENT_MSG_FILE "count_cent_msg.txt"
#define T_READ_BEST_FILE "t_readings_bestInfP.txt"
#define MAPPING_REPORT_FILE "mapping_report.txt"




#define CNLM_MIP_MODEL_FILE "MIPs/CNLM-MIP.mod"
#define CNLM_LP_MODEL_FILE  "MIPs/CNLM-LP.mod"
#define CNLM_DATA_FILE      "MIPs/CNLM.dat"
#define CNLM_OUTPUT_FILE    "MIPs/CNLM.out"

#define MCF_MODEL_FILE     "MIPs/MCF.mod"
#define MCF_DATA_FILE      "MIPs/MCF.dat"
#define MCF_OUTPUT_FILE    "MIPs/MCF.out"

#endif /* DEF_H_ */
