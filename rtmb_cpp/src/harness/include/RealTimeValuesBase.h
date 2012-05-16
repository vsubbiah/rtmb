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
 
#ifndef __RT_VALUES_BASE__
#define __RT_VALUES_BASE__

#include <string>
#include <list>

using namespace std;

class RealTimeValuesBase {
public:
   virtual int getMultiplier()=0;
   virtual float getAcceptablePercentage()=0;
   virtual void setAcceptablePercentage(float acceptable_percentage)=0;
   virtual float getRangePercentage()=0;
   virtual void setRangePercentage(float range_percentage)=0;
};

#endif /*__RT_VALUES_BASE__*/
