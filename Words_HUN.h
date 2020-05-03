// Word Clock Hungarian
// Repo: https://github.com/nygma2004/word_clock_hun
// author: Csongor Varga, csongor.varga@gmail.com

#define MINSIZE 19    // defines the highest number of pixels to be light for each minute
#define HOURSIZE 11   // defines the highest number of pixels to be light for each hour

// This stores the pixels to be light for each hour. 
const byte hours[13][HOURSIZE] = {
  // nulla ora
  {},
  // egy ora
  {75,76,77,255},
  // ketto ora
  {85,86,87,88,89,255},
  // harom ora
  {55,56,57,58,59,255},    
  // negy ora
  {50,51,52,53,255}, 
  // ot ora
  {65,66,255}, 
  // hat ora
  {47,48,49,255}, 
  // het ora
  {67,68,69,255}, 
  // nyolc ora
  {60,61,62,63,64,255}, 
  // kilenc ora
  {90,91,92,93,94,95,255}, 
  // tiz ora
  {70,71,72,255}, 
  // tizenegy ora
  {70,71,72,73,74,75,76,77,255}, 
  // tizenketto ora
  {80,81,82,83,84,85,86,87,88,89,255}
  };

// This stores the pixels to be light for each 5 minute interval. 
const byte minutes[12][MINSIZE] = {
  // 00 minute
  {97,98,99,255},
  // 05 minute
  {0,1,10,11,12,13,14,15,16,20,21,22,23,97,98,99,255},
  // 10 minute
  {3,4,5,10,11,12,13,14,15,16,20,21,22,23,97,98,99,255},
  // 15 minute
  {40,41,42,43,44,45,255},
  // 20 minute
  {3,4,5,10,11,12,13,25,26,27,28,29,36,37,38,255},
  // 25 minute
  {0,1,10,11,12,13,25,26,27,28,29,36,37,38,255},
  // 30 minute
  {36,37,38,255},
  // 35 minute
  {0,1,10,11,12,13,14,15,16,20,21,22,23,36,37,38,255},
  // 40 minute
  {3,4,5,10,11,12,13,14,15,16,20,21,22,23,36,37,38,255},
  // 45 minute
  {30,31,32,33,34,40,41,42,43,44,45,255},
  // 50 minute
  {3,4,5,10,11,12,13,25,26,27,28,29,97,98,99,255},
  // 55 minute
  {0,1,10,11,12,13,25,26,27,28,29,97,98,99,255}
};
