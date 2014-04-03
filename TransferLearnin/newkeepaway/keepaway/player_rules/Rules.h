int LinearSarsaAgent::useRules(double state[]){
  //3v2 from gridworld
  int Action = -1;
  if (0) ;
else if((state[11] >= 63.1475) && (state[3] <= 4.33416) && (state[4] >= 4.06902) ) Action=1;
else if((state[4] <= 7.31493) && (state[12] >= 49.2988) && (state[0] >= 7.33504) && (state[3] <= 4.7517) ) Action=2;
else Action=0;
return (Action);
}
