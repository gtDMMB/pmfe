
// Copyright 2006 Peter Huggins
// Released under the GNU GPL license

/*
    This file is a part of iB4e, which is free software; 
    you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>

#include "BBpolytope.h"

#ifdef GMP
  #include <gmpxx.h>
#endif

using namespace std;

//#define EPSILON 0.000000001 
//#define MULTFACTOR (((long long int)(10000000)) * ((long long int)(10000000)))
//#define MULTFACTOR (1)

#define HASHTABLESIZE 5542347

//#define PROFILING

//#define DEBUG
//#define DEBUG2

#ifdef GMP
void printNumber(NUMBER a) { cout << a.get_str(10);};
#else
void printNumber(NUMBER a) { cout << a;};
#endif

void BBPolytope::Build()
{

  hashtable = new NUMBER *[HASHTABLESIZE];
  facetvertextable = new vertexnode *[HASHTABLESIZE];

  ghostvertices = new int[dimension];

  for(int i = 0; i < HASHTABLESIZE; i++) {
    hashtable[i] = NULL;
    facetvertextable[i] = NULL;
  }

  numvertices = 0;
  linealitydim = 0;

  EuclideanVector *solution1, *solution2;
  EuclideanVector e1(dimension), centroid(dimension);

  for(int i =1; i < dimension; i++)
    e1.data[i] = 0;

  e1.data[0] = 1;

  Face startface;
  startface.InitializeMaxFacets(MAXFACETS);
  startface.InitializeDimensions(-1, dimension);
  startface.InitializeMaxVertices(MAXVERTICES);

  solution1 = new EuclideanVector(dimension);

  solution1->id = 1;

  BlackBoxOptimize(&e1, (solution1));

  #ifdef DEBUG2
  cout << "\nGoing to add vertex:  ";
  //solution1->Print();
  #endif
 
  numvertices++;

  startface.AddLIVertexToLowDim((solution1));

  #ifdef DEBUG2
  cout << "Added the vertex, computing RHS...";
  #endif

  startface.ComputeRHS();

  while(startface.dimension < dimension) {

    //for(int i = 0; i < dimension - startface.dimension + 1; i++) {

      int i = 0;

    // why the hell was this a for loop??      
    {

      solution1 = new EuclideanVector(dimension);
      solution1->id = startface.dimension + 2;
      BlackBoxOptimize(startface.normalvectors[i], (solution1));

      #ifdef DEBUG2
      cout << "new direction under consideration is:  ";
      startface.normalvectors[i]->Print();
      cout << "point in plus direction:  ";
      solution1->Print();
      // printf("and RHS for it is:  %.15f ", startface.rhs[i]);
      #endif

      startface.normalvectors[i]->Negation();
 
      #ifdef DEBUG2
      cout << "Now about to optimize in `minus' direction : ";
      startface.normalvectors[i]->Print();
      #endif

      solution2 = new EuclideanVector(dimension);
      solution2->id = startface.dimension + 2;
      BlackBoxOptimize(startface.normalvectors[i], (solution2));

      #ifdef DEBUG2
      cout << "point in minus direction:  ";
      solution2->Print();
      #endif

      startface.normalvectors[i]->Negation();

      if(dotproduct((solution1), startface.normalvectors[i]) > startface.rhs[i]) {
        
        #ifdef DEBUG2
        cout << "Point to be added:  ";
        solution1->Print();
        #endif

        startface.AddLIVertexToLowDim(solution1);

        #ifdef DEBUG2
        cout << "Point added:  ";
        //solution1->Print();
        #endif
        
        numvertices++;
      }
      else if(dotproduct((solution2), startface.normalvectors[i]) < startface.rhs[i]) {
 
        #ifdef DEBUG2
        cout << "Point to be added:  ";
        solution2->Print();
        #endif

        startface.AddLIVertexToLowDim(solution2);

        #ifdef DEBUG2
        cout << "Point added:  ";
       // solution2->Print();
        #endif

        numvertices++;
      }
      else {
       // TODO Here's where the fun begins

        solution1->plusequals(startface.normalvectors[i]);
        solution1->id = -1 * linealitydim;
        startface.AddLIVertexToLowDim(solution1);
        linealitydim++;
        numvertices++;

       //The easy hack is to add a ghost vertex which lifts the polytope to
       // a cone.  This works even if we've already done such lifts 

       // The things we need to keep track of are the affine hull, and the
       // indices of ghost vertices 

        //cerr << "Polytope not full-dimensional!!";
      }
      startface.ComputeRHS();
    }

  }

  for(int i = 0; i < dimension; i++)
    centroid.data[i] = 0;
  for(int i = 0; i < startface.numvertices; i++)
    centroid.plusequals(*(startface.vertices[i]));

  // NOTE centroid is scaled by (dimension+1) !!!

  #ifdef DEBUG2
  cout << "\nPushing all simplex faces";
  #endif

  //Now we have a full-dimensional simplex

  for(int j = 0; j < dimension + 1; j++) {
    stack.Push(startface.facets[j]);
    startface.facets[j]->enqueued = true;
    #ifdef DEBUG2
    cout << "Here's a facet of full-dim simplex\n";
    startface.facets[j]->Print();
    cout << " Pushed someone with " << startface.facets[j]->numfacets << " facets ";
    #endif
  }

  #ifdef DEBUG2
  cout << "\nPushed all simplex faces";
  #endif

  Face *newridge;
  Face *newface;
  Face *footinthedoor;

  while(!stack.empty) {

      #ifdef DEBUG2
      cout << "\nStack still not empty";
      #endif

    dequeue = (Face *)(stack.Pop());
    dequeue->enqueued = false;

    if(dequeue->deleted == false) 
    if(newdirection(dequeue)){

      #ifdef DEBUG2
      cout << "\nComputing optimal vertex for facet normal";
      #endif

      solution1 = new EuclideanVector(dimension);
      #ifdef DEBUG2
      cout << "\nCreated solution1:";
      solution1->Print();
      #endif

      BlackBoxOptimize(dequeue->normalvectors[0], (solution1));

      #ifdef DEBUG2
      cout << "\n";
      dequeue->normalvectors[0]->Print();
      cout << "was the normal vector, and here's the solution found:  ";
      solution1->Print();
      //cout << "and here's the rhs:  " << dequeue->rhs[0] << "\n";
      #endif

      if((dotproduct((solution1), dequeue->normalvectors[0]) <= dequeue->rhs[0])) {
         #ifdef DEBUG2
         cout << "\nAbout to delete needless solution1:  ";
         solution1->Print();
         #endif         

         solution1->deletedata();
         delete solution1;
         footinthedoor = dequeue;
       
         #ifdef DEBUGEXITS
         exit(0);
         #endif
      }
      else {  //add new vertex

        numvertices++;
        solution1->id = numvertices;

        #ifdef DEBUG2
        cout << "\nadded new vertex: ";
        //solution1->Print();
        #endif

        cout << "1 ";

        solution1->Print();

        #ifdef DEBUG2
        cout << "\nCalling processhorizonridges...";
        #endif

        processhorizonridges((solution1), dequeue, &(stack2), &tobedeletedstack); //recursively adds new faces for all horizon ridges, and also marks visible faces as deleted

        while(!tobedeletedstack.empty) {
          dequeue2 = (Face *)(tobedeletedstack.Pop());
          #ifdef DEBUG2
          cout << "About to delete facet:\n";
          dequeue2->Print();
          #endif
          dequeue2->deleteme();
        }

        #ifdef DEBUG2
        cout << "...done";
        #endif

        while(!stack2.empty)  {
          #ifdef DEBUG2
          cout << "\nstack2 still not empty";
          #endif

          dequeue2 = (Face *)(stack2.Pop());
          
          #ifdef DEBUG2
          cout <<"\ncalling vertexandridge...";
          #endif

          newface = vertexandridge((solution1), dequeue2);  // distinguished vertex will be vertices[0]

          #ifdef DEBUG2
          cout << "...done";
          #endif

        newface->ComputeNormalVectorAwayFromPoint(&centroid);

          //exit(0);
          dequeue2->incidents[0] = newface;
          stack.Push(newface);
          newface->enqueued = true;

          EuclideanVector *distinguishedvertex = newface->vertices[0];
          newface->SortVertices();

          //exit(0);

          for(int i = 0; i < newface->numvertices; i++) {

            // note we skip the distinguished vertex
            if(newface->vertices[i] != distinguishedvertex) {

              newridge = newface->AddFacetWithoutVertex(i);
              newridge->incidents = new Face *[2];
              newridge->incidents[0] = newface;
              newridge->incidents[1] = newface;
              stack3.Push(newridge);
            }
          }
        }

        stack3.Sort();
        Face *ridge1, *ridge2;
        ridge1 = (Face *)(stack3.Pop());

        while(!stack3.empty) {

          ridge2 = ridge1;
          ridge1 = (Face *)(stack3.Pop());

          if(sortcompare(ridge1, ridge2) == 0) { //then equate the two ridges
            ridge1->incidents[1] = ridge2->incidents[0];
            ridge2->incidents[0]->DeleteFacet(ridge2);
            ridge1->incidents[1]->AddFacet(ridge1);

          }
        }
      }
    }
  }

  for(int i = 0; i < HASHTABLESIZE; i++)
    hashtable[i] = NULL;

}

bool operator<(vertexnode a, vertexnode b)
{
  return (a.item < b.item);
}

bool BBPolytope::newdirection(Face *myface)
{

  NUMBER paramValues[myface->ambientdimension];

  bool ismultiple;

    for(int i = 0; i < myface->ambientdimension; i++) {
      paramValues[i] = myface->normalvectors[0]->data[i];
    }

  ismultiple = hash(paramValues, myface, 0);

  if(ismultiple) {
    #ifdef DEBUG2
      cout << "Halleluyah, dodged some work";
    #endif
    return false;
  }
 
  #ifdef DEBUG2
    cout << "Damn dodged no work";
  #endif
  return true;

}


bool BBPolytope::hash(NUMBER *myvector, Face *myface, int recordvertices)
{

  #ifdef GMP
    mpz_class tmp;
    tmp = 0;
  #else
    long long unsigned int tmp;
  #endif

  static unsigned int *weights = NULL;

  static int numhits = 0;

  int size = myface->ambientdimension;

  if(weights == NULL) {
    weights = new unsigned int[size];
    for(int i = 0; i < size; i++)
      weights[i] = rand() * rand();
  }

  NUMBER divisor;
  divisor = gcd(myvector, size);
 
  #ifdef DEBUG
  cout << "Computed gcd is:  " << divisor << "\n";
  #endif

  for(int i = 0; i < size; i++)
    myvector[i] /= divisor;

  long long int hashvalue = 0;

  
  

  for(int i = 0; i < size; i++) {
 
    #if NUMBER_TYPE == GMP_RATIONALS
    tmp = (weights[i] * (23*myvector[i].get_num() + 17*myvector[i].get_den())) % HASHTABLESIZE;
    #else
    tmp = (weights[i] * myvector[i]) % HASHTABLESIZE;
    #endif

    #ifdef GMP
    hashvalue += tmp.get_si();
    #else
    hashvalue += tmp;
    #endif
    
  }

  hashvalue %= HASHTABLESIZE;
  if(hashvalue < 0) {  
    #ifdef DEBUG2
    cout << "Correcting hash value of:  " << hashvalue << "\n";
    #endif
    hashvalue += HASHTABLESIZE;
    hashvalue %= HASHTABLESIZE;
  }

  #ifdef DEBUG2
    cout << "Initial hash value is:  " << hashvalue << "\n";
  #endif

  // main loop
  for(int j = 0; ; j++) {

    if(j == 1000) {
      cerr << "Hash Table too small!!!";
      exit(0);
    }

    hashvalue += j*j;  // quadratic probing

    if(hashvalue >= HASHTABLESIZE)
      hashvalue %= HASHTABLESIZE;

    if(hashvalue < 0 || hashvalue >= HASHTABLESIZE) {
      cerr << "hash table offset  overflow!";
      exit(1);
    }
    if(hashtable[hashvalue] == NULL) {

      #ifdef DEBUG2
        cout << "Hashed to empty bucket " << hashvalue << "\n";
      #endif

      hashtable[hashvalue] = new NUMBER[size];

      for(int i = 0; i < size; i++)
        (hashtable[hashvalue])[i] = myvector[i];

      if(recordvertices) {
        numhits++;
        facetvertextable[hashvalue] = new vertexnode;
        facetvertextable[hashvalue]->item = numhits;
        facetvertextable[hashvalue]->next = NULL;
        
        for(int i=0; i < myface->numvertices; i++)
          pushvertexintoincidence(hashvalue ,myface->vertices[i]);
      }

      return false;
    }
    // else hashtable entry wasnt empty, because the if above returned

    #ifdef DEBUG2
      cout << "Hashed to non-empty bucket " << hashvalue << "\n";

    #endif

    bool isequal = true;

    for(int i = 0; i < size; i++)
      if((hashtable[hashvalue])[i] != myvector[i])
        isequal = false;

    if(isequal) { 
      if(recordvertices)
        for(int i=0; i < myface->numvertices; i++)
          pushvertexintoincidence(hashvalue ,myface->vertices[i]);
      return true;
    }
  }

}

void BBPolytope::pushvertexintoincidence(int location, EuclideanVector *vertex)
{
  vertexnode *current = facetvertextable[location]->next;
  vertexnode *prev = facetvertextable[location];

  while(current != NULL) {
    prev = current;
    if(current->item == vertex->id)
      return;
    current = current->next;
  }

  prev->next = new vertexnode;
  prev->next->next = NULL;
  prev->next->item = vertex->id;
  return;

}

NUMBER gcd(NUMBER *myvector, int size)
{
  NUMBER ans, abs;

  int j;

  // test to see if there are non-units
  for(j = 0; j < size; j++)
    if(myvector[j] != 0)
      if(((NUMBER)(1 / myvector[j])) * myvector[j] != 1)
        break;

  if(j == size) { // ie we didnt break, so everything's a unit
    for(j = 0; myvector[j] == 0; j++)
      ;  // NULL FOR
    if(myvector[j] > 0)
      return myvector[j];
    else return (-1 * myvector[j]);
  } 

  for(j = 0; myvector[j] == 0; j++)
    ;

  ans = myvector[j];
  if(ans < 0)
    ans *= -1;

  for(int i = j+1; i < size; i++) {
    abs = myvector[i];
    if(abs < 0)
      abs *= -1;
    ans = gcd2(ans, abs);
  }

  return ans;
}

#if NUMBER_TYPE != GMP_RATIONALS
NUMBER gcd2(NUMBER a, NUMBER b) 
{
  if(b == 0)
    return a;

  return gcd2(b, a%b);
}

#else
NUMBER gcd2(NUMBER a, NUMBER b) 
{
  if(a > 0)
    return a;
  else return -1 * a;
}
#endif

Face * BBPolytope::vertexandridge(EuclideanVector *v, Face *r)
{
  Face *ans = new Face();

  //exit(0);

  int d = v->dimension;

  ans->InitializeMaxFacets(d);
  ans->InitializeDimensions(d-1, d);
  ans->InitializeMaxVertices(d);

  //exit(0);

  ans->AddVertex(v);
  ans->AddFacet(r);

  //exit(0);

  for(int i = 1; i < d ; i++)
    ans->AddVertex(r->vertices[i-1]);

  //exit(0);

  //ans->ComputeNormalVectorTowardPoint(v);

 return ans;
}



void BBPolytope::processhorizonridges(EuclideanVector *newvertex, Face *face, Stack *stack, Stack *tobedeletedstack) {

  #ifdef DEBUG2
    cout << "ProHorRid called\n";
  #endif

  face->deleted = true;
  tobedeletedstack->Push(face);

  Face *neighbor;

  #ifdef DEBUG2
  cout << "\nProHorRid:  Face has normal vector:  ";
  face->normalvectors[0]->Print(); 
  cout << "\nProHorRid:  Face has " << face->numfacets << " facets";
  #endif

  for(int i = 0; i < face->numfacets; i++) {

    #ifdef DEBUG2
    cout << "\nProHorRid:  Processing facets";
    #endif

    if(face->facets[i]->incidents[0] == face) 
      neighbor = face->facets[i]->incidents[1];
    else { 
      if(face->facets[i]->incidents[1] == face) 
        neighbor = face->facets[i]->incidents[0];

      else {
        cerr << "\nFace not incident to its facet!!";
        exit(0);
      }
    }

	if(neighbor->deleted == false) {
          #ifdef DEBUG2
          cout << "\nNeighbor not deleted";
          #endif
	  if(dotproduct(neighbor->normalvectors[0], newvertex) > neighbor->rhs[0]) 
          {
            #ifdef DEBUG2
            cout << "\nNeighbor was visible, calling ProHorRid recursively...";  
            #endif         
            processhorizonridges(newvertex, neighbor, stack, tobedeletedstack); //neighbor is visible
           }
       
         else {
           #ifdef DEBUG2
           cout << "\nNeighbor not visible, pushing onto stack...";
           #endif
           stack->Push(face->facets[i]);
           //face->facets[i]->enqueued = true;
           face->facets[i]->incidents[0] = neighbor;
           face->facets[i]->incidents[1] = neighbor;
         }
      }

      if(neighbor->tobedeleted == true) {  //neighbor and us are marked to be deleted, so kill the ridge between
        tobedeletedstack->Push(face->facets[i]);
        //cerr << "Pushing a ridge";
      }
  }

  face->tobedeleted = true;

  #ifdef DEBUG2
    cout << "ProHorRid done\n";
  #endif

  return;

}
