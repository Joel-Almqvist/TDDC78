extern "C" {
  //#include "blurfilter.h"
  #include "blurfilter.c"
}

#include<mpi.h>
#include <iostream>
#include <string>

//using namespace std;

main(){
  std::cout << "hello\n";

  pixel px1{1,2,3};
  pixel* px_ptr = &px1;


  const double w = 1.222;
  const double* w_ptr = &w;
  //blurfilter(4, 4, px_ptr, 1, w_ptr);


  pixel* px_ptr2 = pix(px_ptr, 3, 4, 2);



}
