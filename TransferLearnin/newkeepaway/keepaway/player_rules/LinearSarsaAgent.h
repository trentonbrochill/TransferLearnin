#ifndef LINEAR_SARSA_AGENT
#define LINEAR_SARSA_AGENT

#include "SMDPAgent.h"
#include "tiles2.h"

#define RL_MEMORY_SIZE 1048576
#define RL_MAX_NONZERO_TRACES 100000
#define RL_MAX_NUM_TILINGS 6000

class LinearSarsaAgent:public SMDPAgent
{
  char weightsFile[256];
  bool bLearning;
  bool bSaveWeights;

  int epochNum;
  int lastAction;

  double alpha;
  double gamma;
  double lambda;
  double epsilon;

  int B;
  int C;
  int episode;
  bool ruleAction;

  double tileWidths[ MAX_STATE_VARS ];
  double Q[ MAX_ACTIONS ];

  double weights[ RL_MEMORY_SIZE ];
  double traces[ RL_MEMORY_SIZE ];

  int tiles[ MAX_ACTIONS ][ RL_MAX_NUM_TILINGS ];
  int numTilings;

  double minimumTrace;
  int nonzeroTraces[ RL_MAX_NONZERO_TRACES ];
  int numNonzeroTraces;
  int nonzeroTracesInverse[ RL_MEMORY_SIZE ];

  collision_table *colTab;

  // Load / Save weights from/to disk
  bool loadWeights( char *filename );
  bool saveWeights( char *filename );

  // Value function methods for CMACs
  int useRules(double state[]); // let's use a discovered human policy
  int  selectAction();
  void initializeTileWidths( int numK, int numT );
  double computeQ( int a );
  int  argmaxQ();
  void updateWeights( double delta );
  void loadTiles( double state[] );

  // Eligibility trace methods
  void clearTrace( int f );
  void clearExistentTrace( int f, int loc );
  void decayTraces( double decayRate );
  void setTrace( int f, float newTraceValue );
  void increaseMinTrace();

 public:
  LinearSarsaAgent                  ( int    numFeatures,
                      int    numActions,
                      bool   bLearn,
                      double widths[],
                      char   *loadWeightsFile,
                      char   *saveWeightsFile );

  // SMDP Sarsa implementation
  int  startEpisode( double state[] );
  int  step( double reward, double state[] );
  void endEpisode( double reward );
  void setParams(int iCutoffEpisodes, int iStopLearningEpisodes);
} ;

#endif
