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
 
#ifndef _TESTSCORE_H_
#define  _TESTSCORE_H_
#include <list>
#include <TestResults.h>

class TestScore {
        private:
                double consistencyPercent;
                double minThroughput;
                double maxThroughput;
                double meanThroughput;
                double wgtThroughput;
                double subTestScore;
                list <TestResults *> *tResult;
                string testScoreName;

        public:
                TestScore( list <TestResults *>  *result);
                TestScore( );
                double getConsistencyPercent() const;
                void setConsistencyPercent(double consistencyPercent);
                double getMinThroughput() const;
                void setMinThroughput(double minThroughput);
                double getMaxThroughput() const;
                void setMaxThroughput(double maxThroughput);
                double getMeanThroughput() const;
                void setMeanThroughput(double meanThroughput);
                double getWgtThroughput() const;
                void setWgtThroughput(double wgtThroughput);
                double getSubTestScore() const;
                void scoreForSubTest(double subTestScore);
                list <TestResults  *> *getTestResults() const;
                void setTestResults(list <TestResults *> *tResult);
                void setTestResults(TestResults *tResult);

                void setTestScoreName(string testScoreName);
                string getTestScoreName() const;
};
#endif
