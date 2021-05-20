//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Link Lin";
const char *studentID = "A13817414";
const char *email = "yul065@ucsd.edu";
const char *studentName2 = "Tianyi Shan";
const char *studentID2 = "";
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

uint8_t *gshareBHT; //gshare Branch Prediction Table
uint32_t gHistory;

uint32_t *localHT;    // Local history table
uint8_t *localPT;     // Local prediction table
uint8_t *choiceTable; // Choice prediction table

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t convert_state_to_result(uint8_t st){
  // Changing the order results in different number???
  if(st == 2 || st == 3){
    return 1;
  }
  return 0;
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
      gshareBHT = malloc(gTableSize* sizeof(uint8_t));
      memset(gshareBHT, WN, gTableSize * sizeof(uint8_t));

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
    uint32_t localHistory = localHT[LHTIndex];
    uint8_t localPredict = localPT[localHistory];

    // get global history predict
    uint32_t index = gHistory & ((1 << ghistoryBits) - 1);
    uint8_t globalPredict = gshareBHT[index];  

    // Choose based on choice prediction table
    uint8_t choicePredict = choiceTable[index];
    uint8_t selectResult = convert_state_to_result(choicePredict);

    // TODO changing the order may get different result
    if(selectResult == SELECTG){
      return convert_state_to_result(localPredict);
    }
    return convert_state_to_result(globalPredict);
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
      else if (outcome == NOTTAKEN && p_state != SN) {
        *p_state -= 1;
      }
      // This is shifting left 1 
      gHistory <<= 1;
      // add put the new history to the least significant bit
      gHistory += outcome;
      break;
    }
    case TOURNAMENT:
    default:
      break;
  }
}
