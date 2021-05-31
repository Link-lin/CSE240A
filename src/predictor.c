//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Link Lin";
const char *studentID = "A13817414";
const char *email = "yul065@ucsd.edu";
const char *studentName2 = "Tianyi Shan";
const char *studentID2 = "A53313140";
const char *email2 = "tshan@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;



//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t *gshareBHT; //gshare Branch Prediction Table
uint32_t gHistory;

uint8_t *globalPT;    // Global prediction table
uint32_t *localHT;    // Local history table
uint8_t *localPT;     // Local prediction table
uint8_t *choiceTable; // Choice prediction table

uint8_t localResult;  // Local prediction result
uint8_t globalResult; // Global prediction result

int32_t ** customWT; //custom weight table 
int32_t threshold;  // [1.93h + 14]
int32_t y; 



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t convert_state_to_result(uint8_t st){
  // Changing the order results in different number???
  if(st == 1 || st == 0){
    return 0;
  }
  else{
    return 1;
  }
}

// Initialize the predictor
//
void init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType){
    case STATIC:
      break;
    case GSHARE:{
      gHistory = NOTTAKEN;
      // Set up BHT table
      int gTableSize = 1 << ghistoryBits;
      // printf("TableSize: %d\n",tableSize);
      // Initialize the BHT to WN
      gshareBHT = malloc(gTableSize * sizeof(uint8_t));
      memset(gshareBHT, WN, gTableSize * sizeof(uint8_t));
      break;
    }
    case TOURNAMENT:{
      gHistory = NOTTAKEN;
      // Set up BHT talbe
      int gTableSize = 1 << ghistoryBits;
      globalPT= malloc(gTableSize* sizeof(uint8_t));
      memset(globalPT, WN, gTableSize * sizeof(uint8_t));

      // Set up local History table 
      int LHTSize = 1 << pcIndexBits;
      localHT = malloc(LHTSize * sizeof(uint32_t));
      memset(localHT, 0, LHTSize * sizeof(uint32_t));
      

      // Set up local prediction table
      int LPTSize = 1 << lhistoryBits;
      localPT = malloc(LPTSize * sizeof(uint8_t));
      memset(localPT, WN, LPTSize * sizeof(uint8_t));
      

      // Set up choice prediction table
      int CPTSize = 1 << ghistoryBits;
      choiceTable = malloc(CPTSize * sizeof(uint8_t));
      memset(choiceTable, WG, CPTSize*sizeof(uint8_t));
      break;
    }
    case CUSTOM:{
      gHistory = NOTTAKEN;
      // Set up weight table
      int gTableSize = 1 << ghistoryBits;

      customWT = malloc(gTableSize * sizeof(int32_t *));
      for (int i = 0; i < gTableSize; i++ ){
        customWT[i] = malloc(sizeof(int) * ghistoryBits);
      }
      for(int i=0; i < gTableSize; i++) {
        for(int j=0; j < ghistoryBits; j++) {
          customWT[i][j] = 0; // Init weight to 0 
        }
      }

      //set up threshold 
      threshold = floor(1.93 * ghistoryBits + 14);

      break;
    }
    default:
      break;
  }
}

// This is the method for gshare predict
// which is characterizing by XORing the global history register with the
// lower bits(same length as the global history) of the branch's address
// result is either TAKEN/NOTTAKEN
uint8_t gshare_predict(uint32_t pc)
{
  // Get the index into the BHT
  //uint32_t index = gHistory ^ (pc & ((1 << ghistoryBits) - 1));
  uint32_t index = (gHistory ^ pc) & ((1 << ghistoryBits) - 1);
  // previous state of the branch (can be SN, WN, WT, ST)
  uint8_t p_state = gshareBHT[index];
  //printf(" gHistory: %d ",gHistory);
  //printf(" p_state: %d ",p_state);
  return convert_state_to_result(p_state);
}

// This is the method for tournament predict following ALPHA 21264
uint8_t tournament_predict(uint32_t pc){
    // get local history predict
    uint32_t LHTIndex = pc & ((1 << pcIndexBits) -1);
    // would be looking silimiar to  001101 used as index on Local Prediction Table
    uint32_t LPTIndex = localHT[LHTIndex] & ((1 << lhistoryBits)-1);

    uint8_t localPredict = localPT[LPTIndex];
    localResult = convert_state_to_result(localPredict);
    // get global history predict
    uint32_t index = gHistory & ((1 << ghistoryBits) - 1);
    uint8_t globalPredict = globalPT[index];  
    globalResult = convert_state_to_result(globalPredict);
    // Choose based on choice prediction table
    uint8_t choicePredict = choiceTable[index];
    uint8_t selectResult = convert_state_to_result(choicePredict);
    //uint8_t selectResult = (choicePredict == SL || choicePredict == WL) ? SELECTL : SELECTG;
    //printf("localPredict: %d, localRe: %d, globalPredict: %d, globalRe: %d, selectResult: %d\n",localPredict, localResult,globalPredict, globalResult, selectResult);
    // TODO changing the order may get different result
    return selectResult ==SELECTG ? globalResult : localResult;
}

uint8_t custom_predict(uint32_t pc){
  // Get the index into the weight table
  uint32_t index = (gHistory ^ pc) & ((1 << ghistoryBits) - 1);
  // weight vector of this histroy pattern 
  // int32_t * weight_vector = &customWT[index];
  y = 1; //training result + bias weight = 1
  uint32_t the_history = gHistory;

  for(int j=0; j < ghistoryBits; j++) {
    if (the_history & 0x01){
      // printf("1 \n");
      y += customWT[index][j] * 1;
    }else{
      // printf("0 \n");
      y += customWT[index][j] * 0;
    }
    the_history = the_history >> 1;
  }

  return (y < 0) ? 0 : 1;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  //printf("pc: %d ", pc);

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
    // Everything in the gshareBHT should be 1 at the point
    // for(int i = 0; i < tableSize; i++){
    //   printf("%d\n", gshareBHT[i]);
    // }
    return gshare_predict(pc);
  case TOURNAMENT:
    return tournament_predict(pc);
  case CUSTOM:
    return custom_predict(pc);
  default:
    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:{
      // update G-share based on State machine
      // Get the previous state
      uint32_t index = (gHistory ^ pc) & ((1 << ghistoryBits) - 1);
      //printf("test: %d ", (pc & (1 << ghistoryBits)-1));
      //printf(" index : %d", index);
      //printf(" outcome: %d\n", outcome);
      uint8_t *p_state = &gshareBHT[index];
      if (outcome && *p_state != ST) {
        *p_state += 1;
      }
      else if (outcome == NOTTAKEN && *p_state != SN) {
        *p_state -= 1;
      }
      // This is shifting left 1 
      gHistory <<= 1;
      // add put the new history to the least significant bit
      gHistory += outcome;
      break;
    }
    case TOURNAMENT:{
      // shift in local & global history register

      // update local history predict
      uint32_t LHTIndex = pc & ((1 << pcIndexBits) -1);
      uint32_t *LPTIndex = &localHT[LHTIndex];
      *LPTIndex &= (1<<lhistoryBits)-1;
      //printf("LHTIndex %d, LPTIndex: %d\n", LHTIndex, *LPTIndex);
      //fflush(stdout);
      // previous LPT result
      uint8_t *p_LPT_result = &localPT[*LPTIndex];
      //printf("pLPT result: %d, ", *p_LPT_result);
      if (outcome == TAKEN && *p_LPT_result != ST) {
        *p_LPT_result += 1;
      }
      if (outcome == NOTTAKEN && *p_LPT_result != SN) {
        *p_LPT_result -= 1;
      }
      // Update LocalPrediction Table Index 
      *LPTIndex <<= 1;
      *LPTIndex += outcome;

      // update global history predict
      uint32_t index = gHistory & ((1 << ghistoryBits) - 1);
      uint8_t *p_state = &globalPT[index];
      //printf("pstate: %d \n", *p_state);
      if (outcome == TAKEN && *p_state != ST) {
        *p_state += 1;
      }
      else if (outcome == NOTTAKEN && *p_state != SN) {
        *p_state -= 1;
      }
      // This is shifting left 1 
      gHistory <<= 1;
      // add put the new history to the least significant bit
      gHistory += outcome;

      // update choice table only when the result mismatch
      uint8_t *p_choice = &choiceTable[index];
      if(localResult != globalResult){
        if(globalResult == outcome && *p_choice !=SG){
          *p_choice -= 1;
        }
        else if(localResult == outcome && *p_choice != SL){
          *p_choice += 1; 
        }
      }
      break;
    }
    case CUSTOM:{
      uint8_t result = (y < 0) ? 0 : 1;
      if (result != outcome || abs(y) <= threshold){
        uint32_t index = (gHistory ^ pc) & ((1 << ghistoryBits) - 1);
        uint32_t the_history = gHistory;
        for(int j=0; j < ghistoryBits; j++) {
          if (the_history & outcome){
            customWT[index][j] += 1;
          }else{
            customWT[index][j] -= 1;
          }
          the_history = the_history >> 1;
        }

      }
      gHistory <<= 1;
      // add put the new history to the least significant bit
      gHistory += outcome;

      break;
    }
    default:
      break;
  }
}
