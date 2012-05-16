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
 
#ifndef __MATRIX_OPERATIONS__
#define __MATRIX_OPERATIONS__
class Matrix {
		private:
			int rows;
			int cols;
			int **data;

		public:
			Matrix(int rows ,int cols) ;
			Matrix(int rows ,int cols,bool flag) ;
			~Matrix( ) ;
			int getRows() ;
			int getCols() ;
			int getValueAt( int row , int col) ;
			void setValueAt( int row , int col,int val) ;
			void randomFill( )  ;
			void randomFill( unsigned int seedp , unsigned int maxValue) ;
			static Matrix* multiply( Matrix &a , Matrix &b );
			static Matrix* multiply( Matrix &a , Matrix &b,Matrix *c);
			void print();
};

class MatrixMultiplier {
		private:
				int subIterations ;
				int iterations;
				long long *diffTime;
				unsigned int clockAccuracy;
				unsigned int timeAccuracy;
				int sleepTime;

		public:
				MatrixMultiplier(int subIters, int iters );
				void setDiffTime( long long* value);
				long long *getDiffTime( );
				void runIterations( );
				void printMeasuredTime();
				void runIterationsWithSleepTime( );
				void setSleepTime(int );
};
#endif
