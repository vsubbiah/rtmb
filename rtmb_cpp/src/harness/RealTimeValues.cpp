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
#include <RealTimeValues.h>

RealTimeValues::RealTimeValues() {
   acceptablePct = 0;
   rangePct = 0;
   multiplier = 1;
}

RealTimeValues::RealTimeValues(const RealTimeValues &rtValues_) {
   acceptablePct = rtValues_.acceptablePct;
   rangePct = rtValues_.rangePct;
   multiplier = rtValues_.multiplier;
}

float RealTimeValues::getAcceptablePercentage() {
   return acceptablePct;
}

void RealTimeValues::setAcceptablePercentage(float acceptablePct) {
   this ->acceptablePct = acceptablePct;
}

float RealTimeValues::getRangePercentage() {
   return rangePct;
}

void RealTimeValues::setRangePercentage(float rangePct) {
   this->rangePct = rangePct;
}

void RealTimeValues::setMultiplier(int multiplier_) {
   this -> multiplier = multiplier_;
}

int RealTimeValues::getMultiplier() {
   return this -> multiplier;
}
