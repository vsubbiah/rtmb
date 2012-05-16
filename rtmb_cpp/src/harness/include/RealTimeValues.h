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
 
#ifndef __RT_VALUES__
#define __RT_VALUES__

#include <RealTimeValuesBase.h>

using namespace std;

class RealTimeValues: public RealTimeValuesBase {

private:
   float acceptablePct;
   float rangePct;
   int multiplier;

public:
   /* Default constructor */
   RealTimeValues();

   /** Copy constructor */
   RealTimeValues(const RealTimeValues &);

   /** Sets real time multiplier factor */
   void setMultiplier(int);

   /** Returns real time multiplier factor */
   int getMultiplier();

   /** get the real time acceptable percentage */
   float getAcceptablePercentage();

   /** set the real time acceptable percentage */
   void setAcceptablePercentage(float);

   /** get the percentage of acceptable range */
   float getRangePercentage();

   /** set the percentage of acceptable range */
   void setRangePercentage(float range_percentage);
};

#endif /*__RT_VALUES__*/
