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
#include "MatrixOperations.h"
#include <stdlib.h>

using namespace std;

Matrix::Matrix(int rows, int cols) {
   this -> rows = rows;
   this -> cols = cols;
   this -> data = new int *[rows];

   for (int i = 0; i < rows; i++)
      this -> data[i] = new int[cols];

   randomFill();
}

Matrix::Matrix(int rows, int cols, bool flag) {
   this -> rows = rows;
   this -> cols = cols;
   this -> data = new int *[rows];

   for (int i = 0; i < rows; i++)
      this -> data[i] = new int[cols];
}

Matrix::~Matrix() {
   for (int i = 0; i < rows; i++)
      delete[] this -> data[i];

   delete this -> data;
}

int Matrix::getRows() {
   return this -> rows;
}

int Matrix::getCols() {
   return this -> cols;
}

int Matrix::getValueAt(int row, int col) {
   return this -> data[row][col];
}

void Matrix::setValueAt(int row, int col, int val) {
   this -> data[row][col] = val;
}

void Matrix::randomFill() {
   unsigned int seedp = (unsigned int) time(NULL);
   unsigned int maxValue = 100;
   randomFill(seedp, maxValue);
}

void Matrix::randomFill(unsigned int seedp, unsigned int maxValue) {
   int i, j, val;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
         val = 1 + (int) (maxValue * (rand_r(&seedp) / (RAND_MAX + 1.0)));

         data[i][j] = val;
      }
   }
}

Matrix *Matrix::multiply(Matrix &a, Matrix &b) {
   int i = 0, j = 0, k = 0;
   int sum = 0;

   int row = a.getRows();
   int col = b.getCols();

   Matrix* c = new Matrix(a.getRows(), b.getCols(), true);

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

Matrix *Matrix::multiply(Matrix &a, Matrix &b, Matrix* c) {
   int i = 0, j = 0, k = 0;
   int sum = 0;

   int row = a.getRows();
   int col = b.getCols();

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

void Matrix::print() {
   for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
         cout << " " << data[i][j];
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
   Matrix a(10, 10), b(10, 10);
   long long difference = 0;
   struct timespec endTime, startTime;

   for (int i = 0; i < iterations; i++) {
      TimeOperations::getCurTime(&startTime);

      for (int j = 0; j < this->subIterations; j++) {
         Matrix* c = Matrix::multiply(a, b);
         delete c;
      }
      TimeOperations::getCurTime(&endTime);

      difference = TimeOperations::getTimeDiff(endTime, startTime);

      diffTime[i] = difference;
   }
}

void MatrixMultiplier::runIterationsWithSleepTime() {
   Matrix a(10, 10), b(10, 10);
   long long difference = 0;
   struct timespec endTime, startTime;
   for (int i = 0; i < iterations; i++) {
      TimeOperations::getCurTime(&startTime);

      for (int j = 0; j < this->subIterations; j++) {
         Matrix* c = Matrix::multiply(a, b);
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
