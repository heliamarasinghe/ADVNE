//============================================================================
// Name        : MySimINFOCOM2009.cpp
// Author      : Mosharaf K Chowdhury
// Version     :
// Copyright   :
// Description :
//============================================================================

#include "utility.h"
#include "def.h"
#include "simulator.h"


using namespace std;

//---------------v-------------- helis comment start --------------v-------------//
//SubstrateGraph SG0("sub.txt", 0);			//this was moved inside to the main()
//---------------^--------------- helis comment end ---------------^-------------//
vector<VNRequest> VNR;
int firstReq, lastReq, reqCount, subCount;
char reqFolderName[LEN_FILENAME];
char subFolderName[LEN_FILENAME];
char outputFolderName[LEN_FILENAME];

Simulator mySim;






int main(int argc, char *argv[]) {
  int i;
  char reqFileName[LEN_FILENAME], subFileName[LEN_FILENAME], outFileName[LEN_FILENAME];
  bool aOne = false, bOne = false;
  double mNS, aNS, mLS, aLS, sdNS, sdLS;
  double __MULT = 1.0;
  bool fullOrPartialSplit;

  int nodeMapMethod, edgeMapMethod;
  double nodeRev, edgeRev, totRev, nodeCost, edgeCost, totCost;

  vector<int> best_subs;
  vector<int> accepted_req;
  int subItr = 0;
  int mappedSubId;

  FILE * t_agg_file;			// file pointer for time to calculate aggregations file
  FILE * t_tree_file;			// file pointer for time to construct tree file
  FILE * agg_mc_file;			// file pointer for aggregation message count file
  FILE * cent_mc_file;			// file pointer for centralize message count file
  FILE * t_select_match_file;	// file pointer for time to select best InfP
  FILE * output_file;

  if (argc != 11) {
    cerr << "usage: SIM <rC> <rD> <oF> <nM> <eM> <fS> <lB> <MT>" << endl;
    cerr << "<rC>: total number of requests" << endl;
    cerr << "<rD>: directory containing the requests " << endl;
    cerr << "<sC>: total number of substrates" << endl;
    cerr << "<sD>: directory containing the substrates " << endl;
    cerr << "<oF>: output file to dump the results" << endl;
    cerr << "<nM>: node mapping method (1: GREEDY. 5: D-ViNE 6: R-ViNE)" << endl;
    cerr << "<eM>: edge mapping method (1: GREEDY. 5: MCF)" << endl;
    cerr << "<fS>: whether to ignore (1) or respect(0) VNR's splitting choice [eM must be 5]" << endl;
    cerr << "<lB>: load balancing (1: alpha = beta = 1. 0: alpha = beta = residue)" << endl;
    cerr << "<MT>: multiplier in the revenue and cost functions" << endl;
    exit(1);
  }

  reqCount = atoi(argv[1]); // total number of requests
  strcpy(reqFolderName, argv[2]); // folder containing the requests
  subCount = atoi(argv[3]); // total number of substrate networks
  strcpy(subFolderName, argv[4]); // folder containing the substrates
  strcpy(outputFolderName, argv[5]);  // where to save output
  nodeMapMethod = atoi(argv[6]);  // 1: GREEDY. 5: D-ViNE 6: R-ViNE
  edgeMapMethod = atoi(argv[7]);  // 1: GREEDY. 5: MCF
  fullOrPartialSplit = atoi(argv[8]); // true: ignore VNR's choice
  aOne = bOne = atoi(argv[9]);  // true: try to load balance
  __MULT = atof(argv[10]); // cpu vs bw weighting value


  int subFldItr = 10;
int map_status = 0;
int reqFldItr = 10;

//0 = initial and depart event  <- not useful
//1 = success
//2 = rejected by pre-matching
//3 = rejected by node mapping
//4 = rejected by edge mapping

for(subFldItr = 10; subFldItr<=100; subFldItr += 10){				//Substrate folder iterator
	//reqFldItr = subFldItr/2;
	sprintf(subFolderName, "substrates/%d-nodes",subFldItr);		//subFolderName is overwritten here for simulations
	//sprintf(reqFolderName, "requests/%d-nodes",reqFldItr);		//reqFolderName is overwritten here for simulations

	sprintf(outFileName, "%s/out-%d-nodes.txt", outputFolderName, subFldItr);
	output_file = fopen(outFileName, "w+");
	fprintf(output_file,"Simulation results for substrates contain %d-nodes\n",subFldItr);
	fprintf(output_file,"state; 1-success, 2-rejected by pre-matching, 3-rejected by node mapping, 4-rejected by edge mapping\n\n");
	fprintf(output_file,"req\tstate\t maptime\n");

	if(TIME_TREE){t_tree_file = fopen(T_READ_TREE_FILE, "a+"); fprintf(t_tree_file, "\n\n%d-nodes\n",subFldItr); fclose(t_tree_file);}
	if(TIME_AGG){t_agg_file = fopen(T_READ_AGG_FILE, "a+");fprintf(t_agg_file, "\n\n%d-nodes\n",subFldItr);}


	  if(COUNT_MESSAGE){
		  agg_mc_file = fopen (COUNT_AGG_MSG_FILE, "a+"); fprintf(agg_mc_file, "\n\n%d-nodes\n",subFldItr);
		  cent_mc_file = fopen (COUNT_CENT_MSG_FILE, "a+"); fprintf(cent_mc_file, "\n\n%d-nodes\n",subFldItr);
	  }




//for(int timeItr = 0; timeItr<5; timeItr++){		//to run multiple times for accurate time measurements
  timespec aggStart, aggStop, tArriveStart, tArriveEnd;
  double aggTime = 0., arriveTime = 0.;

  bool acceptableVnRequest = false;

  if(TIME_TREE){t_tree_file = fopen(T_READ_TREE_FILE, "a+"); fprintf(t_tree_file, "\n"); fclose(t_tree_file);}
  if(TIME_AGG)fprintf(t_agg_file, "\n");

  if(COUNT_MESSAGE){
	   fprintf(agg_mc_file, "\n");
	  fprintf(cent_mc_file, "\n");
  }


  vector<SubstrateGraph *> substrates;

  // read all the substrates from Folder
  for (i = 0; i < subCount; i++) {
    sprintf(subFileName, "%s/sub%d.txt", subFolderName, i);
    //cout<<"subfile = "<<subFileName<<endl;
    // save the substrates in &SubstrateGraph type vector, substrates
    substrates.push_back(new SubstrateGraph(subFileName, i));

    if(TIME_AGG)clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &aggStart); // CLOCK_REALTIME START
	if(V_AGG) substrates[i]->vectAgg();
	else if(PRE_MATCHING) substrates[i]->aggregator();

    if(TIME_AGG){
    	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &aggStop); //CLOCK_REALTIME STOP
    	aggTime = exectime(aggStart, aggStop);
    	//cout<<"Time taken for agg = "<<aggTime<<endl;
    	fprintf(t_agg_file, "%3.6f\t",aggTime);
    }

    if(COUNT_MESSAGE){
    	fprintf(agg_mc_file, "%d\t",substrates[i]->agg_msg_count);
    	fprintf(cent_mc_file, "%d\t",substrates[i]->cent_msg_count);
    }
  }




 /* FILE *outputFP = fopen(outputFileName, "w");
  if (outputFP == NULL) {
    cout << "failed to open file: " << outputFileName << endl;
    return COULD_NOT_OPEN_FILE;
  }

  for (i = 1; i < argc; i++) {
    fprintf(outputFP, "%s ", argv[i]);
  }
  fprintf(outputFP, "\n");*/

  vector<VNRequest>::iterator VNRIter;
  int curVNR, mappingResult;
  bool requestAccepted = false;

  int vect_size = 0, preMatchingFailCount = 0, nodeMapFailCount = 0, edgeMapFailCount = 0, totalMapFailCount = 0;

  srand((unsigned) time(NULL)); // initialize random number generator


  // read all the requests from file
  for (i = 0; i < reqCount; i++) {
    sprintf(reqFileName, "%s/req%d.txt", reqFolderName, i);

    // save the request in VNRequest type vector, VNR
    VNR.push_back(VNRequest(reqFileName, i));

    // create the arrival event using priority_queue, PQ which store VN request
    // with least keep alive time in front of an Event type vector
    mySim.PQ.push(Event(EVENT_ARRIVE, VNR[i].time, i));
  }
  if(PRE_MATCHING){
	  cout<<"  Aggregation values Before Mapping"<<endl;
	  for(int i = 0; i<subCount; i++) cout << "\tSG"<<i<<" CPU = "<<substrates[i]->tot_cpu_agg<<"  \tBW = "<<substrates[i]->tot_bw_agg<<endl;
  }


  MonitorMultiSub Monitor;

// simulate all the events
  while (!mySim.empty()){		// to carry-out the VN mapping, use  "while (!mySim.empty()) {"

	  int mapped_sub_id = -1;
    const Event &curEvent = mySim.top();
    curVNR = curEvent.index;
    requestAccepted = false;


    //cout <<"Request = "<< curVNR << " Time = " << curEvent.time << " Type = " << curEvent.type << endl;

    if (curEvent.type == EVENT_ARRIVE) { // handle arrival of a new VN request

    	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tArriveStart); // CLOCK_REALTIME START

    	cout <<"@ EVENT_ARRIVE: Request = "<< curVNR << " Time = " << curEvent.time << " Type = " << curEvent.type << endl;



    	//TOuDO: MIP- Programming lines to select best substrate will come here



    		best_subs =  Monitor.getBestSub(substrates, VNR[curVNR], &acceptableVnRequest);




    	cout<<"  substrate order = | ";
    	for(int jk = 0; jk<best_subs.size();jk++){
    		cout<<best_subs[jk]<<" | ";
    	}
    	cout<<'\n';

    	if(best_subs[0] > -1) acceptableVnRequest=true;
    	else acceptableVnRequest=false;


if(acceptableVnRequest){


	LABEL_NXT_SUB:

      switch(nodeMapMethod) {
      case NM_GREEDY_WORST_FIT:
    	  //mappingResult = SG0.mapNodes(VNR[curVNR], VNODE_ORDER_ASC, NM_GREEDY_WORST_FIT);
    	  //TODO: MIP- need to pass SG0# as a parameter to this function
        mappingResult = substrates[best_subs[subItr]]->mapNodes(VNR[curVNR], VNODE_ORDER_ASC, NM_GREEDY_WORST_FIT);
        break;
      case NM_DETERMINISTIC:
        mappingResult = substrates[best_subs[subItr]]->mapNodes_ViNE(VNR[curVNR], NM_DETERMINISTIC, aOne, bOne);
        //mappingResult = SG0.mapNodes_ViNE_v2(VNR[curVNR], NM_DETERMINISTIC, aOne, bOne);
        break;
      case NM_RANDOMIZED:
        mappingResult = substrates[best_subs[subItr]]->mapNodes_ViNE(VNR[curVNR], NM_RANDOMIZED, aOne, bOne);
        //mappingResult = SG0.mapNodes_ViNE_v2(VNR[curVNR], NM_RANDOMIZED, aOne, bOne);
        break;
      default:
        cerr << "invalid node mapping method" << endl;
        exit(1);
        break;
      }




      // node mapping failed, don't admit

      if (mappingResult != NODE_MAP_SUCCESS) {
    	  cout<<"  Node mapping failed on "<<best_subs[subItr]<<endl<<endl;
    	  if(subItr+1 >= best_subs.size()){
				nodeMapFailCount++;
				totalMapFailCount++;
				best_subs.clear();
				best_subs[0]=-1;
				subItr = 0;
				map_status = 3;
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tArriveEnd); //CLOCK_REALTIME STOP
				arriveTime = exectime(tArriveStart, tArriveEnd);
				fprintf(output_file,  "\n%d   \t%d   \t%3.6f",curVNR, map_status, arriveTime);
				goto LABEL_MAP_FAILED;
    	  }
    	  else{
    		  subItr++;
    		  goto LABEL_NXT_SUB;
    	  }
      }
      else cout<<"  Node mapping successful on "<<best_subs[subItr]<<" \n";


      if (edgeMapMethod == EM_GREEDY_WORST_FIT) {
    	  //mappingResult = SG0.mapEdges(VNR[curVNR], VEDGE_ORDER_ASC, EM_GREEDY_WORST_FIT);
    	  //TODO: MIP- need to pass SG0# as a parameter to this function
    	  mapped_sub_id = substrates[best_subs[subItr]]->mapEdges(VNR[curVNR], VEDGE_ORDER_ASC, EM_GREEDY_WORST_FIT);
      }
      else if (edgeMapMethod == EM_MCF) {
        if (fullOrPartialSplit || VNR[curVNR].split) {
        	mapped_sub_id = substrates[best_subs[subItr]]->mapEdges_ViNE(VNR[curVNR], aOne, bOne);
        }
        else {
        	mapped_sub_id = substrates[best_subs[subItr]]->mapEdges(VNR[curVNR], VEDGE_ORDER_ASC, EM_GREEDY_WORST_FIT);
        }
      }
      else {
        cerr << "invalid edge mapping method" << endl;
        exit(1);
      }

      //mappingResult = SG0.mapEdges_ViNE(VNR[curVNR], aOne, bOne);
      //mappingResult = SG0.mapEdges(VNR[curVNR], VEDGE_ORDER_ASC, EM_GREEDY_WORST_FIT);

      // edge mapping failed, don't admit
      //cout<<"mapped_sub_id = "<<mapped_sub_id<<'\n';
      if (mapped_sub_id < EDGE_MAP_SUCCESS){
    	  cout<<"  Edge mapping failed on "<<best_subs[subItr]<<endl<<endl;
    	  if(subItr+1 >= best_subs.size()){
				edgeMapFailCount++;
				totalMapFailCount++;
				map_status = 4;
				best_subs.clear();
				best_subs[0]=-1;
				subItr = 0;
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tArriveEnd); //CLOCK_REALTIME STOP
				arriveTime = exectime(tArriveStart, tArriveEnd);
				fprintf(output_file,  "\n%d   \t%d   \t%3.6f",curVNR, map_status, arriveTime);
				goto LABEL_MAP_FAILED;
    	  }
		else{
         	  subItr++;
         	  goto LABEL_NXT_SUB;
           }
      }
      else cout<<"  Edge mapping successful on "<<best_subs[subItr]<<" \n";



      				//when mapping success, re-initialize iterator
      map_status = 1;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tArriveEnd); //CLOCK_REALTIME STOP
      arriveTime = exectime(tArriveStart, tArriveEnd);
      fprintf(output_file,  "\n%d   \t%d   \t%3.6f",curVNR, map_status, arriveTime);
      // bookkeeping
      requestAccepted = true;
      cout<<"  Embedding request "<<curVNR<<" on substrate "<<best_subs[subItr]<<" \n";

      substrates[best_subs[subItr]]->addVNMapping(VNR[curVNR]);
      mappedSubId = VNR[curVNR].mapped_sub_id;
      cout<<"  VN REQUEST [ "<<curVNR<<" ]  SUCCESSFULLY EMBEDDED IN SUBSTRATE [ "<<mappedSubId<<" ]"<<endl;
      cout<<"  VNR[curVNR].mapped_sub_id = "<<mappedSubId<<'\n';
      //substrates[VNR[curVNR].mapped_sub_id]->aggregator();
      if(V_AGG) substrates[mappedSubId]->vectAgg();
      else if(PRE_MATCHING){
    	  substrates[mappedSubId]->aggregator();
    	  cout <<"  SG"<<mappedSubId<<" cpu agg = " << substrates[mappedSubId]->tot_cpu_agg<<" bw agg =" << substrates[mappedSubId]->tot_bw_agg <<endl<<endl;
      }
      subItr = 0;

      accepted_req.push_back(curVNR);
      // create the departure event after admitting a VN request
      mySim.PQ.push(Event(EVENT_DEPART, VNR[curVNR].time + VNR[curVNR].duration,curVNR));

    }
else{
	map_status = 2;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tArriveEnd); //CLOCK_REALTIME STOP
	arriveTime = exectime(tArriveStart, tArriveEnd);
	fprintf(output_file,  "\n%d   \t%d   \t%3.6f",curVNR, map_status, arriveTime);
	totalMapFailCount++;
	preMatchingFailCount++;
	cout<<"  VN Request "<<curVNR<<" is rejected by pre-matching \n"<<endl;
	goto LABEL_MAP_FAILED;

}




    }																	// end of VN arrival handling

    else if (curEvent.type == EVENT_DEPART) { 							// handle departure event
      // remove from the substrate graph

    	cout <<"\n@ EVENT_DEPART: Request = "<< curVNR << " Time = " << curEvent.time << " Type = " << curEvent.type << endl;
    	cout<<"  Mapped Substrate = "<<mappedSubId<<endl;
    	substrates[mappedSubId]->removeVNMapping(VNR[curVNR]);
    	if(V_AGG) substrates[mappedSubId]->vectAgg();
    	else if(PRE_MATCHING){
    		substrates[mappedSubId]->aggregator();
			cout << "  SG"<<mappedSubId<<" cpu agg = " << substrates[mappedSubId]->tot_cpu_agg<<" bw agg =" << substrates[mappedSubId]->tot_bw_agg <<endl;
    	}
    	map_status = 0;
    }																	//end of VN departure handling

    else {
    	cerr<<"ambiguous event type\n";
    }

    LABEL_MAP_FAILED:


if(best_subs[0] > -1 && mapped_sub_id > -1){
    totRev = getRevenue(VNR[curVNR], __MULT, nodeRev, edgeRev);

    totCost = getCost(VNR[curVNR], *substrates[VNR[curVNR].mapped_sub_id], __MULT, nodeCost, edgeCost, aOne, bOne);

    getDifferentStress(*substrates[VNR[curVNR].mapped_sub_id], mNS, aNS, mLS, aLS, sdNS, sdLS);
    //cout<<"after getDifferentStress\n";
    //fprintf(outputFP, "%4d %6d %6d %d %d %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %10.4lf %4d\n",
    //    curEvent.index, curEvent.time, VNR[curEvent.index].duration, curEvent.type, requestAccepted, nodeRev, edgeRev, totRev, nodeCost, edgeCost, totCost, mNS, aNS, mLS, aLS, sdNS, sdLS, VNR[curVNR].mapped_sub_id);

}
//departTime = exectime(tDepartStart, tDepartEnd);

    // pop the request
    mySim.pop();
  }		//end of while loop


  V_AGG ? vect_size = VECT_SIZE : vect_size = 0;




  fprintf(output_file,  "\n"
		  "\nVECT_SIZE:  \t%d"
		  "\nnum of sub: \t%d"
		  "\nnod per sub:\t%d"
		  "\ntot req:    \t%d"
		  "\nsuccess:    \t%d"
		  "\nPM reject:  \t%d"
		  "\nnode Fail:  \t%d"
		  "\nedge Fail:  \t%d"
		  "\ntot Fail:   \t%d",vect_size, subCount, subFldItr, reqCount, accepted_req.size(), preMatchingFailCount, nodeMapFailCount, edgeMapFailCount, totalMapFailCount);
  fclose(output_file);
  cout << "\n\n\tSIMULATION REPORT\n";
  cout << "  VECT_SIZE\t\t: " <<vect_size << endl;
  cout << "  Number of Requests\t: " <<reqCount << endl;
  cout << "  Number of Substrates\t: " <<subCount << endl;
  cout << "  Mapping Successful\t: " <<accepted_req.size() << endl;
  cout << "  Successful requests\t: ";
  for(unsigned int i = 0; i<accepted_req.size();i++){
	  cout<<accepted_req[i]<<"|";
  }
  cout<<endl;
  cout << "  Pre-matching Reject\t: " << preMatchingFailCount << endl;
  cout << "  Node Mapping Failed\t: " << nodeMapFailCount << endl;
  cout << "  Edge Mapping Failed\t: " << edgeMapFailCount << endl;
  cout << "  Total Mapping Failed\t: " << totalMapFailCount << endl;




  //fclose(outputFP);


  //cout<<"clearing allocated memory and re-initializing variables";
  VNR.clear();
  best_subs.clear();
  aOne = false; bOne = false;
  __MULT = 1.0;
  subItr = 0;
  for (int k = 0; k < subCount; k++) {
	  delete substrates[k];
  }
  substrates.clear();
  accepted_req.clear();
//}		//end of timeItr
if(TIME_AGG)fclose(t_agg_file);
if(COUNT_MESSAGE){fclose(agg_mc_file); fclose(cent_mc_file); }
if(TIME_BEST)fclose(t_select_match_file);

}		//end of subFldItr




  return 0;
}
