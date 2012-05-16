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

/*
 * This micro-benchmark is intended to test the Java interpreter. The test 
 * verifies that the interpretation of various Java bytecodes is happening in a 
 * deterministic manner.
 * 
 * Since this micro-benchmark is meant to test the interpreter, we got to ensure
 * that no pre-compilation by the JIT happens in RTMB.loadAndCompileClasses().
 *  
 * The micro-benchmark broadly classifies the various Java bytecodes into the
 * following catagories:
 * 
 * 1. Combine the following load and store instructions along with arithmetic 
 *    instructions:
 *    
 *    [NOTE: For all operations on arrays (to measure stuff like iaload, iastore 
 *    etc) arraylength and iinc will be implicitly used.]
 *
 *		// Float related instructions.
 *		a. faload, fastore, fload, fstore, fadd, fsub and fneg. 
 *		b. faload, fastore, fload, fstore and fmul. 
 *		c. faload, fastore, fload, fstore, fdiv and frem. 
 *		
 *		// Double related instructions
 *		d. daload, dastore, dstore, dload, dadd, dsub and dneg.
 *		e. daload, dastore, dstore, dload and dmul.
 *		f. daload, dastore, dstore, dload, ddiv and drem.
 *		
 *		// Char, byte and short related instructions:
 *		g. caload, castore, cload, cstore,  baload and bastore.
 *		h. saload and sastore.
 *		
 *		// int related instructions.
 *		i. iaload, iastore, iload, istore, iadd, isub, ineg and 
 *		   the instructions like bipush, sipush etc can be used with some 
 *		   constants while we do the arithmetic.
 * 		j. iaload, iastore, iload, istore and imul.
 *		k. iaload, iastore, iload, istore, irem and idiv.
 *		
 *		// Long related instructions.
 *		l. lload, lstore, laload, lastore, lsub, lneg and ladd.
 *		m. lload, lstore, laload, lastore and lmul.
 *		n. lload, lstore, laload, lastore, ldiv and lrem.
 *		
 *		// Bit shift related:
 *		o. iload, istore, ishl, ishr and iushr.
 *		p. lload, lstore, lshl, lshr and lushr.
 *		
 *		// Logic related:
 *		q. iload, istore, iand, ior and ixor.
 *		r. lload, lstore, land, lor and lxor.
 * 
 * 2. Combine the instructions related to type conversions:
 * 		a. Convertions from type int to other primitive types: 
 *         	o i2b, i2s, i2d, i2l, i2c
 *         	o i2d, i2f
 *      b. Convertions from type long to other primitive types:
 *       	o l2d, l2f 
 *       	o l2i
 *      c. Convertions from type double to other primitive types:
 *       	o d2f, d2l
 *       	o d2i
 *      d. Convertions from type float to other primitive types:
 *        	o f2d, f2l
 *        	o f2i
 * 
 * 3. Bytecode instructions related to casts:
 * 		a. instanceof 
 * 		b. checkcast
 * 
 * 4. Bytecode instructions related to method invocation:
 * 		a. invokeinterface
 * 		b. invokespecial
 * 		c. invokevirtual
 * 		d. invokestatic
 * 
 * 5. Bytecode instructions related to control transfer:
 * 		a. if_icmp and return.
 * 		b. lcmp and lreturn.
 * 		c. dcmp and dreturn.
 * 		d. fcmp and freturn.
 * 		e. ifnonnull and areturn.
 * 		f. ifnull and areturn.
 * 		g. if<cond> and areturn.
 * 		h. if_acmp and goto
 * 		i. if_acmp and goto_w.
 * 		j. lookupswitch
 * 		k. tableswitch
 * 		l. ifne and ifeq.
 * 
 * 6. Bytecode instructions related to object creation:
 * 		a. new
 * 		b. newarray
 * 		c. anewarray
 * 		d. multianewarray
 * 		e. new and athrow.
 * 
 * 7. Bytecode instructions related to object access:
 * 		a. getfield, aload, astore, aastore and aaload
 * 		b. getstatic, aload, astore, aaload and aastore.
 * 		c. putfield, aload, astore, aaload and aastore.
 * 		d. putstatic, aload, astore, aaload and aastore.
 *
 * @TODO
 * Miscellaneous bytecode instructions that are not implemented _yet_:
 * 		
 * 		dup, dup2, nop, wide, ret, jsr, jsr_w, pop and pop2.
 * 
 * Mike's advice:
 * 
 * Anyone who is trying to implement the jsr or jsr_w, will need to either write 
 * bytecode by hand, or alternately, grab an old javac compiler (perhaps 
 * pre-1.4.2) and write some code with 'finally', then save the bytecode for 
 * inclusion in the test. It might be cleaner/simpler to just write it by hand 
 * and get a bytecode compiler to generate bytecode for you.
 */

package com.ibm.realtime.rtmb.tests;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Random;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.tests.util.BCDummyInterface;
import com.ibm.realtime.rtmb.tests.util.BCDummyInterfaceImpl;
import com.ibm.realtime.rtmb.tests.util.BytecodeConsistencyResults;
import com.ibm.realtime.rtmb.tests.util.BytecodeConsistencySubtest;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.util.NoPreCompileAnnotation;
import com.ibm.realtime.rtmb.util.VerbosePrint;

@NoPreCompileAnnotation
public class BytecodeConsistencyTest extends SpecTest {
	private static final short initialSubIterations = 1000;
	private static final long seed = 1024 * 1024 * 100;
	static Random r = new Random(seed);
	private static final Class<?> testNames [] = {intArithmeticTest.class, 
		intArithmeticMulTest.class, 
		intArithmeticDivTest.class,
		floatArithmeticTest.class, 
		floatArithmeticMulTest.class,
		floatArithmeticDivTest.class, 
		longArithmeticTest.class, 
		longArithmeticMulTest.class, 
		longArithmeticDivTest.class, 
		doubleArithmeticTest.class, 
		doubleArithmeticMulTest.class,
		doubleArithmeticDivTest.class, 
		shortLoadAndStoreInstTest.class,
		charByteLoadAndStoreInstTest.class, 
		intBitShiftInstTest.class, 
		longBitShiftInstTest.class, 
		intLogicInstTest.class, 
		longLogicInstTest.class, 
		intToFloatingTypeConvTest.class, 
		intToOtherTypeConvTest.class, 
		longToFloatingTypeConvTest.class, 
		longToOtherTypeConvTest.class, 
		doubleToFloatingTypeConvTest.class, 
		doubleToOtherTypeConvTest.class,
		floatToFloatingTypeConvTest.class, 
		floatToOtherTypeConvTest.class, 
		instanceOfTest.class,
		checkCastTest.class,
		invokeInterfaceTest.class, 
		invokeSpecialTest.class, 
		invokeVirtualTest.class, 
		invokeStaticTest.class, 
		compareAndReturnTest.class, 
		compareLongAndReturnTest.class, 
		compareDoubleAndReturnTest.class, 
		compareFloatAndReturnTest.class, 
		compareEqAndNeqTest.class, 
		nonNullCompareAndReturnTest.class,
		nullCompareAndReturnTest.class,
		compareAndGoto.class,
		lookupSwitchTest.class, 
		tableSwitchTest.class, 
		/*jsrAndJsrWideTest.class,*/
		newTest.class, 
		newArrayTest.class, 
		anewArrayTest.class, 
		newMultiObjectArray.class,
		throwExceptionTest.class, 
		getAndPutFieldTest.class, 
		getAndPutStaticTest.class};

	public static Random getRandom() {
		return r;
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,
			TestParams tParams) {
		updateIterations(tInfo, rtValues, tParams);
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());		

		BytecodeConsistencyResults results = new BytecodeConsistencyResults(testNames.length, tInfo.getTestIterations());
		TestResults testResult = null;
		boolean retryMsg = false;
		for (int i = 0; i < testNames.length; i++) {
			retryMsg = false;
			BytecodeConsistencySubtest subTest = null;
			String testName = testNames[i].getSimpleName();
			Class<?> [] parameterTypes = {Integer.class};
			Constructor<?> ctr = null;
			int subIterations = initialSubIterations;
			try {
				ctr = testNames[i].getConstructor(parameterTypes);
				subTest = (BytecodeConsistencySubtest) ctr.newInstance(Integer.valueOf(subIterations));
			} catch (SecurityException e1) {
				e1.printStackTrace();
			} catch (NoSuchMethodException e1) {
				e1.printStackTrace();
			} catch (IllegalArgumentException e1) {
				e1.printStackTrace();
			} catch (InstantiationException e1) {
				e1.printStackTrace();
			} catch (IllegalAccessException e1) {
				e1.printStackTrace();
			} catch (InvocationTargetException e1) {
				e1.printStackTrace();
			}
			long startTime = 0;
			long endTime = 0;
			int count = 0;
			testResult = results.getNextTestResult();
			testResult.setTestResultName(testName);
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"BytecodeConsistencyTest: Test report for " + testResult.getTestResultName() + " subtest");
			
			subTest.setSubIterations(initialSubIterations);			
			while (true) {
				if (count >= tInfo.getThreshold()) {
					break;
				}
				testResult.resetResults();
				
				if (retryMsg) {
					VerbosePrint.testRetry(System.out, tParams, testName); 
				}
				/*
				 * We are only interested in measuring consistency of
				 * execution of bytecodes. We do not want GC to spoil
				 * our plans. So, try to clear up the garbage outside of time
				 * calculation.
				 */
				System.gc();
				for (int loop = 0; loop < tInfo.getTestIterations(); loop++) {
					long timeDiff;
					startTime = System.nanoTime();
					subTest.bcTest();
					endTime = System.nanoTime();
					/* Get the elapsed time in nsecs */
					timeDiff = endTime - startTime;
					
					VerbosePrint.diffTime(System.out, tParams, testName, timeDiff);					

					testResult.setValuesAtNextIndex(timeDiff);
				}
				/*
				 * We now need to determine if minimum time taken to perform the 
				 * above operation took longer than the maximum time taken for the
				 * clock config test. If no, then the task quantum should be 
				 * increased and the test redone.
				 */
				if (testResult.getMedian() > ClockConfig.getClockAccuracy()) {
					retryMsg = true;
					count++;
					if (testResult.checkStdDev(rtValues, tParams)) {
						retryMsg = false;
						break;
					}
				}
				subTest.setSubIterations(subTest.getSubIterations() * 2);
			}
		}
		return results.getResults();
	}

	@Override
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] tResult) {
		TestResults [] tmpTestResults = new TestResults[1];
		TestScores [] bytecodeConsistencyScore = new TestScores[tResult.length];
		TestScores [] dummyTestScore;
		for (int i = 0; i < tResult.length; i++) {
			tmpTestResults[0] = tResult[i];
			dummyTestScore = super.generateCombinedScore(tInfo, tmpTestResults);
			bytecodeConsistencyScore[i] = dummyTestScore[0];
			bytecodeConsistencyScore[i].setTestScoreName(testNames[i].getSimpleName());
		}
		return bytecodeConsistencyScore;
	}

	@Override
	public String name() {
		return "BytecodeConsistencyTest";
	}
}

class intArithmeticTest extends BytecodeConsistencySubtest {
	private int i;
	private int [] intArray = null;

	public intArithmeticTest(Integer subI) {
		super (subI.intValue());
		this.i = BytecodeConsistencyTest.getRandom().nextInt(100);
		subIterations = subI.intValue();
		this.intArray = new int[subI];			
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.intArray = new int[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < intArray.length; count++) {
			intArray[count] = (intArray.length - count) + 100;
			i = 1703 - (i + intArray[count]);
			intArray[count] = -i;
		}
	}
}

class intArithmeticMulTest extends BytecodeConsistencySubtest {
	private int i;
	private int [] intArray = null;

	public intArithmeticMulTest(Integer subI) {
		super (subI.intValue());
		this.i = BytecodeConsistencyTest.getRandom().nextInt(100);
		this.intArray = new int[subIterations];			
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.intArray = new int[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			intArray[count-1] = i * count;
			i = intArray[count-1];
		}			
	}
}

class intArithmeticDivTest extends BytecodeConsistencySubtest {
	private int i;
	private int [] intArray = null;
	private int modFactor;

	public intArithmeticDivTest(Integer subI) {
		super (subI.intValue());
		this.i = BytecodeConsistencyTest.getRandom().nextInt(100);
		this.intArray = new int[subIterations];
		this.modFactor = BytecodeConsistencyTest.getRandom().nextInt();
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.intArray = new int[subI];
	}
	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			intArray[count-1] = i / count;
			i = intArray[count-1] % modFactor;
		}
	}
}

class floatArithmeticTest extends BytecodeConsistencySubtest {
	private float f;
	private float [] fltArray = null;

	public floatArithmeticTest(Integer subI) {
		super (subI.intValue());
		this.f = BytecodeConsistencyTest.getRandom().nextFloat();
		this.fltArray = new float[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < fltArray.length; count++) {
			fltArray[count] = (fltArray.length - count) + 100;
			f = 1703 - (f + fltArray[count]);
			fltArray[count] = -f;
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.fltArray = new float[subIterations];
	}
}

class floatArithmeticMulTest extends BytecodeConsistencySubtest {
	private float f;
	private float [] fltArray = null;

	public floatArithmeticMulTest(Integer subI) {
		super (subI.intValue());
		this.f = BytecodeConsistencyTest.getRandom().nextFloat();
		this.fltArray = new float[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			fltArray[count-1] = f * count;
			this.f = fltArray[count-1];
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.fltArray = new float[subIterations];
	}
}

class floatArithmeticDivTest extends BytecodeConsistencySubtest {
	private float f;
	private float [] fltArray = null;

	public floatArithmeticDivTest(Integer subI) {
		super (subI.intValue());
		this.f = BytecodeConsistencyTest.getRandom().nextFloat();
		this.fltArray = new float[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			fltArray[count-1] = f / count;
			this.f = fltArray[count-1] % BytecodeConsistencyTest.getRandom().nextFloat();
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.fltArray = new float[subIterations];
	}
}

class longArithmeticTest extends BytecodeConsistencySubtest {
	private long l;
	private long [] lArray = null;

	public longArithmeticTest(Integer subI) {
		super (subI.intValue());
		this.l = BytecodeConsistencyTest.getRandom().nextLong();
		this.lArray = new long[subIterations];			
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.lArray = new long[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < lArray.length; count++) {
			lArray[count] = (lArray.length - count) + 100;
			this.l = 1703 - (this.l + lArray[count]);
			lArray[count] = -l;
		}
	}
}

class longArithmeticMulTest extends BytecodeConsistencySubtest {
	private long l;
	private long [] longArray = null;

	public longArithmeticMulTest(Integer subI) {
		super (subI.intValue());
		this.l = BytecodeConsistencyTest.getRandom().nextLong();
		this.longArray = new long[subIterations];			
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.longArray = new long[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			longArray[count-1] = this.l * count;
			this.l = longArray[count-1];
		}
	}
}

class longArithmeticDivTest extends BytecodeConsistencySubtest {
	private long l;
	private long [] lArray = null;
	private long modFactor;

	public longArithmeticDivTest(Integer subI) {
		super (subI.intValue());
		this.l = BytecodeConsistencyTest.getRandom().nextLong();
		this.lArray = new long[subIterations];
		this.modFactor = BytecodeConsistencyTest.getRandom().nextLong();
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.lArray = new long[subIterations];
	}
	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			lArray[count-1] = l / count;
			l = lArray[count-1] % modFactor;
		}
	}
}

class doubleArithmeticTest extends BytecodeConsistencySubtest {
	private double d;
	private double [] dArray = null;

	public doubleArithmeticTest(Integer subI) {
		super (subI.intValue());
		this.d = BytecodeConsistencyTest.getRandom().nextDouble();
		this.dArray = new double[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < dArray.length; count++) {
			dArray[count] = (dArray.length - count) + 100;
			d = 1703 - (d + dArray[count]);
			dArray[count] = -d;
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.dArray = new double[subIterations];
	}
}

class doubleArithmeticMulTest extends BytecodeConsistencySubtest {
	private double d;
	private double [] dArray = null;

	public doubleArithmeticMulTest(Integer subI) {
		super (subI.intValue());
		this.d = BytecodeConsistencyTest.getRandom().nextDouble();
		this.dArray = new double[subIterations];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			dArray[count-1] = d * count;
			d = dArray[count-1];
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.dArray = new double[subI];
	}
}

class doubleArithmeticDivTest extends BytecodeConsistencySubtest {
	private double d;
	private double [] dArray = null;
	private double modFactor;

	public doubleArithmeticDivTest(Integer subI) {
		super (subI.intValue());
		this.d = BytecodeConsistencyTest.getRandom().nextDouble();
		this.dArray = new double[subI.intValue()];
		this.modFactor = BytecodeConsistencyTest.getRandom().nextFloat();
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			dArray[count-1] = d / count;
			d = dArray[count-1] % modFactor;
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.dArray = new double[subI];
	}
}

class shortLoadAndStoreInstTest extends BytecodeConsistencySubtest {
	private short [] sArray = null;

	public shortLoadAndStoreInstTest(Integer subI) {
		super (subI.intValue());
		this.sArray = new short[subIterations];
	}

	@Override
	public void bcTest() {
		short s = 100;
		for (int count = 1; count <= subIterations; count++) {
			sArray[count-1] = s;
			s = sArray[count-1];
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		this.sArray = new short[subI];
	}
}

class charByteLoadAndStoreInstTest extends BytecodeConsistencySubtest {
	// tmp,c,b,x not used outside this object, but by making it public, may prevent compiler optimization		
	public char [] c = null;
	public byte [] b = null;
	public char tmp;
	public byte x;

	public charByteLoadAndStoreInstTest(Integer subI) {
		super (subI.intValue());
		c = new char[subI.intValue()];
		b = new byte[subI.intValue()];
	}

	@Override
	public void bcTest() {
		for (int count = 1; count <= subIterations; count++) {
			char c = 98;
			this.c[count-1] = c;
			tmp = this.c[count-1];
			this.b[count-1] = 'a';
			x = this.b[count-1];
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI;
		c = new char[subIterations];
		b = new byte[subIterations];
	}
}

class intBitShiftInstTest extends BytecodeConsistencySubtest {
	private int shiftingInt;
	private int shiftAmt;

	public intBitShiftInstTest(Integer subI) {
		super (subI.intValue());
		this.shiftingInt = BytecodeConsistencyTest.getRandom().nextInt();
		this.shiftAmt = BytecodeConsistencyTest.getRandom().nextInt(5);
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			int x = (shiftingInt >> shiftAmt);
			shiftingInt<<= x;
			x>>>= shiftAmt;
		}
	}
}

class longBitShiftInstTest extends BytecodeConsistencySubtest {
	private long shiftingInt;
	private long shiftAmt;

	public longBitShiftInstTest(Integer subI) {
		super (subI.intValue());
		this.shiftingInt = BytecodeConsistencyTest.getRandom().nextInt();
		this.shiftAmt = BytecodeConsistencyTest.getRandom().nextInt(5);
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			long x = (shiftingInt >> shiftAmt);
			shiftingInt<<= x;
			x>>>= shiftAmt;
		}
	}
}

class intLogicInstTest extends BytecodeConsistencySubtest {
	private int int1, int2;
	private int mask;

	public intLogicInstTest(Integer subI) {
		super (subI.intValue() * 10);
		this.int1 = BytecodeConsistencyTest.getRandom().nextInt();
		this.int2 = BytecodeConsistencyTest.getRandom().nextInt();
		this.mask = 0x00FFFFFF;
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			this.int1 = this.int2 & this.mask;
			this.int2 = this.int1 | this.mask;
			this.int1 = this.int2 ^ this.mask;
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI * 10;
		this.mask>>= 0xF;
	}
}

class longLogicInstTest extends BytecodeConsistencySubtest {
	private long int1, int2;
	private long mask;

	public longLogicInstTest(Integer subI) {
		super (subI.intValue() * 10);
		this.int1 = BytecodeConsistencyTest.getRandom().nextInt();
		this.int2 = BytecodeConsistencyTest.getRandom().nextInt();
		this.mask = 0x00FFFFFF;
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			this.int1 = this.int2 & this.mask;
			this.int2 = this.int1 | this.mask;
			this.int1 = this.int2 ^ this.mask;
		}
	}

	@Override
	public void setSubIterations(int subI) {
		subIterations = subI * 10;
		this.mask>>= 0xF;
	}
}

class intToFloatingTypeConvTest extends BytecodeConsistencySubtest {
	// f, d not used outside this object, but by making it public, may prevent compiler optimization	
	public float f;
	public double d;
	public intToFloatingTypeConvTest (Integer subI) {
		super (subI.intValue());
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			f = count;
			d = count;
		}
	}
}

class intToOtherTypeConvTest extends BytecodeConsistencySubtest {
	// b, c, l, s not used outside this object, but by making it public, may prevent compiler optimization	
	public byte b; 
	public char c;
	public long l;
	public short s;
	
	public intToOtherTypeConvTest (Integer subI) {
		super (subI.intValue());
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			b = (byte) count;
			c = (char) count;
			l = count;
			s = (short) count;
		}
	}
}

class longToFloatingTypeConvTest extends BytecodeConsistencySubtest {
	// f,d,l not used outside this object, but by making it public, may prevent compiler optimization	
	public long l;
	public float f;
	public double d;

	public longToFloatingTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.l = BytecodeConsistencyTest.getRandom().nextLong();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			f = l;
			d = l;
		}
	}
}

class longToOtherTypeConvTest extends BytecodeConsistencySubtest {
	// l, i not used outside this object, but by making it public, may prevent compiler optimization	
	public long l;
	public int i;

	public longToOtherTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.l = BytecodeConsistencyTest.getRandom().nextLong();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			i = (int) l;
		}
	}
}

class doubleToFloatingTypeConvTest extends BytecodeConsistencySubtest {
	// f,d not used outside this object, but by making it public, may prevent compiler optimization	
	public double d;
	public float f;

	public doubleToFloatingTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.d = BytecodeConsistencyTest.getRandom().nextDouble();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			f = (float) d;
		}
	}
}

class doubleToOtherTypeConvTest extends BytecodeConsistencySubtest {
	// l, i,d not used outside this object, but by making it public, may prevent compiler optimization		
	public double d;
	public long l;
	public int i;

	public doubleToOtherTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.d = BytecodeConsistencyTest.getRandom().nextDouble();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			i = (int) d;
			l = (long) d;
		}
	}
}

class floatToFloatingTypeConvTest extends BytecodeConsistencySubtest {	
	// f,d not used outside this object, but by making it public, may prevent compiler optimization		
	public float f;
	public double d;

	public floatToFloatingTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.f = BytecodeConsistencyTest.getRandom().nextFloat();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			d = f;
		}
	}
}

class floatToOtherTypeConvTest extends BytecodeConsistencySubtest {
	// f,i,l not used outside this object, but by making it public, may prevent compiler optimization		
	public float f;
	public int i;
	public long l;

	public floatToOtherTypeConvTest (Integer subI) {
		super (subI.intValue());
		this.f = BytecodeConsistencyTest.getRandom().nextFloat();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			i = (int) f;
			l = (long) f;
		}
	}
}

class instanceOfTest extends BytecodeConsistencySubtest {
	public Object iot;
	public instanceOfTest() {
		super();
	}

	public instanceOfTest (Integer subI) {
		super (subI.intValue());
		/*
		 * Create a dummy object of the same class type
		 * Put an alternate type of object into iot first to make it harder 
		 * for an optimizer to optimize out the instanceof check
		 */
		this.iot = new Object();
		this.iot = new instanceOfTest();
	}

	@Override
	public void bcTest() {
		for (int count = 0; count < subIterations; count++) {
			if (!(this.iot instanceof instanceOfTest)) {
				System.err.println("Internal Error. Bytecode test returned false for instanceof");
			}
		}
	}
}

class checkCastTest extends BytecodeConsistencySubtest {
	private checkCastTest test;
	public checkCastSubClass ccsc;

	public checkCastTest() {
		super();
	}

	public checkCastTest(Integer subI) {
		super (subI.intValue());
		this.test = new checkCastSubClass();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			ccsc = (checkCastSubClass) this.test;
		}
	}

	private static class checkCastSubClass extends checkCastTest {
		public checkCastSubClass() {
			super();
		}
	}
}

class invokeInterfaceTest extends BytecodeConsistencySubtest {
	private BCDummyInterface bci = BCDummyInterfaceImpl.getInstance();

	public invokeInterfaceTest (Integer subI) {
		super(subI.intValue());
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			bci.dummyMethod();
		}
	}
}

class invokeSpecialTest extends BytecodeConsistencySubtest {
	public invokeSpecialTest(Integer subI) {
		super (subI.intValue());
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			dummyMethod();
		}
	}

	private void dummyMethod() {
		// Do nothing here.
	}
}

class invokeStaticTest extends BytecodeConsistencySubtest {
	public invokeStaticTest (Integer subI) {
		super (subI.intValue());
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			dummyMethod();
		}
	}

	private static void dummyMethod() {
		// Do nothing here.
	}
}

class invokeVirtualTest extends BytecodeConsistencySubtest {
	private invokeVirtualTestSubClass ivtsc;

	public invokeVirtualTest (Integer subI) {
		super (subI.intValue());
		this.ivtsc = new invokeVirtualTestSubClass();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			ivtsc.dummyMethod();
		}
	}

	private static class invokeVirtualTestSubClass {
		private void dummyMethod() {
			// Do nothing here.
		}
	}
}

class compareAndReturnTest extends BytecodeConsistencySubtest {
	private int x;
	private int y;

	public compareAndReturnTest(Integer subI) {
		super (subI.intValue());
		this.x = BytecodeConsistencyTest.getRandom().nextInt();
		this.y = BytecodeConsistencyTest.getRandom().nextInt();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != this.y) {
				//Nothing
			}
			if (this.x == this.y) {
				// Nothing
			}
		}
	}
}

class compareLongAndReturnTest extends BytecodeConsistencySubtest {
	private long x;

	public compareLongAndReturnTest(Integer subI) {
		super (subI.intValue());
		this.x = BytecodeConsistencyTest.getRandom().nextLong();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != 100) {
				getX();
			}
		}
	}

	private long getX() {
		return x;
	}
}

class compareDoubleAndReturnTest extends BytecodeConsistencySubtest {
	private double x;

	public compareDoubleAndReturnTest(Integer subI) {
		super (subI.intValue());
		this.x = BytecodeConsistencyTest.getRandom().nextDouble();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != 100) {
				getX();
			}
		}
	}

	private double getX() {
		return x;
	}
}

class compareFloatAndReturnTest extends BytecodeConsistencySubtest {
	private float x;

	public compareFloatAndReturnTest(Integer subI) {
		super (subI.intValue());
		this.x = BytecodeConsistencyTest.getRandom().nextFloat();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != 100) {
				getX();
			}
		}
	}

	private float getX() {
		return x;
	}
}

class nonNullCompareAndReturnTest extends BytecodeConsistencySubtest {
	private Object x;

	public nonNullCompareAndReturnTest (Integer subI) {
		super (subI.intValue());
		this.x = new Object();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x == null) {
			} else {
				getX();
			}
		}
	}

	private Object getX() {
		return x;
	}
}

class nullCompareAndReturnTest extends BytecodeConsistencySubtest {
	private Object x;

	public nullCompareAndReturnTest (Integer subI) {
		super (subI.intValue());
		this.x = new Object();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != null) {
				getX();
			}
		}
	}

	private Object getX() {
		return x;
	}
}

class compareAndGoto extends BytecodeConsistencySubtest {
	volatile private Object x;
	volatile private Object y;


	public compareAndGoto (Integer subI) {
		super (subI.intValue());
		this.x = new Object();
		this.y = new Object();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			RTMB:
			{
			if (this.x != this.y) {
				break RTMB;				
			}
			}
		}
	}
}

class compareEqAndNeqTest extends BytecodeConsistencySubtest {
	private long x;
	private long y;

	public compareEqAndNeqTest (Integer subI) {
		super (subI.intValue());
		this.x = BytecodeConsistencyTest.getRandom().nextLong();
		this.y = BytecodeConsistencyTest.getRandom().nextLong();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			if (this.x != this.y) {
				// Do nothing
			}
			if (this.x == this.y) {
				// Do nothing
			}
		}
	}
}

class lookupSwitchTest extends BytecodeConsistencySubtest {
	private int number;

	public lookupSwitchTest(Integer subI) {
		super (subI.intValue());
		this.number = BytecodeConsistencyTest.getRandom().nextInt(100);
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			switch (this.number) {
			case 10:
				break;
			case 30:
				break;
			case 50:
				break;
			case 100:
				break;
			default:
				break;
			}
		}
	}
}

class tableSwitchTest extends BytecodeConsistencySubtest {
	private int number;

	public tableSwitchTest (Integer subI) {
		super (subI);
		this.number = BytecodeConsistencyTest.getRandom().nextInt(100);
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			switch (this.number) {
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			default:
				break;
			}
		}
	}
}

class newTest extends BytecodeConsistencySubtest {
	public Object obj; // not used outside this object, but by making it public, may prevent compiler optimization

	public newTest (Integer subI) {
		super (subI);
	}
	@Override
	public void bcTest() {	
		for (int i = 0; i < subIterations; i++) {
			obj = new Object();
		}
	}
}

class newArrayTest extends BytecodeConsistencySubtest {
	public int integers[]; // not used outside this object, but by making it public, may prevent compiler optimization

	public newArrayTest (Integer subI) {
		super (subI);
	}
	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {	
			integers = new int[1];
		}
	}
}

class anewArrayTest extends BytecodeConsistencySubtest {
	public Object obj[]; // not used outside this object, but by making it public, may prevent compiler optimization

	public anewArrayTest (Integer subI) {
		super (subI);
	}
	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {	
			obj = new Object[1];
		}
	}
}

class newMultiObjectArray extends BytecodeConsistencySubtest {
	public Object obj[][]; // not used outside this object, but by making it public, may prevent compiler optimization

	public  newMultiObjectArray (Integer subI) {
		super (subI);
	}
	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {	
			obj = new Object[1][1];
		}
	}
}

class throwExceptionTest extends BytecodeConsistencySubtest {

	public throwExceptionTest (Integer subI) {
		super (subI);
	}
	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations; i++) {
			try {
				throw new Exception();			
			} catch (Exception e) {
			}
		}
	}
}

class getAndPutStaticTest extends BytecodeConsistencySubtest {
	private static int staticInteger;
	// x is not used outside this object, but by making it public, may prevent compiler optimization		
	public int x;

	public getAndPutStaticTest (Integer subI) {
		super (subI);
		staticInteger = BytecodeConsistencyTest.getRandom().nextInt();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations;) {
			x = staticInteger;
			staticInteger = i++;
		}
	}
}

class getAndPutFieldTest extends BytecodeConsistencySubtest {
	private int integer;
	// x is not used outside this object, but by making it public, may prevent compiler optimization		
	public int x;	

	public getAndPutFieldTest (Integer subI) {
		super (subI);
		integer = BytecodeConsistencyTest.getRandom().nextInt();
	}

	@Override
	public void bcTest() {
		for (int i = 0; i < subIterations;) {
			x = integer;
			integer = i++;
		}
	}
}
