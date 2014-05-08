#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "LinearSarsaAgent.h"
#include "LoggerDraw.h"


// Uses classifier defined in .model to seed a reinforcement learner using Value Bonus method

// If all is well, there should be no mention of anything keepaway- or soccer-
// related in this file.

extern LoggerDraw LogDraw;



LinearSarsaAgent::LinearSarsaAgent( int numFeatures, int numActions, bool bLearn,
                    double widths[],
                    char *loadWeightsFile, char *saveWeightsFile ):
  SMDPAgent( numFeatures, numActions )
{
  bLearning = bLearn;

  for ( int i = 0; i < getNumFeatures(); i++ ) {
    tileWidths[ i ] = widths[ i ];
  }

  if ( bLearning && strlen( saveWeightsFile ) > 0 ) {
    strcpy( weightsFile, saveWeightsFile );
    bSaveWeights = true;
  }
  else {
    bSaveWeights = false;
  }

  alpha = 0.125;
  gamma = 1.0;
  lambda = 0;
  epsilon = 0.01;
  minimumTrace = 0.01;

  B = 10; // reward bonus
  C = 100; // initial episodes where learner must use classifier
  episode = 0;



  epochNum = 0;
  lastAction = -1;

  numNonzeroTraces = 0;
  for ( int i = 0; i < RL_MEMORY_SIZE; i++ ) {
    weights[ i ] = 0;
    traces[ i ] = 0;
  }

  srand( (unsigned int) 0 );
  int tmp[ 2 ];
  float tmpf[ 2 ];
  colTab = new collision_table( RL_MEMORY_SIZE, 1 );

  GetTiles( tmp, 1, 1, tmpf, 0 );  // A dummy call to set the hashing table
  srand( time( NULL ) );

  if ( strlen( loadWeightsFile ) > 0 )
    loadWeights( loadWeightsFile );


  if((model=svm_load_model("best_mix_weka2.libsvm.model"))==0)
  {
      fprintf(stderr,"can't open model file %s\n","best_mix.libsvm.model");
      exit(1);
  }

}

int LinearSarsaAgent::classifierAction(double state[]){

//    svm_node state_node[13];
//   // std::cout << "trying";
//    for(int i = 0; i < 13; i++){
//        state_node[i].index = i+1;
//        state_node[i].value = state[i] ;
//    //    std::cout << state_node[i].index << " " << state_node[i].value << " " ;
//    }

    svm_node state_node[5];
    state_node[0].value = state[3] ;
    state_node[0].index = 1;
    state_node[1].value = state[4] ;
    state_node[1].index = 2;
    state_node[2].value = state[9] ;
    state_node[2].index = 3;
    state_node[3].value = state[11] ;
    state_node[3].index = 4;
    state_node[4].index = -1;

    double prediction = svm_predict(model, state_node);

    return (int)prediction;
}

int LinearSarsaAgent::startEpisode( double state[] )
{
  epochNum++;
  decayTraces( 0 );
  loadTiles( state );
  for ( int a = 0; a < getNumActions(); a++ ) {
    Q[ a ] = computeQ( a );
  }



  //lastAction = selectAction(); instead follow rules

  lastAction = LinearSarsaAgent::classifierAction(state);
  ruleAction = true;

  char buffer[128];
  sprintf( buffer, "Q[%d] = %.2f", lastAction, Q[lastAction] );
  LogDraw.logText( "Qmax", VecPosition( 25, -30 ),
                   buffer,
                   1, COLOR_BROWN );

  for ( int j = 0; j < numTilings; j++ )
    setTrace( tiles[ lastAction ][ j ], 1.0 );
  return lastAction;
}

int LinearSarsaAgent::step( double reward, double state[] )
{

  if (ruleAction) reward += B;

  double delta = reward - Q[ lastAction ];
  loadTiles( state );
  for ( int a = 0; a < getNumActions(); a++ ) {
    Q[ a ] = computeQ( a );
  }

  if (episode > C){
      lastAction = selectAction();

      ruleAction = ( LinearSarsaAgent::classifierAction(state) == lastAction );

      //std::cout <<"class "<< LinearSarsaAgent::classifierAction(state) << std::endl;

  }
  else {
      episode++;
      lastAction = LinearSarsaAgent::classifierAction(state);
      ruleAction = true;
  }


  char buffer[128];
  sprintf( buffer, "Q[%d] = %.2f", lastAction, Q[lastAction] );
  LogDraw.logText( "Qmax", VecPosition( 25, -30 ),
                   buffer,
                   1, COLOR_BROWN );

  if ( !bLearning )
    return lastAction;

  //char buffer[128];
  sprintf( buffer, "reward: %.2f", reward );
  LogDraw.logText( "reward", VecPosition( 25, 30 ),
           buffer,
           1, COLOR_NAVY );

  delta += Q[ lastAction ];
  updateWeights( delta );
  Q[ lastAction ] = computeQ( lastAction ); // need to redo because weights changed
  decayTraces( gamma * lambda );

  for ( int a = 0; a < getNumActions(); a++ ) {  //clear other than F[a]
    if ( a != lastAction ) {
      for ( int j = 0; j < numTilings; j++ )
        clearTrace( tiles[ a ][ j ] );
    }
  }
  for ( int j = 0; j < numTilings; j++ )      //replace/set traces F[a]
    setTrace( tiles[ lastAction ][ j ], 1.0 );

  return lastAction;
}

void LinearSarsaAgent::endEpisode( double reward )
{
  if ( bLearning && lastAction != -1 ) { /* otherwise we never ran on this episode */
    char buffer[128];
    sprintf( buffer, "reward: %.2f", reward );
    LogDraw.logText( "reward", VecPosition( 25, 30 ),
             buffer,
             1, COLOR_NAVY );

    /* finishing up the last episode */
    /* assuming gamma = 1  -- if not,error*/
    if ( gamma != 1.0)
      cerr << "We're assuming gamma's 1" << endl;
    double delta = reward - Q[ lastAction ];
    updateWeights( delta );
  }
  if ( bLearning && bSaveWeights && rand() % 200 == 0 ) {
    saveWeights( weightsFile );
  }
  lastAction = -1;
}

int LinearSarsaAgent::selectAction()
{
  int action;

  // Epsilon-greedy
  if ( bLearning && drand48() < epsilon ) {     /* explore */
    action = rand() % getNumActions();
  }
  else{
    action = argmaxQ();
  }

  return action;
}

bool LinearSarsaAgent::loadWeights( char *filename )
{
  cout << "Loading weights from " << filename << endl;
  int file = open( filename, O_RDONLY );
  read( file, (char *) weights, RL_MEMORY_SIZE * sizeof(double) );
  colTab->restore( file );
  close( file );
  cout << "...done" << endl;
  return true;
}

bool LinearSarsaAgent::saveWeights( char *filename )
{
  int file = open( filename, O_CREAT | O_WRONLY, 0664 );
  write( file, (char *) weights, RL_MEMORY_SIZE * sizeof(double) );
  colTab->save( file );
  close( file );
  return true;
}

// Compute an action value from current F and theta
double LinearSarsaAgent::computeQ( int a )
{
  double q = 0;
  for ( int j = 0; j < numTilings; j++ ) {
    q += weights[ tiles[ a ][ j ] ];
  }

  return q;
}

// Returns index (action) of largest entry in Q array, breaking ties randomly
int LinearSarsaAgent::argmaxQ()
{
  int bestAction = 0;
  double bestValue = Q[ bestAction ];
  int numTies = 0;
  for ( int a = bestAction + 1; a < getNumActions(); a++ ) {
    double value = Q[ a ];
    if ( value > bestValue ) {
      bestValue = value;
      bestAction = a;
    }
    else if ( value == bestValue ) {
      numTies++;
      if ( rand() % ( numTies + 1 ) == 0 ) {
    bestValue = value;
    bestAction = a;
      }
    }
  }

  return bestAction;
}

void LinearSarsaAgent::updateWeights( double delta )
{
  double tmp = delta * alpha / numTilings;
  for ( int i = 0; i < numNonzeroTraces; i++ ) {
    int f = nonzeroTraces[ i ];
    if ( f > RL_MEMORY_SIZE || f < 0 )
      cerr << "f is too big or too small!!" << f << endl;
    weights[ f ] += tmp * traces[ f ];
  }
}

void LinearSarsaAgent::loadTiles( double state[] )
{
  int tilingsPerGroup = 32;  /* num tilings per tiling group */
  numTilings = 0;

  /* These are the 'tiling groups'  --  play here with representations */
  /* One tiling for each state variable */
  for ( int v = 0; v < getNumFeatures(); v++ ) {
    for ( int a = 0; a < getNumActions(); a++ ) {
      GetTiles1( &(tiles[ a ][ numTilings ]), tilingsPerGroup, colTab,
         state[ v ] / tileWidths[ v ], a , v );
    }
    numTilings += tilingsPerGroup;
  }
  if ( numTilings > RL_MAX_NUM_TILINGS )
    cerr << "TOO MANY TILINGS! " << numTilings << endl;
}


// Clear any trace for feature f
void LinearSarsaAgent::clearTrace( int f)
{
  if ( f > RL_MEMORY_SIZE || f < 0 )
    cerr << "ClearTrace: f out of range " << f << endl;
  if ( traces[ f ] != 0 )
    clearExistentTrace( f, nonzeroTracesInverse[ f ] );
}

// Clear the trace for feature f at location loc in the list of nonzero traces
void LinearSarsaAgent::clearExistentTrace( int f, int loc )
{
  if ( f > RL_MEMORY_SIZE || f < 0 )
    cerr << "ClearExistentTrace: f out of range " << f << endl;
  traces[ f ] = 0.0;
  numNonzeroTraces--;
  nonzeroTraces[ loc ] = nonzeroTraces[ numNonzeroTraces ];
  nonzeroTracesInverse[ nonzeroTraces[ loc ] ] = loc;
}

// Decays all the (nonzero) traces by decay_rate, removing those below minimum_trace
void LinearSarsaAgent::decayTraces( double decayRate )
{
  int f;
  for ( int loc = numNonzeroTraces - 1; loc >= 0; loc-- ) {
    f = nonzeroTraces[ loc ];
    if ( f > RL_MEMORY_SIZE || f < 0 )
      cerr << "DecayTraces: f out of range " << f << endl;
    traces[ f ] *= decayRate;
    if ( traces[ f ] < minimumTrace )
      clearExistentTrace( f, loc );
  }
}

// Set the trace for feature f to the given value, which must be positive
void LinearSarsaAgent::setTrace( int f, float newTraceValue )
{
  if ( f > RL_MEMORY_SIZE || f < 0 )
    cerr << "SetTraces: f out of range " << f << endl;
  if ( traces[ f ] >= minimumTrace )
    traces[ f ] = newTraceValue;         // trace already exists
  else {
    while ( numNonzeroTraces >= RL_MAX_NONZERO_TRACES )
      increaseMinTrace(); // ensure room for new trace
    traces[ f ] = newTraceValue;
    nonzeroTraces[ numNonzeroTraces ] = f;
    nonzeroTracesInverse[ f ] = numNonzeroTraces;
    numNonzeroTraces++;
  }
}

// Try to make room for more traces by incrementing minimum_trace by 10%,
// culling any traces that fall below the new minimum
void LinearSarsaAgent::increaseMinTrace()
{
  minimumTrace *= 1.1;
  cerr << "Changing minimum_trace to " << minimumTrace << endl;
  for ( int loc = numNonzeroTraces - 1; loc >= 0; loc-- ) { // necessary to loop downwards
    int f = nonzeroTraces[ loc ];
    if ( traces[ f ] < minimumTrace )
      clearExistentTrace( f, loc );
  }
}

void LinearSarsaAgent::setParams(int iCutoffEpisodes, int iStopLearningEpisodes)
{
  /* set learning parameters */
}
