//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
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

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
}

// This is the method for gshare predict
// which is characterizing by XORing the global history register with the
// lower bits(same length as the global history) of the branch's address
// result is either TAKEN/NOTTAKEN
uint8_t gshare_predict(uint32_t pc)
{
  // Get the index into the BHT
  uint32_t index = gHistory ^ (pc & (1 << ghistoryBits - 1));
  // previous state of the branch (can be SN, WN, WT, ST)
  uint8_t p_state = gshareBHT[index];
  if (p_state == WT || p_state == ST)
    return TAKEN;
  return NOTTAKEN;
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

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
    gHistory = NOTTAKEN;
    // Set up BHT table
    int tableSize = 1 << ghistoryBits;
    // Initialize the BHT to WN
    gshareBHT = malloc(tableSize * sizeof(uint8_t));
    memset(gshareBHT, WN, tableSize * sizeof(uint8_t));
    // Everything in the gshareBHT should be 1 at the point
    // for(int i = 0; i < tableSize; i++){
    //   printf("%d\n", gshareBHT[i]);
    // }
    return gshare_predict(pc);
  case TOURNAMENT:
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
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    // update G-share based on State machine
    // Get the previous state
    uint32_t index = gHistory ^ (pc & (1 << ghistoryBits - 1));
    uint8_t *p_state = gshareBHT[index];
    if (outcome && *p_state != ST)
    {
      *p_state += 1;
    }
    else if (outcome == NOTTAKEN && p_state != SN)
    {
      *p_state -= 1;
    }
  default:
    break;
  }
}
