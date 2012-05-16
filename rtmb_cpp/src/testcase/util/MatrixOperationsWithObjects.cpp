/*******************************************************************************
 * Copyright (c) 2008 International Business Machines Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ajith Ramanath            - initial API and implementation
 *    Radhakrishnan Thangamuthu - initial API and implementation
 *    Mike Fulton               - initial API and implementation
 *******************************************************************************/
 
#include <iostream>
#include "TimeOperations.h"
#include "MatrixOperationsWithObjects.h"
#include <stdlib.h>

using namespace std;

Integer::Integer() {
   num = 0;
}

inline int Integer::getValue() {
   return num;
}

inline void Integer::setValue(int val) {
   num = val;
}

MatrixObjects::MatrixObjects(int rows, int cols) {
   this -> rows = rows;
   this -> cols = cols;
   this -> data = new Integer*[rows];

   for (int i = 0; i < rows; i++)
      this -> data[i] = new Integer[cols];

   randomFill();
}

MatrixObjects::MatrixObjects(int rows, int cols, bool flag) {
   this -> rows = rows;
   this -> cols = cols;
   this -> data = new Integer*[rows];

   for (int i = 0; i < rows; i++)
      this -> data[i] = new Integer[cols];
}

MatrixObjects::~MatrixObjects() {
   for (int i = 0; i < rows; i++)
      delete[] this -> data[i];

   delete this -> data;
}

int MatrixObjects::getRows() {
   return this -> rows;
}

int MatrixObjects::getCols() {
   return this -> cols;
}

int MatrixObjects::getValueAt(int row, int col) {
   return data[row][col].getValue();
}

void MatrixObjects::setValueAt(int row, int col, int val) {
   data[row][col].setValue(val);
}

void MatrixObjects::randomFill() {
   unsigned int seedp = (unsigned int) time(NULL);
   unsigned int maxValue = 100;
   randomFill(seedp, maxValue);
}

void MatrixObjects::randomFill(unsigned int seedp, unsigned int maxValue) {
   int i, j, val;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
         val = 1 + (int) (maxValue * (rand_r(&seedp) / (RAND_MAX + 1.0)));

         data[i][j].setValue(val);
      }
   }
}

MatrixObjects *MatrixObjects::multiply(MatrixObjects &a, MatrixObjects &b) {
   int i = 0, j = 0, k = 0;
   int sum = 0;

   int row = a.getRows();
   int col = b.getCols();

   MatrixObjects* c = new MatrixObjects(a.getRows(), b.getCols(), true);

   /* we are only doing only square matrix multiplication */
   for (i = 0; i < row; i++) {
      for (j = 0; j < col; j++) {
         sum = 0;

         for (k = 0; k < col; k++) {
            sum += a.getValueAt(i, k) * b.getValueAt(k, j);
         }

         c->setValueAt(i, j, sum);
      }
   }

   return c;
}

void MatrixObjects::print() {
   for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
         cout << " " << data[i][j].getValue();
      }

      cout << endl;
   }
}

MatrixMultiplier::MatrixMultiplier(int subIters, int iters) {
   this->subIterations = subIters;
   iterations = iters;
   diffTime = new long long[iters];
}

void MatrixMultiplier::setDiffTime(long long* value) {
   diffTime = value;
}

long long * MatrixMultiplier::getDiffTime() {
   return diffTime;
}

void MatrixMultiplier::runIterations() {
   MatrixObjects a(10, 10), b(10, 10);
   long long difference = 0;
   struct timespec endTime, startTime;

   for (int i = 0; i < iterations; i++) {
      TimeOperations::getCurTime(&startTime);

      for (int j = 0; j < this->subIterations; j++) {
         MatrixObjects* c = MatrixObjects::multiply(a, b);
         delete c;
      }
      TimeOperations::getCurTime(&endTime);

      difference = TimeOperations::getTimeDiff(endTime, startTime);

      diffTime[i] = difference;
   }
}

void MatrixMultiplier::runIterationsWithSleepTime() {
   MatrixObjects a(10, 10), b(10, 10);
   long long difference = 0;
   struct timespec endTime, startTime;
   for (int i = 0; i < iterations; i++) {
      TimeOperations::getCurTime(&startTime);

      for (int j = 0; j < this->subIterations; j++) {
         MatrixObjects* c = MatrixObjects::multiply(a, b);
         delete c;
      }

      TimeOperations::doNanoSleep(sleepTime);
      TimeOperations::getCurTime(&endTime);

      difference = TimeOperations::getTimeDiff(endTime, startTime);
      diffTime[i] = difference;
   }
}

void MatrixMultiplier::setSleepTime(int timeToSleep) {
   sleepTime = timeToSleep;
}

void MatrixMultiplier::printMeasuredTime() {
   for (int i = 0; i < iterations; i++) {
      cout << " time_diff# " << i << " = " << diffTime[i] << endl;
   }
}
